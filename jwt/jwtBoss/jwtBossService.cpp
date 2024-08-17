#include "Events/System/timerEvent.h"
#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>

#include "tools_mt.h"

#include <map>
#include "jwtServerService.h"
#include "Events/jwtEvent.h"
#include "events_jwtServerService.hpp"
#include "version_mega.h"



bool jwtBoss::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;
    try {

        sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(1,NULL,NULL,cfg_timer_timeout,this));

    }  catch (CommonError& e) {

        logErr2("catch %s",e.what());
    }

    return true;
}

bool jwtBoss::Service::TickTimer(const timerEvent::TickTimer* e)
{
    time_t now=time(NULL);
    for(auto r=subscribers.begin();r!=subscribers.end(); r++)
    {
        if(now-r->second>cfg_timer_timeout)
        {
            r=subscribers.erase(r);
            logErr2("erased subscriber due timeout");
        }

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

        if(jwtEventEnum::Ping==ID)
            return Ping((const jwtEvent::Ping*)e.get());


        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            auto E=(rpcEvent::IncomingOnAcceptor*)e.get();
            auto &IDA=E->e->id;
            if(jwtEventEnum::AddTokenREQ==IDA)
                return AddTokenREQ((const jwtEvent::AddTokenREQ*)E->e.get());
            if(jwtEventEnum::Ping==IDA)
                return Ping((const jwtEvent::Ping*)E->e.get());
        }
        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            auto E=(rpcEvent::IncomingOnConnector*)e.get();
            auto &IDC=E->e->id;
            if(jwtEventEnum::AddTokenREQ==IDC)
                return AddTokenREQ((const jwtEvent::AddTokenREQ*)E->e.get());
            if(jwtEventEnum::Ping==IDC)
                return Ping((const jwtEvent::Ping*)E->e.get());
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

void registerjwtServerService(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::jwtBoss,"jwtBoss",
                                    getEvents_jwtServerService());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::jwtBoss,jwtBoss::Service::construct,"jwtBoss");
        regEvents_jwtServerService();
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

bool jwtBoss::Service::Ping(const jwtEvent::Ping* e)
{
    auto r=poppedFrontRoute(e->route);
    subscribers.insert({r,time(NULL)});
    passEvent(new jwtEvent::AddTokenRSP(lastId(),r));
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
        passEvent(new jwtEvent::AddTokenRSP(lastid,r.first));
    }
    return true;
}

