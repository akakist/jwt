#pragma once
#include <stdint.h>
#include <string>
#include "ioBuffer.h"
struct user_rec
{
    std::string login;
    int64_t expired;
    int64_t reg_datetime;
    std::string jwt;
};
inline outBuffer& operator<< (outBuffer& b,const user_rec& s)
{
    b<<s.login<<s.expired<<s.reg_datetime<<s.jwt;
    return b;
}
inline  inBuffer& operator>> (inBuffer& b,  user_rec& s)
{
    b>>s.login>>s.expired>>s.reg_datetime>>s.jwt;
    return b;

}

struct P_user_rec: public Refcountable
{
    user_rec ur;
};
