
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
#include "ObjectGenerator.h"
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

	cout << "CGame: Init\n"; 

	m_Timer.Reset();
	m_Timer.Start();

	ObjectGenerator objGen;
	objGen.Init(m_Engine.GetObjectManager());
	objGen.GeneratePlanet(float2(0.0f, 0.0f), 3.0f, 5.0f);

	for (unsigned int i = 0; i < 50; ++i )
		m_Engine.GetObjectManager()->InstanceObject(float3(0.0f, 10.0f, 0.0f), "Square");
}

// execution
bool CGame::Main()
{
	if (m_Engine.isShutdown())
		return false;

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