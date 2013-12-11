
#include "CGame.h"
#include <string>
#include <iostream>
#include <time.h>
using std::string;

#include "../../engine/ATHUtil/MemoryManager.h"
#include "../../engine/ATHRenderer/ATHRenderer.h"
#include "../../engine/Objects/ATHObjectManager.h"
#include "RenderFunctions.h"

// For testing purposes
#include "../../engine/Objects/ATHObject.h"
#include "../../engine/ATHScriptManager/ATHScriptManager.h"
//////////

// default constructor
CGame::CGame() :	m_pObjectManager( nullptr ),
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

	srand( unsigned int( time( 0 ) ) );


	//// Testing Code ////
	TestInit();
	//////////

	m_Timer.Reset();
	m_Timer.Start();

}

void CGame::TestInit()
{
		
	m_pRenderer->CreateRenderPass( "test", 1, RenderTest, "texture", true );
	m_pObjectManager->LoadObjectsFromXML();
	std::cout << "Got Sum " << ATHObjectLuaTest() << " from LUA.\n";
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
	
}

bool CGame::Update( float fDT )
{
	m_pObjectManager->Update( fDT );
	return true;

}

void CGame::PostUpdate( float fDT )
{
	// Do Render Processing
	Render();
	//////////

	//Replace with an in-engine menu
	if( GetAsyncKeyState( 'W' ) )
	{
		ATHRenderer::GetInstance()->GetCamera()->ViewTranslateLocalZ( 0.1f );
	}
	if( GetAsyncKeyState( 'S' ) )
	{
		ATHRenderer::GetInstance()->GetCamera()->ViewTranslateLocalZ( -0.1f );
	}
	if( GetAsyncKeyState( 'A' ) )
	{
		
	}
	if( GetAsyncKeyState( 'D' ) )
	{
		
	}
	if( GetAsyncKeyState( VK_F2 ) & 1 )
	{
		MemoryManager::GetInstance()->DebugString();
	}
	if( GetAsyncKeyState( VK_F3 ) & 1 )
	{
		m_pObjectManager->ClearObjects();
		m_pObjectManager->LoadObjectsFromXML();
	}
	if( GetAsyncKeyState( VK_F4 ) & 1 )
	{ 
		m_pObjectManager->LoadObjectsFromXML();
	}

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
	m_pObjectManager->Shutdown();
	delete m_pObjectManager;

	m_pRenderer->Shutdown();
	m_pRenderer->DeleteInstance();
}