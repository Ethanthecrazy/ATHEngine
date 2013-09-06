
#include "CGame.h"
#include <string>
#include <iostream>
#include <time.h>
using std::string;

#include "../ATHUtil/MemoryManager.h"
#include "../ATHRenderer/ATHRenderer.h"

// For testing purposes
#include "../ATHRenderer/RenderFunctions.h"
//////////

// default constructor
CGame::CGame()
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

	m_pRenderer->CreateRenderPass( "test", 1, RenderTest, "texture" );

	for( int i = 0; i < 50; ++i )
	{

		ATHRenderNode* pTestNode = m_pRenderer->CreateRenderNode( "test", 0 );

		pTestNode->SetMesh( m_pRenderer->GetQuad() );
		pTestNode->SetTexture( m_pRenderer->GetAtlas()->GetTexture( "wall" ) );

		D3DXMATRIX matTrans;
		D3DXMatrixTranslation( &matTrans, (rand() % 1000 / 100.0f ) - 5.0f, (rand() % 1000 / 100.0f ) - 5.0f, 5.0f + (rand() % 1000 / 150.0f ) );

		D3DXMATRIX scale;
		D3DXMatrixScaling( &scale, 2.0f, 2.0f, 2.0f );

		pTestNode->SetTransform( scale * matTrans ); 
	}

	//////////

	srand( unsigned int( time( 0 ) ) );

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


		return true;

}

void CGame::PostUpdate( float fDT )
{
	// Do Render Processing
	Render();
	//////////

	// File Manager Update Here
	// 
	//////////
}

void CGame::Render()
{
	m_pRenderer->DRXClear( float3( 0.5f, 0.5f, 0.7f ) );
	m_pRenderer->DRXBegin();

	m_pRenderer->CommitDraws();

	m_pRenderer->DRXEnd();

	m_pRenderer->DRXPresent();
}

// cleanup
void CGame::Shutdown()
{
	m_pRenderer->Shutdown();
	m_pRenderer->DeleteInstance();
}