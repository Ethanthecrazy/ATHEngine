
#include "CGame.h"
#include <string>
#include <iostream>
#include <time.h>
using std::string;

#include "../../engine/ATHRenderer/ATHRenderer.h"
#include "../../engine/ATHObjectSystem/ATHObjectManager.h"
#include "../../engine/ATHScriptManager/ATHScriptManager.h"
#include "../../engine/ATHInputManager/ATHInputManager.h"
#include "../../engine/ATHEventSystem/ATHEventManager.h"
#include "../../engine/ATHSoundSystem/ATHAudio.h"

// For testing purposes
#include "../../engine/ATHObjectSystem/ATHObject.h"

#include "../../engine/ATHUtil/ATHRand.h"
//////////

// default constructor
CGame::CGame() :	m_fFrameTime( 0.0f ), 
					m_unFrameCounter( 0 )
{
	bFullscreen = false;
	bShutdown = false;
}

// destructor
CGame::~CGame() 
{ 

}

// singleton accessor
CGame* CGame::GetInstance()
{
	static CGame instance;
	return &instance;
}

// initialization
void CGame::Initialize()
{
	m_Engine.Init();
	m_ObjectGenerator.Init(m_Engine.GetObjectManager());

	cout << "CGame: Init\n"; 

	m_Timer.Reset();
	m_Timer.Start();

	for (unsigned int i = 0; i < 25; ++i)
	{
		ATHObject* pObj = m_Engine.GetObjectManager()->InstanceObject(float3(0.0f, 10.0f, 0.0f), "Square");
		pObj->GetBody()->ApplyLinearImpulse(b2Vec2(70.0f, -55.0f), pObj->GetBody()->GetWorldCenter());
	}

	ATHRandom::SeedRand( (unsigned long)time(0) );
	ATHRandom::SeedFastRand( (unsigned int)time(0) );

	m_ObjectGenerator.GeneratePlanet(float2(32.0f, -32.0f), 5.0f, 10.0f, float3(0.9f, 0.3f, 0.1f));
}

// execution
bool CGame::Main()
{
	if (m_Engine.isShutdown())
		return false;

	static ATHObject* s_pObj = nullptr;
	if (!s_pObj)
	{
		s_pObj = m_ObjectGenerator.GeneratePlanet(float2(0.0f, 0.0f), 3.0f, 3.0f, float3(ATHRandom::Rand(0.0f, 1.0f), ATHRandom::Rand(0.0f, 1.0f), ATHRandom::Rand(0.0f, 1.0f)));
	}

	if (GetAsyncKeyState(VK_F1) & 1)
	{
		s_pObj->SetAlive(false);
		s_pObj = m_ObjectGenerator.GeneratePlanet(float2(0.0f, 0.0f), 3.0f, 10.0f, float3(ATHRandom::Rand(0.0f, 1.0f), ATHRandom::Rand(0.0f, 1.0f), ATHRandom::Rand(0.0f, 1.0f)));
	}

	m_fElapsedTime = (float)m_Timer.GetElapsedTime();
	m_Timer.Reset();

	m_fFrameTime += m_fElapsedTime;
	m_unFrameCounter++;
	if( m_fFrameTime >= 1.0f )
	{
		std::cout << "FPS: " << m_unFrameCounter << "\n";
		m_fFrameTime = 0.0f;
		m_unFrameCounter = 0;
	}

	if( !m_Engine.Update( m_fElapsedTime ) )
		return false;

	Render();

	return true;
}

void CGame::Render()
{
	m_Engine.Render();
}

// cleanup
void CGame::Shutdown()
{
	m_Engine.Shutdown();
}