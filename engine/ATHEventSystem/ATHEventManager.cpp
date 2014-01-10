#include "ATHEventManager.h"
#include "ATHEventListener.h"

ATHEventManager* ATHEventManager::m_pInstance = nullptr;

ATHEventManager::ATHEventManager()
{

}
//================================================================================
ATHEventManager::~ATHEventManager()
{

}
//================================================================================
ATHEventManager* ATHEventManager::GetInstance()
{
	if( !m_pInstance )
	{
		m_pInstance = new ATHEventManager();
	}

	return m_pInstance;
}
//================================================================================
void ATHEventManager::DeleteInstance()
{
	if( m_pInstance )
	{
		delete m_pInstance;
	}

	m_pInstance = nullptr;
}
//================================================================================
void ATHEventManager::RegisterClient( ATHEventType _eventType, ATHEventListener* _pClient )
{
	if( !_pClient )
		return;

	m_mmapClients.insert( std::make_pair( _eventType, _pClient ) );
}
//================================================================================
void ATHEventManager::UnregisterClient( ATHEventType _eventType , ATHEventListener* _pClient )
{
	std::pair< std::multimap< ATHEventType, ATHEventListener* >::iterator, std::multimap< ATHEventType, ATHEventListener* >::iterator > range;

	range = m_mmapClients.equal_range( _eventType );

	for( std::multimap< ATHEventType, ATHEventListener* >::iterator mmIter = range.first; mmIter != range.second; mmIter++ )
	{
		if( (*mmIter).second == _pClient )
		{
			mmIter = m_mmapClients.erase( mmIter );
			break;
		}
	}
}
//================================================================================
void ATHEventManager::SendEvent( ATHEvent _Event, ATHEventPriority _Priority )
{
	switch( _Priority )
	{
	case AEP_NORMAL:
		{
			m_liEvents.push_back( _Event );
			break;
		}
	case AEP_IMMEDIATE:
		{
			DispatchEvent( &_Event );
		}
	}
	
}
//================================================================================
void ATHEventManager::ProcessEvents()
{
	while( m_liEvents.size() )
	{
		DispatchEvent( &m_liEvents.front() );
		m_liEvents.pop_front();
	}
}
//================================================================================
void ATHEventManager::DispatchEvent( ATHEvent* _toDispatch )
{
	std::pair< std::multimap< ATHEventType, ATHEventListener* >::iterator, std::multimap< ATHEventType, ATHEventListener* >::iterator > range;

	range = m_mmapClients.equal_range( _toDispatch->m_EventType );

	for( std::multimap< ATHEventType, ATHEventListener* >::iterator mmIter = range.first; mmIter != range.second; mmIter++ )
	{
		(*mmIter).second->HandleEvent( _toDispatch );
	}
}
//================================================================================
void ATHEventManager::ClearEvents(void)
{
	m_liEvents.clear();
}
//================================================================================
void ATHEventManager::Shutdown(void)
{
	ClearEvents();
	m_mmapClients.clear();
}
//================================================================================