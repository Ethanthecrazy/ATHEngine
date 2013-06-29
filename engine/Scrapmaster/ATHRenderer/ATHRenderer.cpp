#include "ATHRenderer.h"

#include <iostream>

#include "Camera.h"
#include "ATHRenderNode.h"
#include "../ATHUtil/MemoryManager.h"


ATHRenderer* ATHRenderer::m_pInstance = nullptr;

void ATHRenderer::BuildQuad()
{
	m_meshQuad.GetVerts().push_back( sVertPosNormUV( float3( 0.0f, 1.0f, 0.0f ), float3( 0.0f, 0.0f, -1.0f ), float2( 0.0f, 0.0f ) ) );
	m_meshQuad.GetVerts().push_back( sVertPosNormUV( float3( 1.0f, 1.0f, 0.0f ), float3( 0.0f, 0.0f, -1.0f ), float2( 1.0f, 0.0f ) ) );
	m_meshQuad.GetVerts().push_back( sVertPosNormUV( float3( 1.0f, 0.0f, 0.0f ), float3( 0.0f, 0.0f, -1.0f ), float2( 1.0f, 1.0f ) ) );
	m_meshQuad.GetVerts().push_back( sVertPosNormUV( float3( 0.0f, 0.0f, 0.0f ), float3( 0.0f, 0.0f, -1.0f ), float2( 0.0f, 1.0f ) ) );

	m_meshQuad.GetIndicies().push_back( 0 );
	m_meshQuad.GetIndicies().push_back( 2 );
	m_meshQuad.GetIndicies().push_back( 3 );

	m_meshQuad.GetIndicies().push_back( 0 );
	m_meshQuad.GetIndicies().push_back( 1 );
	m_meshQuad.GetIndicies().push_back( 2 );

	m_meshQuad.RebuildBuffers();

}

ATHRenderNode* ATHRenderer::CreateNode()
{
	if( m_pNodeInventory.size() > 0 )
	{
		ATHRenderNode* toReturn = m_pNodeInventory.back();
		m_pNodeInventory.pop_back();
		return toReturn;
	}
	else
		return ATHNew<ATHRenderNode>("Rendering");
}

void ATHRenderer::DestroyNode( ATHRenderNode* _toDestroy )
{
	if( _toDestroy )
	{
		*_toDestroy = ATHRenderNode();
		m_pNodeInventory.push_back( _toDestroy );
	}
}

void ATHRenderer::ClearInventory()
{
	while( m_pNodeInventory.size() )
	{
		ATHDelete( m_pNodeInventory.back() );
		m_pNodeInventory.pop_back();
	}
}

ATHRenderer::ATHRenderer()
{
	m_FrameCounter		= 0;		// Frame Counter
	m_unScreenWidth		= 0;
	m_unScreenHeight	= 0;

	m_hWnd;					// Windows handle.
	m_hInstance;

	m_pD3D				= nullptr;		// The Direct3d Object
	m_pDevice			= nullptr;		// The Device
	m_pvdPosNormUV		= nullptr;

	ZeroMemory( &m_PresentParams, sizeof( D3DPRESENT_PARAMETERS ) );

	m_pCamera			= nullptr;

	m_pNodeInventory	= std::list<ATHRenderNode*>();

}

ATHRenderer::~ATHRenderer()
{

}

ATHRenderer* ATHRenderer::GetInstance()
{
	if( !m_pInstance )
		m_pInstance = ATHNew<ATHRenderer>("Rendering");
	return m_pInstance;

}

void ATHRenderer::DeleteInstance()
{
	if( m_pInstance )
	{
		ATHDelete( m_pInstance );
		m_pInstance = nullptr;
	}
}

bool ATHRenderer::Initialize( HWND hWnd, HINSTANCE hInstance, unsigned int nScreenWidth, unsigned int nScreenHeight, bool bFullScreen, bool bVsync )
{
	m_hWnd				= hWnd;
	m_hInstance			= hInstance;
	m_unScreenWidth		= nScreenWidth;
	m_unScreenHeight	= nScreenHeight;

	m_pD3D				= Direct3DCreate9( D3D_SDK_VERSION );

	m_PresentParams.BackBufferWidth				= nScreenWidth;
	m_PresentParams.BackBufferHeight			= nScreenHeight;
	m_PresentParams.BackBufferFormat			= (!bFullScreen) ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;
	m_PresentParams.BackBufferCount				= 1;
	m_PresentParams.Windowed					= !bFullScreen;
	m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParams.MultiSampleQuality			= NULL;
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	m_PresentParams.EnableAutoDepthStencil		= TRUE;
	m_PresentParams.AutoDepthStencilFormat		= D3DFMT_D16;
	m_PresentParams.hDeviceWindow				= hWnd;
	m_PresentParams.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_PresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;


	m_PresentParams.PresentationInterval		= (bVsync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	HRESULT hr = 0;

	if( FAILED( hr = m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&m_PresentParams, &m_pDevice )))
	{
		MessageBoxW( hWnd, L"Failed to create the device.", NULL, MB_OK );
		return false;
	}

	D3DVERTEXELEMENT9 decl[] = 
	{ 
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0},
		D3DDECL_END()
	};

	m_pDevice->CreateVertexDeclaration(decl, &m_pvdPosNormUV);
	m_pDevice->SetVertexDeclaration(m_pvdPosNormUV);

	ID3DXBuffer	*errors(NULL);

	D3DXCreateEffectFromFile( m_pDevice, "resource/texture.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pEffect, &errors ); 

	if( errors )
	{
		std::cout << (char*)errors->GetBufferPointer();
		errors = NULL;
		return false;
	}

	m_pCamera = ATHNew<CCamera>("Rendering");
	m_pCamera->BuildPerspective(D3DX_PI / 2.0f, ((float)(m_unScreenWidth))/m_unScreenHeight, 0.1f, 10000.0f);
	m_pCamera->SetViewPosition(0.0f, 0.0f, -680.0f);

	m_TextureAtlas.Initialize( m_pDevice );

	BuildQuad();

	return true;
}

