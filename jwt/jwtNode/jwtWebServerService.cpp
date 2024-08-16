#include "Events/System/Net/rpcEvent.h"
#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include <map>
#include "jwtWebServerService.h"
#include "events_jwtWebServer.hpp"
#include "version_mega.h"



bool jwtWebServer::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    sendEvent(ServiceEnum::HTTP,new httpEvent::DoListen(bindAddr,ListenerBase::serviceId));

    return true;
}


bool jwtWebServer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
        {
            const timerEvent::TickTimer*ev=static_cast<const timerEvent::TickTimer*>(e.get());
            if(ev->tid==TIMER_PUSH_NOOP)
            {
                for(auto &z: sessions)
                {
                    auto esi=z.second->esi;
                    if(esi.valid())
                    {
                        if(!esi->closed())
                        {
                            esi->write_("   ");
                        }
                    }
                    else logErr2("!if(esi.valid())");
                }
            }
            return true;
        }
        if(httpEventEnum::RequestIncoming==ID)
            return on_RequestIncoming((const httpEvent::RequestIncoming*)e.get());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());


        if(jwtEventEnum::TokenAddedRSP==ID)
            return on_TokenAddedRSP((const jwtEvent::TokenAddedRSP*)e.get());

        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.get();
            auto& IDC=E->e->id;
            if(jwtEventEnum::TokenAddedRSP==IDC)
                return on_TokenAddedRSP((const jwtEvent::TokenAddedRSP*)E->e.get());


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.get();
            auto& IDA=E->e->id;
            if(jwtEventEnum::TokenAddedRSP==IDA)
                return on_TokenAddedRSP((const jwtEvent::TokenAddedRSP*)E->e.get());


            return false;
        }


    }
    catch(CommonError& e)
    {
        logErr2("jwtWebServer CommonError  %s",e.what());

    }
    catch(std::exception &e)
    {
        logErr2("jwtWebServer std::exception  %s",e.what());
    }
    XPASS;
    return false;
}

jwtWebServer::Service::~Service()
{
}


jwtWebServer::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(nm,id),
    Broadcaster(ins)
{
    auto ba=ins->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","http listen address");
    if(ba.size()==0)
        throw CommonError("if(ba.size()==0)");

    bindAddr=*ba.begin();
    jwtServerAddr=ins->getConfig()->get_string("jwtServerAddr","local","server jwt address");

}

void registerjwtServerWebService(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::jwtServerWeb,"jwtServerWeb",getEvents_jwtWebServer());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::jwtServerWeb,jwtWebServer::Service::construct,"jwtServerWeb");
        regEvents_jwtWebServer();
    }
    XPASS;
}






std::string index_html=R"ZXC(
<!DOCTYPE html>
<html>
<head>
<title>jwt</title>
</head>

<body>

    <div>
     starting handling request
    </div>
</body>
</html>
)ZXC";

bool jwtWebServer::Service::on_RequestIncoming(const httpEvent::RequestIncoming*e)
{

    HTTP::Response resp(getIInstance());
    auto S=check_session(e->req,resp,e->esi);
//    for(auto &z: e->req->headers)
//    {
//        printf("%s: %s\n",z.first.c_str(),z.second.c_str());
//    }
    S->esi=e->esi;

    if(1){

        std::string query_string=e->req->params["query_string"];

//        for(int i=0;i<10;i++)
        {
            sendEvent(jwtServerAddr,ServiceEnum::jwtServer,new jwtEvent::AddTokenREQ(S->sessionId,
                                                                                                   query_string,0,ListenerBase::serviceId));
        }
        return true;
    }
    else
    {
        bool keepAlive=e->req->headers["CONNECTION"]=="Keep-Alive";
        keepAlive=true;
        if(keepAlive)
        {
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";
        }
        resp.content="<div>received response </div>";
//        logErr2("resp:%s",resp.build_html_response().c_str());
        if(keepAlive)
            resp.makeResponsePersistent(e->esi);
        else
            resp.makeResponse(e->esi);

    }

    return true;
}


static int cnt2;
REF_getter<jwtWebServer::Session> jwtWebServer::Service::check_session( const REF_getter<HTTP::Request>& req, HTTP::Response& resp, const REF_getter<epoll_socket_info>& esi)
{

    auto session_id=std::to_string(cnt2++);
    REF_getter<Session>S=new Session(session_id,req,esi);
    {
//        logErr2("insert into sessions %s",iUtils->bin2hex(session_id).c_str());
        sessions.insert(std::make_pair(session_id,S));
    }
    return S;

}
REF_getter<jwtWebServer::Session> jwtWebServer::Service::get_session( const std::string& session_id)
{

    REF_getter<Session> S(nullptr);
    auto it=sessions.find(session_id);
    if(it!=sessions.end())
        S=it->second;
    else
    {
        throw CommonError("session not found %s",session_id.c_str());
    }
    return S;

}

bool jwtWebServer::Service::on_TokenAddedRSP(const jwtEvent::TokenAddedRSP*e)
{
    if(e->count==0)
    {
        HTTP::Response resp(getIInstance());
        auto S=get_session(e->session);
        bool keepAlive=S->req->headers["CONNECTION"]=="Keep-Alive";
        keepAlive=true;
        if(keepAlive)
        {
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";
        }
        resp.content="<div>received response </div>";
//        logErr2("resp:%s",resp.build_html_response().c_str());
        if(keepAlive)
            resp.makeResponsePersistent(S->esi);
        else
            resp.makeResponse(S->esi);

    }
    else
    {
        sendEvent(jwtServerAddr,ServiceEnum::jwtServer,new jwtEvent::AddTokenREQ(e->session,e->sampleString,e->count-1,ListenerBase::serviceId));

    }

    return true;
}
