#include "ATHObjectManager.h"
#include "../ATHRenderer/ATHRenderer.h"
#include "../ATHUtil/NewInclude.h"
#include "../Box2D/Box2D.h"
#include "ATHObject.h"

const unsigned int	NUM_VELOCITY_ITERATIONS = 8;
const unsigned int	NUM_POSITION_ITERATIONS = 5;
const float			TIMESTEP_LENGTH = (1.0f/20.0f);
const float			MAX_TIMEBUFFER = 1.0f;

ATHObjectManager::ATHObjectManager() :	m_fTimeBuffer( 0.0f ),
										m_pWorld( nullptr )
{
}
//================================================================================
void ATHObjectManager::Init()
{
	// Box2D Init
	m_pWorld = new b2World( b2Vec2( 0.0f, -3.0f ) );

	ATHBox2DRenderer* pDebugRenderer = ATHRenderer::GetInstance()->GetDebugRenderer();
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
	flags += b2Draw::e_centerOfMassBit;
	pDebugRenderer->SetFlags(flags);
	m_pWorld->SetDebugDraw( pDebugRenderer );

	b2Vec2 vertices[3];
	vertices[0].Set(0.0f, 0.0f);
	vertices[1].Set(1.0f, 0.0f);
	vertices[2].Set(0.0f, 1.0f);

	int32 count = 3;
	b2PolygonShape polygon;
	b2BodyDef bodyDef;

	bodyDef.type = b2_staticBody;
	bodyDef.position = b2Vec2( 0.0f, 0.0f );
	polygon.SetAsBox( 10.0f, 1.0f, b2Vec2( 0.0f, 0.0f ), 0.0f );
	m_pWorld->CreateBody( &bodyDef )->CreateFixture( &polygon, 1.0f );
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
		m_pWorld->Step( TIMESTEP_LENGTH, NUM_VELOCITY_ITERATIONS, NUM_POSITION_ITERATIONS );
		m_fTimeBuffer -= TIMESTEP_LENGTH;
	}

	m_pWorld->DrawDebugData();

	std::list<ATHObject*>::iterator itrObjects = m_liObjects.begin();
	std::list<ATHObject*>::iterator itrObjectsEnd = m_liObjects.end();
	while( itrObjects != itrObjectsEnd )
	{
		ATHObject* pCurrObj = (*itrObjects);
		if( pCurrObj->GetAlive() )
		{
			if( pCurrObj->GetActive() )
				pCurrObj->Update( _fDT );
		}
		else
			m_liToRemove.push_back( pCurrObj );

		++itrObjects;
	}

	itrObjects = m_liToRemove.begin();
	itrObjectsEnd = m_liToRemove.end();
	while( itrObjects != itrObjectsEnd )
	{
		m_liObjects.remove( (*itrObjects) );
		delete (*itrObjects);
		itrObjects = m_liToRemove.erase( itrObjects );
	}
}
//================================================================================
void ATHObjectManager::Shutdown()
{
	std::list<ATHObject*>::iterator itrObjects = m_liObjects.begin();
	std::list<ATHObject*>::iterator itrObjectsEnd = m_liObjects.end();
	while( itrObjects != itrObjectsEnd )
	{
		delete (*itrObjects);
		itrObjects = m_liObjects.erase( itrObjects );
	}

	itrObjects = m_liStaticObjects.begin();
	itrObjectsEnd = m_liStaticObjects.end();
	while( itrObjects != itrObjectsEnd )
	{
		delete (*itrObjects);
		itrObjects = m_liObjects.erase( itrObjects );
	}

	delete m_pWorld;
}

void ATHObjectManager::AddObject( ATHObject* pObject )
{
	if( pObject )
		m_liObjects.push_back( pObject );
}

void ATHObjectManager::AddObjectStatic( ATHObject* pObject )
{
	if( pObject )
		m_liStaticObjects.push_back( pObject );
}