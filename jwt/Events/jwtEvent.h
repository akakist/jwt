#ifndef _________dfsReferrerEvent_h123Z1
#define _________dfsReferrerEvent_h123Z1
#include "EVENT_id.h"
#include "SERVICE_id.h"
#include "event_mt.h"
#include "genum.hpp"
#include "common/jwt_common.h"
namespace ServiceEnum
{
    const SERVICE_id jwtBoss(genum_jwtServer);
    const SERVICE_id jwtNode(genum_jwtServerWeb);
}

namespace jwtEventEnum
{

    const EVENT_id AddTokenREQ(genum_ptAddTokenREQ);
    const EVENT_id AddTokenRSP(genum_ptAddTokenRSP);
    const EVENT_id TokenAddedRSP(genum_ptTokenAddedRSP);

    const EVENT_id Ping(genum_Ping);
    const EVENT_id GetUrSinceREQ(genum_GetUrSinceREQ);
    const EVENT_id GetUrSinceRSP(genum_GetUrSinceRSP);

}

namespace jwtEvent {




    class AddTokenREQ: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new AddTokenREQ(r);
        }
        AddTokenREQ(const user_rec& _ur,
                   const route_t&r)
            :Base(jwtEventEnum::AddTokenREQ,r),
             ur(_ur)
        {}
        AddTokenREQ(const route_t&r)
            :Base(jwtEventEnum::AddTokenREQ,r) {}
        user_rec ur;
        void unpack(inBuffer& o)
        {
            o>>ur;
        }
        void pack(outBuffer&o) const
        {
            o<<ur;
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class AddTokenRSP: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new AddTokenRSP(r);
        }
        AddTokenRSP(int64_t _lastId,
                   const route_t&r)
            :Base(jwtEventEnum::AddTokenRSP,r),
             lastId(_lastId)
        {}
        AddTokenRSP(const route_t&r)
            :Base(jwtEventEnum::AddTokenRSP,r) {}
        int64_t lastId;
        void unpack(inBuffer& o)
        {
            o>>lastId;
        }
        void pack(outBuffer&o) const
        {
            o<<lastId;
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class Ping: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new Ping(r);
        }
        Ping(const route_t&r)
            :Base(jwtEventEnum::Ping,r) {}
        void unpack(inBuffer& o)
        {
        }
        void pack(outBuffer&o) const
        {
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class GetUrSinceREQ: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new GetUrSinceREQ(r);
        }
        GetUrSinceREQ(int64_t _sinceId,
                   const route_t&r)
            :Base(jwtEventEnum::GetUrSinceREQ,r),
             sinceId(_sinceId)
        {}
        GetUrSinceREQ(const route_t&r)
            :Base(jwtEventEnum::GetUrSinceREQ,r) {}
        int64_t sinceId;
        void unpack(inBuffer& o)
        {
            o>>sinceId;
        }
        void pack(outBuffer&o) const
        {
            o<<sinceId;
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class GetUrSinceRSP: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new GetUrSinceRSP(r);
        }
        GetUrSinceRSP(int64_t _lastId, const std::map<int64_t,user_rec>& _c,
                   const route_t&r)
            :Base(jwtEventEnum::GetUrSinceRSP,r),
             lastId(_lastId),
             container(_c)
        {}
        GetUrSinceRSP(const route_t&r)
            :Base(jwtEventEnum::GetUrSinceRSP,r) {}
        int64_t lastId;
        std::map<int64_t,user_rec> container;
        void unpack(inBuffer& o)
        {
            o>>lastId>>container;
        }
        void pack(outBuffer&o) const
        {
            o<<lastId<<container;
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
        TokenAddedRSP(const std::string& _user,
                   const std::string& _sampleString,int _count,
                   const route_t&r)
            :Base(jwtEventEnum::TokenAddedRSP,r),
             user(_user),
             sampleString(_sampleString),count(_count)
        {}
        TokenAddedRSP(const route_t&r)
            :Base(jwtEventEnum::TokenAddedRSP,r) {}
        std::string user;
        std::string sampleString;
        int count;
        void unpack(inBuffer& o)
        {
            o>>user>>sampleString>>count;
        }
        void pack(outBuffer&o) const
        {
            o<<user<<sampleString<<count;
        }
        void jdump(Json::Value &) const
        {
        }

    };

}




#endif
