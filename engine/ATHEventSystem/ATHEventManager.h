#ifndef ATHEVENTMANAGER_H
#define ATHEVENTMANAGER_H

#include "ATHEvent.h"

#include <list>
#include <map>


class ATHEventListener;
class ATHEventManager
{
private:

	static ATHEventManager* m_pInstance;
	std::multimap< ATHEventType, ATHEventListener* > m_mmapClients;
	std::list< ATHEvent > m_liEvents;

	void DispatchEvent( ATHEvent* _toDispatch );

	// This is a singleton
	ATHEventManager();
	ATHEventManager(const ATHEventManager&);
	ATHEventManager& operator=(const ATHEventManager&);
	~ATHEventManager();

public:

	static ATHEventManager* GetInstance();
	static void DeleteInstance();

	void RegisterClient( ATHEventType _eventType, ATHEventListener* _pClient );

	void UnregisterClient( ATHEventListener* _pClient );

	void UnregisterClient( ATHEventType _eventType , ATHEventListener* _pClient );

	void SendEvent( ATHEvent _Event, ATHEventPriority _Priority = AEP_NORMAL );

	void ProcessEvents();

	void ClearEvents(void);

	void Shutdown(void);

};

#endif