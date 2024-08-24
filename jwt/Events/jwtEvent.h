#pragma once
#include "EVENT_id.h"
#include "SERVICE_id.h"
#include "event_mt.h"
#include "genum.hpp"
#include "common/jwt_common.h"
#include "msockaddr_in.h"
namespace ServiceEnum
{
    const SERVICE_id jwtBoss(genum_jwtBoss);
    const SERVICE_id jwtNode(genum_jwtNode);
    const SERVICE_id jwtClient(genum_jwtClient);

}

namespace jwtEventEnum
{

    const EVENT_id AddTokenREQ(genum_ptAddTokenREQ);
    const EVENT_id AddTokenRSP(genum_ptAddTokenRSP);
    const EVENT_id TokenAddedRSP(genum_ptTokenAddedRSP);

    const EVENT_id Ping(genum_Ping);
    const EVENT_id GetUrSinceREQ(genum_GetUrSinceREQ);
    const EVENT_id GetUrSinceRSP(genum_GetUrSinceRSP);

    const EVENT_id NotifyNewTokenREQ(genum_NotifyNewTokenREQ);
    const EVENT_id NotifyNewTokenRSP(genum_NotifyNewTokenRSP);

    const EVENT_id NotifyDB(genum_NotifyDB);

    const EVENT_id RegisterTokenREQ(genum_RegisterTokenREQ);
    const EVENT_id RegisterTokenRSP(genum_RegisterTokenRSP);

}

namespace jwtEvent {

    class NotifyNewTokenREQ: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new NotifyNewTokenREQ(r);
        }
        NotifyNewTokenREQ(const msockaddr_in& _dst,const user_rec &_ur,
                   const route_t&r)
            :Base(jwtEventEnum::NotifyNewTokenREQ,r),
              dst(_dst),
             ur(_ur)
        {}
        NotifyNewTokenREQ(const route_t&r)
            :Base(jwtEventEnum::NotifyNewTokenREQ,r) {}
        msockaddr_in dst;
        user_rec ur;
        void unpack(inBuffer& o)
        {
            o>>dst>>ur;
        }
        void pack(outBuffer&o) const
        {
            o<<dst<<ur;
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class NotifyNewTokenRSP: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new NotifyNewTokenRSP(r);
        }
        NotifyNewTokenRSP(const msockaddr_in& _dst,int64_t _t_id,const user_rec& _ur,
                   const route_t&r)
            :Base(jwtEventEnum::NotifyNewTokenRSP,r),
              dst(_dst),
             t_id(_t_id),ur(_ur)
        {}
        NotifyNewTokenRSP(const route_t&r)
            :Base(jwtEventEnum::NotifyNewTokenRSP,r) {}
        msockaddr_in dst;
        int64_t t_id;
        user_rec ur;
        void unpack(inBuffer& o)
        {
            o>>dst>>t_id>>ur;
        }
        void pack(outBuffer&o) const
        {
            o<<dst<<t_id<<ur;
        }
        void jdump(Json::Value &) const
        {
        }

    };


    class RegisterTokenREQ: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new RegisterTokenREQ(r);
        }
        RegisterTokenREQ(const std::string& l, const std::string& p, int64_t _reqNo,
                   const route_t&r)
            :Base(jwtEventEnum::RegisterTokenREQ,r),
             login(l),password(p),reqNo(_reqNo)
        {}
        RegisterTokenREQ(const route_t&r)
            :Base(jwtEventEnum::RegisterTokenREQ,r) {}
        std::string login;
        std::string password;
        int64_t reqNo;
        void unpack(inBuffer& o)
        {
            o>>login>>password>>reqNo;
        }
        void pack(outBuffer&o) const
        {
            o<<login<<password<<reqNo;
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class RegisterTokenRSP: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new RegisterTokenRSP(r);
        }
        RegisterTokenRSP(const std::string& _err, int64_t _reqNo, const user_rec &_ur,
                   const route_t&r)
            :Base(jwtEventEnum::RegisterTokenRSP,r),
             error(_err),reqNo(_reqNo),ur(_ur)
        {}
        RegisterTokenRSP(const route_t&r)
            :Base(jwtEventEnum::RegisterTokenRSP,r) {}
        std::string error;
        int64_t reqNo;
        user_rec ur;
        void unpack(inBuffer& o)
        {
            o>>error>>reqNo>>ur;
        }
        void pack(outBuffer&o) const
        {
            o<<error<<reqNo<<ur;
        }
        void jdump(Json::Value &) const
        {
        }

    };

    class NotifyDB: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new NotifyDB(r);
        }
        NotifyDB(const std::map<int64_t,user_rec>& _ur,
                   const route_t&r)
            :Base(jwtEventEnum::NotifyDB,r),
             ur(_ur)
        {}
        NotifyDB(const route_t&r)
            :Base(jwtEventEnum::NotifyDB,r) {}
        std::map<int64_t,user_rec> ur;
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

        Ping(unsigned short _port, const route_t&r)
            :Base(jwtEventEnum::Ping,r),port(_port) {}

        unsigned short port;
        void unpack(inBuffer& o)
        {
            o>>port;
        }
        void pack(outBuffer&o) const
        {
            o<<port;
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