void ATHRenderer::CommitDraws()
{

}

void ATHRenderer::RasterTexture( LPDIRECT3DTEXTURE9 _texture, float _left, float _top, float _right, float _bottom )
{
	D3DXMATRIX _matProj;
	D3DXMatrixIdentity( &_matProj );

	D3DXMATRIX scale;
	D3DXMatrixScaling( &scale, (_right - _left) * 2.0f, (_bottom - _top) * 2.0f, 1.0f );
	_matProj *= scale;

	D3DXMATRIX translate;
	D3DXMatrixTranslation( &translate, (_left * 2) - 1.0f, ( ( -_bottom )), 0.0f );
	_matProj *= translate;

	unsigned int passes(0);
	m_pEffect->Begin( &passes, 0 );
	for( unsigned int i(0); i < passes; ++i )
	{
		m_pEffect->BeginPass( i );
		{
			m_pEffect->SetTexture("tex1", _texture );
			m_pEffect->SetFloatArray( "multColor", float4( 1.0f, 1.0f, 1.0f, 1.0f ).Array, 4 );
			m_pEffect->SetMatrix("gWVP", &( _matProj ) );
			m_pEffect->CommitChanges();

			m_pDevice->SetStreamSource( 0, m_meshQuad.GetVertexBuffer(), 0, sizeof( sVertPosNormUV ) );
			m_pDevice->SetVertexDeclaration( m_pvdPosNormUV );
			m_pDevice->SetIndices( m_meshQuad.GetIndexBuffer() );
			m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );
		}
		m_pEffect->EndPass();
	}
	m_pEffect->End();
}

void ATHRenderer::DRXClear( float3 _color )
{
	//Cleat the current render target and the Z-Buffer
	m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,  D3DCOLOR_COLORVALUE( _color.cR,  _color.cG,  _color.cB, 1.0f ), 1.0f, 0);

	// Check for lost device (could happen from an ALT+TAB or ALT+ENTER).
	if (m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ResetDevice();
	}
}

void ATHRenderer::DRXBegin()
{
	m_pDevice->BeginScene();
	if( m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		ResetDevice();
}

void ATHRenderer::DRXEnd()
{
	m_pDevice->EndScene();
	if( m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		ResetDevice();
}

void ATHRenderer::DRXPresent()
{
	m_pDevice->Present( NULL, NULL, NULL, NULL );
	if( m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		ResetDevice();
	IncrementFrameCounter();
}

void ATHRenderer::Shutdown()
{
	if( m_pCamera )
		ATHDelete( m_pCamera );

	m_meshQuad.Clear();

	ClearInventory();

	if( m_pEffect )
		m_pEffect->Release();

	m_TextureAtlas.Shutdown();

	m_pvdPosNormUV->Release();
	m_pDevice->Release();
	m_pD3D->Release();
}

void ATHRenderer::ChangeDisplayParam( int nScreenWidth, int nScreenHeight, bool bFullScreen, bool bVsync )
{
	//Change the display parameters
	m_PresentParams.BackBufferWidth				= nScreenWidth;
	m_PresentParams.BackBufferHeight			= nScreenHeight;
	m_PresentParams.BackBufferFormat			= (!bFullScreen) ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;

	m_PresentParams.Windowed					= !bFullScreen;

	m_PresentParams.FullScreen_RefreshRateInHz	= (bVsync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	m_PresentParams.PresentationInterval		= (bVsync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	
	ResetDevice();

	if (!bFullScreen)
	{
		// Setup the desired client area size
		RECT rWindow;
		rWindow.left	= 0;
		rWindow.top		= 0;
		rWindow.right	= nScreenWidth;
		rWindow.bottom	= nScreenHeight;

		// Calculate the width/height of that window's dimensions
		int windowWidth		= rWindow.right - rWindow.left;
		int windowHeight	= rWindow.bottom - rWindow.top;

		SetWindowPos(m_hWnd, HWND_TOP,	(GetSystemMetrics(SM_CXSCREEN)>>1) - (windowWidth>>1),
			(GetSystemMetrics(SM_CYSCREEN)>>1) - (windowHeight>>1),
			windowWidth, windowHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
}

void ATHRenderer::ResetDevice(void)
{
	m_pDevice->Reset( &m_PresentParams );
}