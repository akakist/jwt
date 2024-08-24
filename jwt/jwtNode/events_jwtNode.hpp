#ifndef __________jwtNode_EventInfo__HH
#define __________jwtNode_EventInfo__HH


#include "IUtils.h"

#include "Events/System/Net/httpEvent.h"
#include "Events/System/Net/rpcEvent.h"
#include "Events/System/Run/startServiceEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/jwtEvent.h"
inline std::set<EVENT_id> getEvents_jwtNode()
{

	std::set<EVENT_id> out;
	out.insert(httpEventEnum::DoListen);
	out.insert(httpEventEnum::RequestIncoming);
	out.insert(jwtEventEnum::AddTokenREQ);
	out.insert(jwtEventEnum::AddTokenRSP);
	out.insert(jwtEventEnum::GetUrSinceREQ);
	out.insert(jwtEventEnum::GetUrSinceRSP);
	out.insert(jwtEventEnum::NotifyDB);
	out.insert(jwtEventEnum::NotifyNewTokenREQ);
	out.insert(jwtEventEnum::NotifyNewTokenRSP);
	out.insert(jwtEventEnum::Ping);
	out.insert(jwtEventEnum::RegisterTokenREQ);
	out.insert(jwtEventEnum::TokenAddedRSP);
	out.insert(rpcEventEnum::Accepted);
	out.insert(rpcEventEnum::Connected);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(rpcEventEnum::SubscribeNotifications);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::SetTimer);
	out.insert(timerEventEnum::TickTimer);

	return out;
}

inline void regEvents_jwtNode()
{
	iUtils->registerEvent(httpEvent::DoListen::construct);
	iUtils->registerEvent(httpEvent::RequestIncoming::construct);
	iUtils->registerEvent(jwtEvent::AddTokenREQ::construct);
	iUtils->registerEvent(jwtEvent::AddTokenRSP::construct);
	iUtils->registerEvent(jwtEvent::GetUrSinceREQ::construct);
	iUtils->registerEvent(jwtEvent::GetUrSinceRSP::construct);
	iUtils->registerEvent(jwtEvent::NotifyDB::construct);
	iUtils->registerEvent(jwtEvent::NotifyNewTokenREQ::construct);
	iUtils->registerEvent(jwtEvent::NotifyNewTokenRSP::construct);
	iUtils->registerEvent(jwtEvent::Ping::construct);
	iUtils->registerEvent(jwtEvent::RegisterTokenREQ::construct);
	iUtils->registerEvent(jwtEvent::TokenAddedRSP::construct);
	iUtils->registerEvent(rpcEvent::Accepted::construct);
	iUtils->registerEvent(rpcEvent::Connected::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(rpcEvent::SubscribeNotifications::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::SetTimer::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
}
#endif
