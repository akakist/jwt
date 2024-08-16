#ifndef __________jwtWebServer_EventInfo__HH
#define __________jwtWebServer_EventInfo__HH


#include "IUtils.h"

#include "Events/System/Net/httpEvent.h"
#include "Events/System/Net/rpcEvent.h"
#include "Events/System/Run/startServiceEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/jwtEvent.h"
inline std::set<EVENT_id> getEvents_jwtWebServer()
{

	std::set<EVENT_id> out;
	out.insert(httpEventEnum::DoListen);
	out.insert(httpEventEnum::RequestIncoming);

	out.insert(jwtEventEnum::AddTokenREQ);
	out.insert(jwtEventEnum::TokenAddedRSP);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::TickTimer);

	return out;
}

inline void regEvents_jwtWebServer()
{
	iUtils->registerEvent(httpEvent::DoListen::construct);
	iUtils->registerEvent(httpEvent::RequestIncoming::construct);
	iUtils->registerEvent(jwtEvent::AddTokenREQ::construct);
	iUtils->registerEvent(jwtEvent::TokenAddedRSP::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
}
#endif
