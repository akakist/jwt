#ifndef _________dfsReferrerEvent_h123Z1
#define _________dfsReferrerEvent_h123Z1
#include "EVENT_id.h"
#include "SERVICE_id.h"
#include "event_mt.h"
#include "genum.hpp"
namespace ServiceEnum
{
    const SERVICE_id jwtServer(genum_jwtServer);
    const SERVICE_id jwtServerWeb(genum_jwtServerWeb);
}

namespace jwtEventEnum
{

    const EVENT_id AddTokenREQ(genum_ptAddTokenREQ);
    const EVENT_id TokenAddedRSP(genum_ptTokenAddedRSP);

}

namespace jwtEvent {




    class AddTokenREQ: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new AddTokenREQ(r);
        }
        AddTokenREQ(const std::string& _session,
                   const std::string& _sampleString,int _count,
                   const route_t&r)
            :Base(jwtEventEnum::AddTokenREQ,r),
             session(_session),
             sampleString(_sampleString),count(_count)
        {}
        AddTokenREQ(const route_t&r)
            :Base(jwtEventEnum::AddTokenREQ,r) {}
        std::string session;
        std::string sampleString;
        int count;
        void unpack(inBuffer& o)
        {
            o>>session>>sampleString>>count;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<sampleString<<count;
        }
        void jdump(Json::Value &) const
        {
        }

    };

    class TokenAddedRSP: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new TokenAddedRSP(r);
        }
        TokenAddedRSP(const std::string& _session,
                   const std::string& _sampleString,int _count,
                   const route_t&r)
            :Base(jwtEventEnum::TokenAddedRSP,r),
             session(_session),
             sampleString(_sampleString),count(_count)
        {}
        TokenAddedRSP(const route_t&r)
            :Base(jwtEventEnum::TokenAddedRSP,r) {}
        std::string session;
        std::string sampleString;
        int count;
        void unpack(inBuffer& o)
        {
            o>>session>>sampleString>>count;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<sampleString<<count;
        }
        void jdump(Json::Value &) const
        {
        }

    };

}




#endif
