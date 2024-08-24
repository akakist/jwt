#pragma once
#include <stdint.h>
#include <string>
#include "ioBuffer.h"
#include <json/json.h>
struct user_rec
{
    int64_t id;
    std::string login;
    int64_t expired;
    int64_t reg_datetime;
    std::string jwt;
    Json::Value jdump()
    {
        Json::Value j;
        j["id"]=id;
        j["login"]=login;
        j["expired"]=expired;
        j["reg_datetime"]=reg_datetime;
        j["jwt"]=jwt;
        return j;
    }
};
inline outBuffer& operator<< (outBuffer& b,const user_rec& s)
{
    b<<s.id<<s.login<<s.expired<<s.reg_datetime<<s.jwt;
    return b;
}
inline  inBuffer& operator>> (inBuffer& b,  user_rec& s)
{
    b>>s.id>>s.login>>s.expired>>s.reg_datetime>>s.jwt;
    return b;

}

struct P_user_rec: public Refcountable
{
    user_rec ur;
};
