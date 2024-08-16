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



bool jwtServer::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;
    try {

    }  catch (CommonError& e) {

        logErr2("catch %s",e.what());
    }

    return true;
}


bool jwtServer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
        {
            const timerEvent::TickTimer*ev=static_cast<const timerEvent::TickTimer*>(e.get());
            return true;
        }
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());

        if(jwtEventEnum::AddTokenREQ==ID)
            return on_AddTokenREQ((const jwtEvent::AddTokenREQ*)e.get());


        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            auto E=(rpcEvent::IncomingOnAcceptor*)e.get();
            auto &IDA=E->e->id;
            if(jwtEventEnum::AddTokenREQ==IDA)
                return on_AddTokenREQ((const jwtEvent::AddTokenREQ*)E->e.get());
        }

    } catch(std::exception &e)
    {
        logErr2("BlockchainTop std::exception  %s",e.what());
    }
    XPASS;
    return false;
}



jwtServer::Service::~Service()
{
}


jwtServer::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
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
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::jwtServer,"jwtServer",
                                    getEvents_jwtServerService());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::jwtServer,jwtServer::Service::construct,"jwtServer");
        regEvents_jwtServerService();
    }
    XPASS;
}

int cnt=0;


bool jwtServer::Service::on_AddTokenREQ(const jwtEvent::AddTokenREQ* e)
{
    cnt++;
    if(cnt%1000==0)
    {
        printf("cnt %d\n",cnt);
    }
    std::string xored=e->sampleString+"123";


    passEvent(new jwtEvent::TokenAddedRSP(e->session,xored,e->count,poppedFrontRoute(e->route)));
    return true;
}

