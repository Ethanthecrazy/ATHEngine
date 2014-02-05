
#include "CGame.h"
#include <string>
#include <iostream>
#include <time.h>
using std::string;

#include "../../engine/ATHUtil/MemoryManager.h"
#include "../../engine/ATHRenderer/ATHRenderer.h"
#include "../../engine/ATHObjectSystem/ATHObjectManager.h"
#include "../../engine/ATHScriptManager/ATHScriptManager.h"
#include "../../engine/ATHInputManager/ATHInputManager.h"
#include "../../engine/ATHEventSystem/ATHEventManager.h"

// For testing purposes
#include "../../engine/ATHObjectSystem/ATHObject.h"
//////////

// default constructor
CGame::CGame() :	m_pObjectManager( nullptr ),
					m_pRenderer( nullptr ),
					m_pScriptManager( nullptr ),
					m_fFrameTime( 0.0f ), 
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
void CGame::Initialize(HWND _hWnd, HINSTANCE hInstance, 
	int nScreenWidth, int nScreenHeight, bool bIsWindowed)
{

	cout << "CGame: Init\n"; 
	srand( unsigned int( time( 0 ) ) );
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;

	m_pRenderer = ATHRenderer::GetInstance();
	m_pRenderer->Initialize( _hWnd, hInstance, nScreenWidth, nScreenHeight, false, true );

	m_pObjectManager = new ATHObjectManager();
	m_pObjectManager->Init();

	m_pInputManager = ATHInputManager::GetInstance();
	m_pInputManager->Init( _hWnd, hInstance, nScreenWidth, nScreenHeight, nScreenWidth / 2, nScreenHeight / 2 );

	m_pEventManager = ATHEventManager::GetInstance();

	srand( unsigned int( time( 0 ) ) );


	//// Testing Code ////
	TestInit();
	//////////

	m_Timer.Reset();
	m_Timer.Start();

}

void CGame::TestInit()
{
	ATHObject* pTemp = new ATHObject();
	m_pEventManager->RegisterClient( AET_KEYBOARD, pTemp );
	m_pObjectManager->AddObject( pTemp );

	m_pObjectManager->InstanceObject(float3( 0.0f, 9.0f, 0.0f ), "CircleTest");
}

// execution
bool CGame::Main()
{
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

	PreUpdate( m_fElapsedTime );
	
	if( !Update( m_fElapsedTime ) )
		return false;

	PostUpdate( m_fElapsedTime );

	return true;
}

void CGame::PreUpdate( float fDT )
{
	m_pInputManager->Update();
}

bool CGame::Update( float fDT )
{
	m_pObjectManager->Update( fDT );
	m_pEventManager->ProcessEvents();
	return true;

}

void CGame::PostUpdate( float fDT )
{
	// Do Render Processing
	Render();
	//////////
}

void CGame::Render()
{
	m_pRenderer->DRXBegin();

	m_pRenderer->CommitDraws();

	m_pRenderer->DRXEnd();

	m_pRenderer->DRXPresent();
}

// cleanup
void CGame::Shutdown()
{
	m_pEventManager->Shutdown();
	m_pEventManager->DeleteInstance();

	m_pInputManager->Shutdown();
	m_pInputManager->DeleteInstance();

	m_pObjectManager->Shutdown();
	delete m_pObjectManager;

	m_pRenderer->Shutdown();
	m_pRenderer->DeleteInstance();
}