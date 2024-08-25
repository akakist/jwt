#include "Events/System/Net/rpcEvent.h"
#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include <map>
#include "jwtNodeService.h"
#include "events_jwtNode.hpp"
#include "version_mega.h"
#include "tools_mt.h"


bool jwtNode::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    sendEvent(ServiceEnum::HTTP,new httpEvent::DoListen(bindAddr,this));
    sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(T_PING,NULL,NULL,ping_timeout,this));

    sendEvent(ServiceEnum::RPC,new rpcEvent::SubscribeNotifications(this));
    auto port=getRpcExternalListenPortMain(instance);
    sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::Ping(port,ListenerBase::serviceId));


    return true;
}

bool jwtNode::Service::TickTimer(const timerEvent::TickTimer *e)
{
    auto port=getRpcExternalListenPortMain(instance);
    sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::Ping(port,ListenerBase::serviceId));
    return true;
}
bool jwtNode::Service::NotifyDB( jwtEvent::NotifyDB* e)
{
    for(auto& z:e->ur)
    {
        REF_getter<P_user_rec> p=new P_user_rec;
        p->ur=std::move(z.second);
        int64_t id=z.first;
        user_2_ur.insert({p->ur.login,p});
        id_2_ur.insert({z.first,p});
        token_2_ur.insert({p->ur.jwt,p});
    }
    return true;
}

bool jwtNode::Service::Connected(rpcEvent::Connected *e)
{
    return true;
}
bool jwtNode::Service::Accepted(rpcEvent::Accepted *e)
{
    return true;
}
bool jwtNode::Service::NotifyNewTokenREQ(jwtEvent::NotifyNewTokenREQ *e)
{
    REF_getter<P_user_rec> p=new P_user_rec;
    p->ur=e->ur;
    user_2_ur.insert({p->ur.login,p});
    id_2_ur.insert({p->ur.id,p});
    token_2_ur.insert({p->ur.jwt,p});
//    logErr2("node recv jwt %s",p->ur.jdump().toStyledString().c_str());
    passEvent(new jwtEvent::NotifyNewTokenRSP(e->dst,e->ur.id,e->ur,poppedFrontRoute(e->route)));
    return true;
}
bool jwtNode::Service::RegisterTokenRSP(jwtEvent::RegisterTokenRSP *e)
{
    if(e->error=="")
    {
        auto it=http_sessions.find(e->reqNo);
        if(it!=http_sessions.end())
        {
            HTTP::Response resp(getIInstance());
            Json::Value j;
            j["status"]="OK";
            j["login"]=e->ur.login;
            j["expired"]=e->ur.expired;
            j["id"]=e->ur.id;
            j["jwt"]=e->ur.jwt;
            j["reg_datetime"]=e->ur.reg_datetime;
            resp.content=j.toStyledString();
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";

            std::string out = resp.build_html_response();
            it->second->esi->write_(out);

        }
        else throw CommonError("!if(it!=http_sessions.end())");
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
            return RequestIncoming((const httpEvent::RequestIncoming*)e.get());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());


        if(jwtEventEnum::RegisterTokenRSP==ID)
            return RegisterTokenRSP(( jwtEvent::RegisterTokenRSP*)e.get());




        if(jwtEventEnum::NotifyDB==ID)
            return NotifyDB((jwtEvent::NotifyDB*)e.get());
        if(rpcEventEnum::Connected==ID)
            return Connected((rpcEvent::Connected*)e.get());
        if(rpcEventEnum::Accepted==ID)
            return Accepted((rpcEvent::Accepted*)e.get());

        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.get();
            auto& IDC=E->e->id;
            if(jwtEventEnum::NotifyDB==IDC)
                return NotifyDB(( jwtEvent::NotifyDB*)E->e.get());
            if(jwtEventEnum::NotifyNewTokenREQ==IDC)
                return NotifyNewTokenREQ(( jwtEvent::NotifyNewTokenREQ*)E->e.get());
            if(jwtEventEnum::RegisterTokenRSP==IDC)
                return RegisterTokenRSP(( jwtEvent::RegisterTokenRSP*)E->e.get());


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.get();
            auto& IDA=E->e->id;
            if(jwtEventEnum::NotifyDB==IDA)
                return NotifyDB(( jwtEvent::NotifyDB*)E->e.get());

            if(jwtEventEnum::NotifyNewTokenREQ==IDA)
                return NotifyNewTokenREQ(( jwtEvent::NotifyNewTokenREQ*)E->e.get());
            if(jwtEventEnum::RegisterTokenRSP==IDA)
                return RegisterTokenRSP(( jwtEvent::RegisterTokenRSP*)E->e.get());


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
    Broadcaster(ins), instance(ins)
{
    auto ba=ins->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","http listen address");
    if(ba.size()==0)
        throw CommonError("if(ba.size()==0)");

    bindAddr=*ba.begin();
    jwtBossAddr=ins->getConfig()->get_string("bossAddr","local","boss jwt address");

    ping_timeout=ins->getConfig()->get_int64_t("ping_timeout",3,"ping timeout to boss");

}

void register_jwtNode(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::jwtNode,"jwtNode",getEvents_jwtNode());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::jwtNode,jwtNode::Service::construct,"jwtNode");
        regEvents_jwtNode();
    }
    XPASS;
}







int cnt=0;
bool jwtNode::Service::RequestIncoming(const httpEvent::RequestIncoming*e)
{
    cnt++;
    if(cnt%1000==0)
        logErr2("RequestIncoming %d",cnt);
//    logErr2("@@ %s",e->dump().toStyledString().c_str());
    HTTP::Response resp(getIInstance());

    if(e->req->url=="/register")
    {
        auto &l=e->req->params["l"];
        auto &p=e->req->params["p"];
        auto session=reg_seqId++;
        http_sessions.insert({session,e});
        sendEvent(jwtBossAddr,ServiceEnum::jwtBoss,new jwtEvent::RegisterTokenREQ(l,p,session,ListenerBase::serviceId));
        return true;

    } else if(e->req->url=="/")
    {
        HTTP::Response resp(getIInstance());
        auto token=e->req->headers["Auth"];
//        logErr2("token %s",token.c_str());
        auto it=token_2_ur.find(token);
        if(it==token_2_ur.end())
        {

            resp.http_code=401;
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";
            std::string out = resp.build_html_response();
            e->esi->write_(out);

        }
        else {
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";
            std::string out = resp.build_html_response();
            e->esi->write_(out);

        }
    }

#ifdef KALL
    if(1){

        std::string query_string=e->req->params["query_string"];

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
#endif
    return true;
}



