#pragma once
#include "broadcaster.h"



#include "listenerBuffered1Thread.h"
#include <map>
#include "Events/System/Run/startServiceEvent.h"

#include "Events/System/Net/httpEvent.h"
#include "Events/jwtEvent.h"
#define SESSION_ID  "session_id"
enum TIMERS
{
    TIMER_PUSH_NOOP=1
};
namespace jwtWebServer
{
    class Session:
        public Refcountable
    {
    public:
        std::string sessionId;
        REF_getter<epoll_socket_info> esi;
        REF_getter<HTTP::Request> req;


        Session(const std::string& sid, const REF_getter<HTTP::Request> &_req,const REF_getter<epoll_socket_info> &_esi): sessionId(sid),
            req(_req),
            esi(_esi)
        {
        }
        ~Session() {}

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



    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }

        REF_getter<jwtWebServer::Session> check_session(const REF_getter<HTTP::Request>& req, HTTP::Response& resp, const REF_getter<epoll_socket_info> &_esi);
        REF_getter<jwtWebServer::Session> get_session( const std::string& session_id);

        std::map<std::string,REF_getter<Session> > sessions;


        msockaddr_in bindAddr;
        std::string jwtServerAddr;

        std::set<std::string> tokens;



    };

}

