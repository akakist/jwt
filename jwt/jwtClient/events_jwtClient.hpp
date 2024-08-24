#ifndef __________jwtClient_EventInfo__HH
#define __________jwtClient_EventInfo__HH


#include "IUtils.h"

#include "Events/System/Net/httpEvent.h"
#include "Events/System/Net/rpcEvent.h"
#include "Events/System/Net/socketEvent.h"
#include "Events/System/Run/startServiceEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/jwtEvent.h"
inline std::set<EVENT_id> getEvents_jwtClient()
{

	std::set<EVENT_id> out;
	out.insert(httpEventEnum::RequestIncoming);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(socketEventEnum::AddToConnectTCP);
	out.insert(socketEventEnum::Connected);
	out.insert(socketEventEnum::StreamRead);
	out.insert(socketEventEnum::Write);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::TickTimer);

	return out;
}

inline void regEvents_jwtClient()
{
	iUtils->registerEvent(httpEvent::RequestIncoming::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(socketEvent::AddToConnectTCP::construct);
	iUtils->registerEvent(socketEvent::Connected::construct);
	iUtils->registerEvent(socketEvent::StreamRead::construct);
	iUtils->registerEvent(socketEvent::Write::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
}
#endif
