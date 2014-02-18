
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
	srand( unsigned int( time( 0 ) ) );

	m_Timer.Reset();
	m_Timer.Start();
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