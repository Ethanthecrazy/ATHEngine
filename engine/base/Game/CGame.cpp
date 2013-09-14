
#include "CGame.h"
#include <string>
#include <iostream>
#include <time.h>
using std::string;

#include "../ATHUtil/MemoryManager.h"
#include "../ATHRenderer/ATHRenderer.h"
#include "../Objects/ATHObjectManager.h"

// For testing purposes
#include "../ATHRenderer/RenderFunctions.h"
//////////

// default constructor
CGame::CGame() : m_pObjectManager( nullptr )
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

}

void CGame::TestInit()
{
	m_pRenderer->CreateRenderPass( "test", 1, RenderTest, "texture", true );

	for( int i = 0; i < 10; ++i )
	{

		ATHRenderNode* pTestNode = m_pRenderer->CreateRenderNode( "test", 0 );

		pTestNode->SetMesh( m_pRenderer->GetQuad() );
		pTestNode->SetTexture( m_pRenderer->GetAtlas()->GetTexture( "wall" ) );

		D3DXMATRIX matTrans;
		D3DXMatrixTranslation( &matTrans, (rand() % 1000 / 100.0f ) - 5.0f, (rand() % 1000 / 100.0f ) - 5.0f, 0.0f );

		D3DXMATRIX scale;
		D3DXMatrixScaling( &scale, 2.0f, 2.0f, 2.0f );

		pTestNode->SetTransform( scale * matTrans ); 
	}

	m_pRenderer->DebugLinesAdd( float3( 0.0f ), float3( 0.0f, 5.0f, 0.0f ), float4( 1.0f, 0.0f, 0.0f, 1.0f ) );
}

// execution
bool CGame::Main()
{

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
		ATHRenderer::GetInstance()->GetCamera()->ViewRotateGlobalY( -0.05f );
	}
	if( GetAsyncKeyState( 'D' ) )
	{
		ATHRenderer::GetInstance()->GetCamera()->ViewRotateGlobalY( 0.05f );
	}
	if( GetAsyncKeyState( VK_F2 ) & 1 )
	{
		MemoryManager::GetInstance()->DebugString();
	}

	// File Manager Update Here
	// 
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
	m_pObjectManager->Shutdown();
	delete m_pObjectManager;

	m_pRenderer->Shutdown();
	m_pRenderer->DeleteInstance();
}