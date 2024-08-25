#include "Events/System/Net/rpcEvent.h"
#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include <map>
#include "jwtClientService.h"
#include "events_jwtClient.hpp"
#include "splitStr.h"
#include "tools_mt.h"
#include "version_mega.h"



bool jwtClient::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    for(auto& z: servers)
    {
        auto sid=iUtils->getNewSocketId();
        sendEvent(ServiceEnum::Socket,new socketEvent::AddToConnectTCP(sid, z,"jwt connect",this));
    }

    return true;
}
std::string charss="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


bool jwtClient::Service::Connected(socketEvent::Connected *e)
{
    std::string user;
    for(int i=0;i<20;i++)
        user+=charss[rand()%charss.size()];

    std::string password;
    for(int i=0;i<20;i++)
        password+=charss[rand()%charss.size()];

    auto req="GET /register?l="+user+"&p="+password+" HTTP/1.1\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
            "Host: www.tutorialspoint.com\r\n"
            "Accept-Language: en-us\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: Keep-Alive\r\n"
             "\r\n";
    e->esi->write_(req);

    auto &kac=keep_alive_conns[e->esi->id_];
    kac.esi=e->esi;
    kac.login=user;
    kac.passwd=password;
    kac.actions.push_back("registration");

    return true;
}
bool jwtClient::Service::ConnectFailed(socketEvent::ConnectFailed *e)
{
    logErr2("connect failed, exiting");
    iUtils->setTerminate(1);
    return true;
}
bool jwtClient::Service::NotifyOutBufferEmpty(socketEvent::NotifyOutBufferEmpty *e)
{
    return true;
}
bool parseHttp(std::string& inb, std::map<std::string,std::string> & params, std::string& body)
{
    auto pz=inb.find("\r\n\r\n");
    if(pz!=std::string::npos)
    {
        auto head=inb.substr(0,pz);
//        logErr2("head %s",head.c_str());
        auto dq=splitStr("\r\n",head);
        if(dq.size())
        {
            auto l1=splitStr(" ",dq[0]);
//            logErr2("l1 %s",dq[0].c_str());
            if(l1.size()==3)
            {
                auto code=l1[1];
                if(code!="200")
                    throw CommonError("invalid response");
            }
            dq.pop_front();
            for(auto &z: dq)
            {
                auto l1=splitStr(": ",z);
                if(l1.size()==2)
                {
                    params[l1[0]]=l1[1];
                }

            }
        }
        auto ct=atoi(params["Content-Length"].c_str());
        if(inb.size()==pz+4+ct)
        {
            body=inb.substr(pz+4,inb.size()-(pz+4));
            inb.clear();
        }
        else if(inb.size()>pz+4+ct)
        {
            body=inb.substr(pz+4,ct);
            inb=inb.substr(pz+4+ct);
        }
        else if(inb.size()<pz+4+ct)
            return false;
        return true;
        if(body.size())
        {
            logErr2("body %s",body.c_str());
        }
    }
    return false;
}
bool jwtClient::Service::StreamRead(socketEvent::StreamRead *e)
{
    std::string head;
    auto& inb=in_bufs[e->esi->id_];
    {
        W_LOCK(e->esi->inBuffer_.lk);
        inb.append(std::move(e->esi->inBuffer_._mx_data));
        e->esi->inBuffer_._mx_data.clear();
    }
    std::map<std::string,std::string> params;
    std::string body;
    if(parseHttp(inb,params,body))
    {
        auto & kac=keep_alive_conns[e->esi->id_];
        if(kac.actions.empty())
            throw CommonError("if(kac.actions.empty())");
        auto action=kac.actions.front();
        kac.actions.pop_front();
        if(action=="registration")
        {
            Json::Value j=jparse(body);
            user_rec ur;
            ur.load(j);
            tokens.push_back(std::move(ur));
        }
        else if(action=="/")
        {

        }
        else
            throw CommonError("unhandled action %s",action.c_str());

    }
    auto token=tokens[rand()%tokens.size()].jwt;
    auto req="GET / HTTP/1.1\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
            "Host: www.tutorialspoint.com\r\n"
            "Accept-Language: en-us\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Auth: "+token+"\r\n"
            "Connection: Keep-Alive\r\n"
            "Keep-Alive: timeout=5, max=1000\r\n"
             "\r\n";
    e->esi->write_(req);

    auto &kac=keep_alive_conns[e->esi->id_];
    kac.actions.push_back("/");

    return true;
}

bool jwtClient::Service::TickTimer(const timerEvent::TickTimer *e)
{
    return true;
}

bool jwtClient::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;

        if(socketEventEnum::NotifyOutBufferEmpty==ID)
            return NotifyOutBufferEmpty(static_cast< socketEvent::NotifyOutBufferEmpty*>(e.get()));
        if(socketEventEnum::Connected==ID)
            return Connected(static_cast< socketEvent::Connected*>(e.get()));
        if(socketEventEnum::ConnectFailed==ID)
            return ConnectFailed(static_cast< socketEvent::ConnectFailed*>(e.get()));

        if(socketEventEnum::StreamRead==ID)
            return StreamRead(static_cast< socketEvent::StreamRead*>(e.get()));

        if(timerEventEnum::TickTimer==ID)
            return TickTimer(static_cast<const timerEvent::TickTimer*>(e.get()));

        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());
        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.get();
            auto& IDC=E->e->id;


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.get();
            auto& IDA=E->e->id;


            return false;
        }


    }
    catch(CommonError& e)
    {
        logErr2("jwtWebServer CommonError  %s",e.what());

    }
    catch(std::exception &e)
    {
        logErr2("jwtWebServer std::exception  %s",e.what());
    }
    XPASS;
    return false;
}

jwtClient::Service::~Service()
{
}


jwtClient::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(nm,id),
    Broadcaster(ins)
{
    auto ba=ins->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","http listen address");
    if(ba.size()==0)
        throw CommonError("if(ba.size()==0)");

    auto _servers=ins->getConfig()->get_tcpaddr("jwtServers","0.0.0.0:8088,0.0.0.0:8089,0.0.0.0:8090","servers listen addresses");
    for(auto& z:_servers)
        servers.push_back(z);


    ping_timeout=ins->getConfig()->get_int64_t("ping_timeout",3,"ping timeout to boss");

}

void register_jwtClient(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::jwtClient,"jwtClient",getEvents_jwtClient());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::jwtClient,jwtClient::Service::construct,"jwtClient");
        regEvents_jwtClient();
    }
    XPASS;
}






