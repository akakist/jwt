#include "Events/System/timerEvent.h"
#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>

#include "tools_mt.h"

#include <map>
#include "jwtBossService.h"
#include "Events/jwtEvent.h"
#include "events_jwtBossService.hpp"
#include "version_mega.h"



bool jwtBoss::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;
    try {

        sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(TI_PING,NULL,NULL,cfg_timer_timeout,this));
        sendEvent(ServiceEnum::RPC,new rpcEvent::SubscribeNotifications(this));

    }  catch (CommonError& e) {

        logErr2("catch %s",e.what());
    }

    return true;
}

bool jwtBoss::Service::TickTimer(const timerEvent::TickTimer* e)
{
    if(e->tid==TI_PING)
    {
        time_t now=time(NULL);
        for(auto r=subscribers.begin();r!=subscribers.end(); r++)
        {
            if(now-r->second.last_time_hit > cfg_timer_timeout)
            {
                r=subscribers.erase(r);
                logErr2("erased subscriber due timeout");
            }

        }
    }
    return true;
}
std::string buildToken(const std::string &l, const std::string& p)
{
    return "";
}
bool jwtBoss::Service::RegisterTokenREQ(const jwtEvent::RegisterTokenREQ* e)
{
    user_rec ur;
    ur.login=e->login;
    ur.expired=time(NULL)+3600*4;
    ur.jwt=buildToken(e->login,e->password);

    for(auto &s: subscribers)
    {
//        jwtEvent::
    }
    return true;
}


bool jwtBoss::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
            return TickTimer(static_cast<const timerEvent::TickTimer*>(e.get()));

        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());

        if(jwtEventEnum::AddTokenREQ==ID)
            return AddTokenREQ((const jwtEvent::AddTokenREQ*)e.get());

//        if(jwtEventEnum::Ping==ID)
//            return Ping((const jwtEvent::Ping*)e.get());


        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            auto E=(rpcEvent::IncomingOnAcceptor*)e.get();
            auto &IDA=E->e->id;
            if(jwtEventEnum::AddTokenREQ==IDA)
                return AddTokenREQ((const jwtEvent::AddTokenREQ*)E->e.get());
            if(jwtEventEnum::Ping==IDA)
                return Ping((const jwtEvent::Ping*)E->e.get(),E->esi.get());
        }
        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            auto E=(rpcEvent::IncomingOnConnector*)e.get();
            auto &IDC=E->e->id;
            if(jwtEventEnum::AddTokenREQ==IDC)
                return AddTokenREQ((const jwtEvent::AddTokenREQ*)E->e.get());
            if(jwtEventEnum::Ping==IDC)
                return Ping((const jwtEvent::Ping*)E->e.get(),E->esi.get());
        }

    } catch(std::exception &e)
    {
        logErr2("BlockchainTop std::exception  %s",e.what());
    }
    XPASS;
    return false;
}



jwtBoss::Service::~Service()
{
}


jwtBoss::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(nm,id),
    Broadcaster(ins)
{
}

void register_jwtBoss(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::jwtBoss,"jwtBoss",
                                    getEvents_jwtBossService());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::jwtBoss,jwtBoss::Service::construct,"jwtBoss");
        regEvents_jwtBossService();
    }
    XPASS;
}

int64_t jwtBoss::Service::lastId()
{
    int64_t lastNo=0;
    auto it=users.rbegin();
    if(it!=users.rend())
    {
        lastNo=it->first;
    }
    return lastNo;
}
bool jwtBoss::Service::Accepted(const rpcEvent::Accepted* e)
{
    e->esi->remote_name();
    return true;
}

bool jwtBoss::Service::Ping(const jwtEvent::Ping* e,   epoll_socket_info *esi_remote)
{

    auto sa=esi_remote->remote_name();
    auto it=subscribers.find(sa);
    if(it!=subscribers.end())
    {
        it->second.last_time_hit=time(NULL);
    }
    else
    {
        subscriber s;
        s.route=poppedFrontRoute(e->route);
        s.sa=esi_remote->remote_name();
        s.last_time_hit=time(NULL);
        sendEvent(s.sa,ServiceEnum::jwtNode,new jwtEvent::NotifyDB(users,ListenerBase::serviceId));
        subscribers.insert({s.sa,std::move(s)});
    }
    return true;
}

bool jwtBoss::Service::AddTokenREQ(const jwtEvent::AddTokenREQ* e)
{
    int64_t newid=0;
    auto rb=users.rbegin();
    if(rb!=users.rend())
    {
        newid=rb->first+1;
    }
    users.insert({newid,std::move(e->ur)});

    auto lastid=lastId();
    for(auto& r:subscribers)
    {
        passEvent(new jwtEvent::AddTokenRSP(lastid,r.second.route));
    }
    return true;
}

