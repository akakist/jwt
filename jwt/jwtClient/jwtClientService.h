#pragma once
#include "broadcaster.h"



#include "listenerBuffered1Thread.h"
#include <map>
#include "Events/System/Run/startServiceEvent.h"

#include "Events/System/Net/httpEvent.h"
#include "Events/jwtEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/System/Net/socketEvent.h"
#define SESSION_ID  "session_id"

namespace jwtClient
{

    enum timers{
//        T_PING,
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



        bool TickTimer(const timerEvent::TickTimer *e);


        bool Connected(socketEvent::Connected *e);
        bool StreamRead(socketEvent::StreamRead *e);
        bool ConnectFailed(socketEvent::ConnectFailed *e);


    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }




        time_t ping_timeout;

        std::map<std::string, REF_getter<P_user_rec> > user_2_ur;
        std::map<int64_t, REF_getter<P_user_rec> > id_2_ur;


        int64_t reg_seqId=0;
        std::map<int64_t, REF_getter<httpEvent::RequestIncoming> > http_sessions;

        std::vector<msockaddr_in> servers;
        struct _connInfo
        {
            REF_getter<epoll_socket_info> esi=nullptr;
            std::string login;
            std::string passwd;


        };
        std::map<SOCKET_id, _connInfo > keep_alive_conns;




    };

}

