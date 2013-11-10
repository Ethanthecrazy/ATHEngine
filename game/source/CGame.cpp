
#include "CGame.h"
#include <string>
#include <iostream>
#include <time.h>
using std::string;

#include "../../engine/ATHUtil/MemoryManager.h"
#include "../../engine/ATHRenderer/ATHRenderer.h"
#include "../../engine/Objects/ATHObjectManager.h"

// For testing purposes
#include "../../engine/ATHRenderer/RenderFunctions.h"
#include "../../engine/Objects/ATHObject.h"
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
	
	//ATHRenderNode* pTestNode = m_pRenderer->CreateRenderNode( "test", 0 );
	//pTestNode->SetMesh( m_pRenderer->GetQuad() );
	//pTestNode->SetTexture( m_pRenderer->GetAtlas()->GetTexture( "wall" ) );
	//D3DXMATRIX matTrans;
	//D3DXMatrixTranslation( &matTrans, (rand() % 1000 / 100.0f ) - 5.0f, (rand() % 1000 / 100.0f ) - 5.0f, 0.0f );
	//D3DXMATRIX scale;
	//D3DXMatrixScaling( &scale, 2.0f, 2.0f, 2.0f );
	//pTestNode->SetTransform( scale * matTrans ); 
	
	m_pRenderer->CreateRenderPass( "test", 1, RenderTest, "texture", true );

	//b2Vec2 vertices[4];
	//vertices[0].Set(0.5f, 0.5f);
	//vertices[1].Set(-0.5f, 0.5f);
	//vertices[2].Set(-0.5f, -0.5f);
	//vertices[3].Set(0.5f, -0.5f);

	//int32 count = 4;
	//b2PolygonShape polygon;
	//b2BodyDef bodyDef;

	//polygon.Set(vertices, count);
	//bodyDef.type = b2_dynamicBody;
	//
	//for( unsigned int i = 0; i < 50; ++i )
	//{
	//	bodyDef.position = b2Vec2( 0.0f, i * 2.0f + 3.0f );

	//	b2Body* pBody = m_pObjectManager->m_pWorld->CreateBody( &bodyDef );
	//	pBody->CreateFixture( &polygon, 1.0f );

	//	ATHRenderNode* pTestNode = m_pRenderer->CreateRenderNode( "test", 0 );
	//	pTestNode->SetMesh( &m_pRenderer->m_Quad );
	//	pTestNode->SetTexture( m_pRenderer->GetAtlas()->GetTexture( "wall" ) );

	//	ATHObject* testObject = new ATHObject();
	//	testObject->Init( pTestNode, pBody );

	//	m_pObjectManager->AddObject( testObject );
	//}

	m_pObjectManager->LoadObjectsFromXML();
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