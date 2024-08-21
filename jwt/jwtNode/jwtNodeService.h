#pragma once
#include "broadcaster.h"



#include "listenerBuffered1Thread.h"
#include <map>
#include "Events/System/Run/startServiceEvent.h"

#include "Events/System/Net/httpEvent.h"
#include "Events/jwtEvent.h"
#include "Events/System/timerEvent.h"
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


        bool on_RequestIncoming(const httpEvent::RequestIncoming*);
        bool on_TokenAddedRSP(const jwtEvent::TokenAddedRSP*e);

        bool TickTimer(const timerEvent::TickTimer *e);

        bool AddTokenRSP(const jwtEvent::AddTokenRSP* e);
        bool GetUrSinceRSP(const jwtEvent::GetUrSinceRSP* e);
        bool NotifyDB(jwtEvent::NotifyDB *e);

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


        int64_t reg_seqId=0;
        std::map<int64_t, REF_getter<httpEvent::RequestIncoming> > http_sessions;


    };

}

