#include "ATHObjectManager.h"
#include "../ATHUtil/NewInclude.h"
#include "../Box2D/Box2D.h"

const unsigned int	NUM_VELOCITY_ITERATIONS = 8;
const unsigned int	NUM_POSITION_ITERATIONS = 3;
const float			TIMESTEP_LENGTH = (1.0f/20.0f);
const float			MAX_TIMEBUFFER = 1.0f;

ATHObjectManager::ATHObjectManager() :	m_fTimeBuffer( 0.0f ),
										m_pWorld( nullptr )
{
}
//================================================================================
void ATHObjectManager::Init()
{
	m_pWorld = new b2World( b2Vec2( 0.0f, 0.0f ) );
}
//================================================================================
void ATHObjectManager::Update( float _fDT )
{
	m_fTimeBuffer += _fDT;
	
	// Cap the time to avoide a complete explosive meltodwn
	if( m_fTimeBuffer > MAX_TIMEBUFFER )
		m_fTimeBuffer = MAX_TIMEBUFFER;
	
	while( m_fTimeBuffer > TIMESTEP_LENGTH )
	{
		m_pWorld->Step( _fDT, NUM_VELOCITY_ITERATIONS, NUM_POSITION_ITERATIONS );
		m_fTimeBuffer -= TIMESTEP_LENGTH;
	}
}
//================================================================================
void ATHObjectManager::Shutdown()
{
	delete m_pWorld;
}