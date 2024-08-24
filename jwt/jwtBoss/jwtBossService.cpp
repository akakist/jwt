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

        sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(TI_PING,NULL,NULL,TI_PING_timeout,this));
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
        std::set<msockaddr_in> rm;
        for(auto r=subscribers.begin();r!=subscribers.end(); r++)
        {
            if(now-r->second.last_time_hit > cfg_node_time_out)
            {
                rm.insert(r->first);
            }

        }
        for(auto& r:rm)
        {
            logErr2("erased subscriber %s due timeout",r.dump().c_str());
        }
    }
    return true;
}
std::string buildToken(const std::string &l, const std::string& p)
{
    return l+"."+p;
}
bool jwtBoss::Service::RegisterTokenREQ(const jwtEvent::RegisterTokenREQ* e)
{
    user_rec ur;
    ur.id=idGen++;
    ur.login=e->login;
    ur.expired=time(NULL)+3600*4;
    ur.jwt=buildToken(e->login,e->password);
    logErr2("boss: gen token %s",ur.jdump().toStyledString().c_str());
    notify_task[ur.id].reqNo=e->reqNo;
    notify_task[ur.id].backRoute=e->route;

    for(auto &s: subscribers)
    {
        sendEvent(s.first,ServiceEnum::jwtNode,new jwtEvent::NotifyNewTokenREQ(s.first,ur,ListenerBase::serviceId));
        notify_task[ur.id].addrs.insert(s.first);
    }
    return true;
}
bool jwtBoss::Service::NotifyNewTokenRSP(const jwtEvent::NotifyNewTokenRSP* e)
{
    notify_task[e->t_id].addrs.erase(e->dst);
    if(notify_task[e->t_id].addrs.empty())
    {
        passEvent(new jwtEvent::RegisterTokenRSP("",notify_task[e->t_id].reqNo,e->ur,poppedFrontRoute(notify_task[e->t_id].backRoute)));
        notify_task.erase(e->t_id);

    }
    return true;
}

bool jwtBoss::Service::ConnectFailed(const rpcEvent::ConnectFailed* e)
{
    logErr2("rpc connect failed %s",e->destination_addr.dump().c_str());
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

        if(jwtEventEnum::NotifyNewTokenRSP==ID)
            return NotifyNewTokenRSP((const jwtEvent::NotifyNewTokenRSP*)e.get());

        if(rpcEventEnum::Accepted==ID)
            return Accepted((const rpcEvent::Accepted*)e.get());

        if(rpcEventEnum::ConnectFailed==ID)
            return ConnectFailed((const rpcEvent::ConnectFailed*)e.get());
        if(rpcEventEnum::Connected==ID)
            return Connected((const rpcEvent::Connected*)e.get());


        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            auto E=(rpcEvent::IncomingOnAcceptor*)e.get();
            auto &IDA=E->e->id;
            if(jwtEventEnum::Ping==IDA)
                return Ping((const jwtEvent::Ping*)E->e.get(),E->esi.get());
            if(jwtEventEnum::RegisterTokenREQ==IDA)
                return RegisterTokenREQ((const jwtEvent::RegisterTokenREQ*)E->e.get());
            if(jwtEventEnum::NotifyNewTokenRSP==IDA)
                return NotifyNewTokenRSP((const jwtEvent::NotifyNewTokenRSP*)E->e.get());
        }
        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            auto E=(rpcEvent::IncomingOnConnector*)e.get();
            auto &IDC=E->e->id;
            if(jwtEventEnum::Ping==IDC)
                return Ping((const jwtEvent::Ping*)E->e.get(),E->esi.get());
            if(jwtEventEnum::RegisterTokenREQ==IDC)
                return RegisterTokenREQ((const jwtEvent::RegisterTokenREQ*)E->e.get());
            if(jwtEventEnum::NotifyNewTokenRSP==IDC)
                return NotifyNewTokenRSP((const jwtEvent::NotifyNewTokenRSP*)E->e.get());
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
    cfg_node_time_out=ins->getConfig()->get_real("cfg_node_time_out",5.,"Kill node by ping timeout");
    TI_PING_timeout=ins->getConfig()->get_real("TI_PING_timeout",5.,"Kill node by ping timeout");
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
//    e->esi->remote_name();
    return true;
}
bool jwtBoss::Service::Connected(const rpcEvent::Connected* e)
{
    return true;
}

bool jwtBoss::Service::Ping(const jwtEvent::Ping* e,   epoll_socket_info *esi_remote)
{
    msockaddr_in sa=esi_remote->remote_name();

    sa.setPort(e->port);
    auto it=subscribers.find(sa);
    if(it!=subscribers.end())
    {
        it->second.last_time_hit=time(NULL);
    }
    else
    {
        subscriber s;
        s.route=poppedFrontRoute(e->route);
        s.sa=sa;
        s.last_time_hit=time(NULL);
        sendEvent(s.sa,ServiceEnum::jwtNode,new jwtEvent::NotifyDB(users,ListenerBase::serviceId));
        subscribers.insert({s.sa,std::move(s)});
    }
    return true;
}


