#ifndef __________jwtServerService_EventInfo__HH
#define __________jwtServerService_EventInfo__HH


#include "IUtils.h"

#include "Events/System/Run/startServiceEvent.h"
#include "Events/System/timerEvent.h"
#include "Events/jwtEvent.h"
inline std::set<EVENT_id> getEvents_jwtServerService()
{

	std::set<EVENT_id> out;
	out.insert(jwtEventEnum::AddTokenREQ);
	out.insert(jwtEventEnum::TokenAddedRSP);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::TickTimer);

	return out;
}

inline void regEvents_jwtServerService()
{
	iUtils->registerEvent(jwtEvent::AddTokenREQ::construct);
	iUtils->registerEvent(jwtEvent::TokenAddedRSP::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
}
#endif
