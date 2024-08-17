#ifndef __________jwtBossService_EventInfo__HH
#define __________jwtBossService_EventInfo__HH


#include "IUtils.h"

#include "Events/System/Run/startServiceEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/jwtEvent.h"
inline std::set<EVENT_id> getEvents_jwtBossService()
{

	std::set<EVENT_id> out;
	out.insert(jwtEventEnum::AddTokenREQ);
	out.insert(jwtEventEnum::AddTokenRSP);
	out.insert(jwtEventEnum::Ping);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::SetTimer);
	out.insert(timerEventEnum::TickTimer);

	return out;
}

inline void regEvents_jwtBossService()
{
	iUtils->registerEvent(jwtEvent::AddTokenREQ::construct);
	iUtils->registerEvent(jwtEvent::AddTokenRSP::construct);
	iUtils->registerEvent(jwtEvent::Ping::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::SetTimer::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
}
#endif
