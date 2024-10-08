#pragma once
#include "broadcaster.h"



#include "listenerBuffered1Thread.h"
#include <map>
#include "Events/System/Run/startServiceEvent.h"

#include "Events/System/Net/httpEvent.h"
#include "Events/jwtEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/System/Net/rpcEvent.h"
#define SESSION_ID  "session_id"

namespace jwtNode
{

    enum timers{
        T_PING,
    };


    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster
    {
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);



    public:
        Service(const SERVICE_id&, const std::string&  nm, IInstance *ins);
        ~Service();


        bool RequestIncoming(const httpEvent::RequestIncoming*);

        bool TickTimer(const timerEvent::TickTimer *e);

        bool NotifyDB(jwtEvent::NotifyDB *e);

        bool NotifyNewTokenREQ(jwtEvent::NotifyNewTokenREQ *e);
        bool RegisterTokenRSP(jwtEvent::RegisterTokenRSP *e);

        bool Connected(rpcEvent::Connected *e);
        bool Accepted(rpcEvent::Accepted *e);

    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }




        msockaddr_in bindAddr;
        std::string jwtBossAddr;
        time_t ping_timeout;

        std::map<std::string, REF_getter<P_user_rec> > user_2_ur;
        std::map<int64_t, REF_getter<P_user_rec> > id_2_ur;
        std::map<std::string, REF_getter<P_user_rec> > token_2_ur;


        int64_t reg_seqId=0;
        std::map<int64_t, REF_getter<httpEvent::RequestIncoming> > http_sessions;

        IInstance *instance=nullptr;
    };

}

