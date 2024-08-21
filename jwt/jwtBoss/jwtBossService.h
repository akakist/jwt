#pragma once
#include "broadcaster.h"
#include "common/jwt_common.h"


#include "epoll_socket_info.h"
#include "listenerBuffered1Thread.h"
#include <map>


#include "Events/System/Run/startServiceEvent.h"

#include "Events/jwtEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/System/Net/rpcEvent.h"



namespace jwtBoss
{
    enum Timers
    {
        TI_PING,
    };
    struct subscriber
    {
        route_t route;
        msockaddr_in sa;
        time_t last_time_hit=0;
    };

    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster
    {
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);




        Service(const SERVICE_id&, const std::string&  nm, IInstance *ins);
        ~Service();



    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }

        bool Accepted(const rpcEvent::Accepted* e);
        bool AddTokenREQ(const jwtEvent::AddTokenREQ* e);
        bool RegisterTokenREQ(const jwtEvent::RegisterTokenREQ* e);
        bool Ping(const jwtEvent::Ping* e, epoll_socket_info *esi_remote);
        bool TickTimer(const timerEvent::TickTimer* e);
        int64_t lastId();

        std::map<msockaddr_in, subscriber > subscribers;

        std::map<int64_t, user_rec> users;


        time_t cfg_node_time_out;
        time_t cfg_timer_timeout;

    };

}

