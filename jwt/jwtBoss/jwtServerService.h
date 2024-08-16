#pragma once
#include "broadcaster.h"



#include "listenerBuffered1Thread.h"
#include <map>


#include "Events/System/Run/startServiceEvent.h"

#include "Events/jwtEvent.h"



namespace jwtServer
{


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


        bool on_AddTokenREQ(const jwtEvent::AddTokenREQ* e);









    };

}

