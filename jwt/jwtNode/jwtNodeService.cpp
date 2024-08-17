#include "Events/System/Net/rpcEvent.h"
#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include <map>
#include "jwtWebServerService.h"
#include "events_jwtWebServer.hpp"
#include "version_mega.h"



bool jwtNode::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    sendEvent(ServiceEnum::HTTP,new httpEvent::DoListen(bindAddr,ListenerBase::serviceId));
    sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(T_PING,NULL,NULL,ping_timeout,this));

    sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::Ping(this));


    return true;
}

bool jwtNode::Service::TickTimer(const timerEvent::TickTimer *e)
{
    sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::Ping(this));
    return true;
}
bool jwtNode::Service::AddTokenRSP(const jwtEvent::AddTokenRSP* e)
{
    int64_t mylastId=0;
    if(id_2_ur.size())
    {
        mylastId=id_2_ur.rbegin()->first;
    }
    if(e->lastId>mylastId)
    {
        sendEvent(jwtBossAddr,ServiceEnum::jwtBoss, new jwtEvent::GetUrSinceREQ(mylastId,ListenerBase::serviceId));
    }
    return true;
}
bool jwtNode::Service::GetUrSinceRSP(const jwtEvent::GetUrSinceRSP* e)
{
    for(auto& z:e->container)
    {
        REF_getter<P_user_rec> u=new P_user_rec;
        u->ur=std::move(z.second);
        user_2_ur.insert({u->ur.login,u});
        id_2_ur.insert({z.first,u});
    }
    int64_t li=0;
    if(id_2_ur.size())
    {
        li=id_2_ur.rbegin()->first;
    }
    if(li<e->lastId)
    {
        sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::GetUrSinceREQ(li,ListenerBase::serviceId));
    }
    return true;
}

bool jwtNode::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
            return TickTimer(static_cast<const timerEvent::TickTimer*>(e.get()));

        if(httpEventEnum::RequestIncoming==ID)
            return on_RequestIncoming((const httpEvent::RequestIncoming*)e.get());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());

        if(jwtEventEnum::AddTokenRSP==ID)
            return AddTokenRSP((const jwtEvent::AddTokenRSP*)e.get());

        if(jwtEventEnum::TokenAddedRSP==ID)
            return on_TokenAddedRSP((const jwtEvent::TokenAddedRSP*)e.get());

        if(jwtEventEnum::AddTokenRSP==ID)
            return AddTokenRSP((const jwtEvent::AddTokenRSP*)e.get());


        if(jwtEventEnum::GetUrSinceRSP==ID)
            return GetUrSinceRSP((const jwtEvent::GetUrSinceRSP*)e.get());


        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.get();
            auto& IDC=E->e->id;
            if(jwtEventEnum::TokenAddedRSP==IDC)
                return on_TokenAddedRSP((const jwtEvent::TokenAddedRSP*)E->e.get());
            if(jwtEventEnum::AddTokenRSP==IDC)
                return AddTokenRSP((const jwtEvent::AddTokenRSP*)E->e.get());
            if(jwtEventEnum::GetUrSinceRSP==IDC)
                return GetUrSinceRSP((const jwtEvent::GetUrSinceRSP*)E->e.get());


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.get();
            auto& IDA=E->e->id;
            if(jwtEventEnum::TokenAddedRSP==IDA)
                return on_TokenAddedRSP((const jwtEvent::TokenAddedRSP*)E->e.get());
            if(jwtEventEnum::AddTokenRSP==IDA)
                return AddTokenRSP((const jwtEvent::AddTokenRSP*)E->e.get());
            if(jwtEventEnum::GetUrSinceRSP==IDA)
                return GetUrSinceRSP((const jwtEvent::GetUrSinceRSP*)E->e.get());


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

jwtNode::Service::~Service()
{
}


jwtNode::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(nm,id),
    Broadcaster(ins)
{
    auto ba=ins->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","http listen address");
    if(ba.size()==0)
        throw CommonError("if(ba.size()==0)");

    bindAddr=*ba.begin();
    jwtBossAddr=ins->getConfig()->get_string("bossAddr","local","boss jwt address");

    ping_timeout=ins->getConfig()->get_int64_t("ping_timeout",3,"ping timeout to boss");

}

void registerjwtServerWebService(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::jwtNode,"jwtNode",getEvents_jwtWebServer());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::jwtNode,jwtNode::Service::construct,"jwtNode");
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

bool jwtNode::Service::on_RequestIncoming(const httpEvent::RequestIncoming*e)
{

    HTTP::Response resp(getIInstance());
//    for(auto &z: e->req->headers)
//    {
//        printf("%s: %s\n",z.first.c_str(),z.second.c_str());
//    }

    if(1){

        std::string query_string=e->req->params["query_string"];

        {
            user_rec ur;
            sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::AddTokenREQ(ur, ListenerBase::serviceId));
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



bool jwtNode::Service::on_TokenAddedRSP(const jwtEvent::TokenAddedRSP*e)
{
    if(e->count==0)
    {
        HTTP::Response resp(getIInstance());
        auto keepAlive=true;
        if(keepAlive)
        {
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";
        }
        resp.content="<div>received response </div>";
//        logErr2("resp:%s",resp.build_html_response().c_str());
//        if(keepAlive)
//            resp.makeResponsePersistent(S->esi);
//        else
//            resp.makeResponse(S->esi);

    }
    else
    {
        user_rec ur;
        sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::AddTokenREQ(ur,ListenerBase::serviceId));

    }

    return true;
}
