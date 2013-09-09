#include "ATHRenderer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


#include "Camera.h"
#include "ATHRenderNode.h"
#include "../ATHUtil/NewInclude.h"
#include "Texture/ATHAtlas.h"
#include "Mesh/ATHVertexDecl.h"

ATHRenderer* ATHRenderer::m_pInstance = nullptr;
// The sorting predicate for the ATHRenderPass pointers
bool compare_ATHRenderPass( ATHRenderPass* _first, ATHRenderPass* _second )
{
	return ( _first->GetPriority() > _second->GetPriority() );
}
//================================================================================
ATHRenderNode* ATHRenderer::CreateNode()
{
	if( m_pNodeInventory.size() > 0 )
	{
		ATHRenderNode* toReturn = m_pNodeInventory.back();
		m_pNodeInventory.pop_back();
		return toReturn;
	}
	else
	{
		ATHRenderNode* pNewNode = new ATHRenderNode();
		m_liNodeTotalList.push_back( pNewNode );
		return pNewNode;
	}
}
//================================================================================
void ATHRenderer::DestroyNode( ATHRenderNode* _toDestroy )
{
	if( _toDestroy )
	{
		*_toDestroy = ATHRenderNode();
		m_pNodeInventory.push_back( _toDestroy );
	}
}
//================================================================================
void ATHRenderer::DestoryAllNodes()
{
	while( m_liNodeTotalList.size() )
	{
		delete m_liNodeTotalList.back();
		m_liNodeTotalList.pop_back();
	}

	m_pNodeInventory.clear();
}
//================================================================================
ATHRenderer::ATHRenderer() : m_Quad( "Quad", GetVertexDeclaration( ATH_VERTEXDECL_TEXTURED), D3DPT_TRIANGLELIST )
{
	m_FrameCounter		= 0;		// Frame Counter
	m_unScreenWidth		= 0;
	m_unScreenHeight	= 0;

	m_hWnd;					// Windows handle.
	m_hInstance;

	m_pD3D				= nullptr;		// The Direct3d Object
	m_pDevice			= nullptr;		// The Device

	ZeroMemory( &m_PresentParams, sizeof( D3DPRESENT_PARAMETERS ) );

	m_pCamera			= nullptr;

	m_pNodeInventory	= std::list<ATHRenderNode*>();

}
//================================================================================
ATHRenderer::~ATHRenderer()
{

}
//================================================================================
ATHRenderer* ATHRenderer::GetInstance()
{
	if( !m_pInstance )
		m_pInstance = new ATHRenderer();
	return m_pInstance;

}
//================================================================================
void ATHRenderer::DeleteInstance()
{
	if( m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//================================================================================
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

	InitVertexDecls();

	m_pTextureAtlas = new ATHAtlas();
	m_pTextureAtlas->Initialize( m_pDevice );

	LoadTextures( TEXTURE_LOAD_PATH );
	LoadShaders( SHADER_LOAD_PATH );

	m_pCamera = new CCamera();
	m_pCamera->BuildPerspective(D3DX_PI / 2.0f, ((float)(m_unScreenWidth))/m_unScreenHeight, 0.1f, 10000.0f);
	m_pCamera->SetViewPosition(0.0f, 0.0f, 0.0f);


	BuildQuad();

	return true;
}

void ATHRenderer::InitVertexDecls()
{
	ATHVertexDecl* pNewDecl = nullptr;

	// Colored dcl
	pNewDecl = new ATHVertexDecl();
	pNewDecl->AddVertexElement( D3DDECLUSAGE_POSITION, D3DDECLTYPE_FLOAT3 );
	pNewDecl->AddVertexElement( D3DDECLUSAGE_COLOR, D3DDECLTYPE_FLOAT4 );
	pNewDecl->BuildDecl();
	m_mapVertDecls.insert( std::pair< unsigned int, ATHVertexDecl* >( ATH_VERTEXDECL_COLORED, pNewDecl ) );

	// Textured decl
	pNewDecl = new ATHVertexDecl();
	pNewDecl->AddVertexElement( D3DDECLUSAGE_POSITION, D3DDECLTYPE_FLOAT3 );
	pNewDecl->AddVertexElement( D3DDECLUSAGE_TEXCOORD, D3DDECLTYPE_FLOAT2 );
	pNewDecl->BuildDecl();
	m_mapVertDecls.insert( std::pair< unsigned int, ATHVertexDecl* >( ATH_VERTEXDECL_TEXTURED, pNewDecl ) );

}

//================================================================================
void ATHRenderer::Shutdown()
{
	std::map< unsigned int, ATHVertexDecl* >::iterator itrDecls = m_mapVertDecls.begin();
	while( itrDecls != m_mapVertDecls.end() )
	{
		if( itrDecls->second != nullptr )
		{
			delete itrDecls->second;
		}

		itrDecls++;
	}

	if( m_pCamera )
		delete m_pCamera;



	DestoryAllNodes();

	UnloadShaders();

	m_pTextureAtlas->Shutdown();
	delete m_pTextureAtlas;
	m_pTextureAtlas = nullptr;

	m_pDevice->Release();
	m_pD3D->Release();
}

//================================================================================
void ATHRenderer::CommitDraws()
{
	std::list< ATHRenderPass* >::iterator itrPass = m_liSortedRenderPasses.begin();
	while( itrPass != m_liSortedRenderPasses.end() )
	{
		(*itrPass)->PreExecute();
		(*itrPass)->Execute( this );
		itrPass++;
	}
}
//================================================================================
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

	//unsigned int passes(0);
	//m_pEffect->Begin( &passes, 0 );
	//for( unsigned int i(0); i < passes; ++i )
	//{
	//	m_pEffect->BeginPass( i );
	//	{
	//		m_pEffect->SetTexture("tex1", _texture );
	//		m_pEffect->SetFloatArray( "multColor", float4( 1.0f, 1.0f, 1.0f, 1.0f ).Array, 4 );
	//		m_pEffect->SetMatrix("gWVP", &( _matProj ) );
	//		m_pEffect->CommitChanges();

	//		m_pDevice->SetStreamSource( 0, m_meshQuad.GetVertexBuffer(), 0, sizeof( sVertPosNormUV ) );
	//		m_pDevice->SetVertexDeclaration( m_pvdPosNormUV );
	//		m_pDevice->SetIndices( m_meshQuad.GetIndexBuffer() );
	//		m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );
	//	}
	//	m_pEffect->EndPass();
	//}
	//m_pEffect->End();
}
//================================================================================
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
//================================================================================
void ATHRenderer::DRXBegin()
{
	m_pDevice->BeginScene();
	if( m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		ResetDevice();
}
//================================================================================
void ATHRenderer::DRXEnd()
{
	m_pDevice->EndScene();
	if( m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		ResetDevice();
}
//================================================================================
void ATHRenderer::DRXPresent()
{
	m_pDevice->Present( NULL, NULL, NULL, NULL );
	if( m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		ResetDevice();
	IncrementFrameCounter();
}
//================================================================================
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
//================================================================================
void ATHRenderer::ResetDevice(void)
{
	m_pDevice->Reset( &m_PresentParams );
}
//================================================================================
void ATHRenderer::LoadTextures( char* _path )
{
		// Data for searching
	WIN32_FIND_DATA search_data;
	memset( &search_data, 0, sizeof( WIN32_FIND_DATA ) );

	// Setup the path to start the search
	std::string pathToDirectory = std::string( _path );
	pathToDirectory += "\\*";

	HANDLE handle = FindFirstFile( pathToDirectory.c_str(), &search_data );

	while( handle != INVALID_HANDLE_VALUE )
	{
		if( !( search_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			// Get the filename of the file
			std::string szFilename = search_data.cFileName;
			
			// Get the file extension
			unsigned int unExtenPos = szFilename.find_last_of( "." );
			std::string szFileExtension = szFilename.substr( unExtenPos );

			// Make sure that it is the correct filetype
			if( strcmp( szFileExtension.c_str(), TEXTURE_SEARCH_EXTENSION ) == 0 )
			{
				//Generate the filepath to the effect file
				std::string pathToFile = std::string( _path );
				pathToFile += szFilename;

				//Generate the name of the shader
				std::string szName = szFilename.substr( 0, unExtenPos);

				ID3DXEffect* pCurrEffect(NULL);
				ID3DXBuffer	*errors(NULL);

				GetAtlas()->LoadTexture( (char*)szName.c_str(), (char*)pathToFile.c_str() );
				std::cout << "Loaded Texture: " << pathToFile << "\n";

			}
		}

		if( FindNextFile(handle, &search_data ) == FALSE )
			break;
	}

	//Close the handle after use or memory/resource leak
	FindClose(handle);
}
//================================================================================
ATHVertexDecl*	ATHRenderer::GetVertexDeclaration( unsigned int _unHandle )
{
	if( m_mapVertDecls.count( _unHandle ) > 0 )
		return m_mapVertDecls.at( _unHandle );
	else
		return nullptr;
}
//================================================================================
void ATHRenderer::LoadShaders( char* _path )
{
	// Data for searching
	WIN32_FIND_DATA search_data;
	memset( &search_data, 0, sizeof( WIN32_FIND_DATA ) );

	// Setup the path to start the search
	std::string pathToDirectory = std::string( _path );
	pathToDirectory += "\\*";

	HANDLE handle = FindFirstFile( pathToDirectory.c_str(), &search_data );

	while( handle != INVALID_HANDLE_VALUE )
	{
		if( !( search_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			// Get the filename of the file
			std::string szFilename = search_data.cFileName;
			
			// Get the file extension
			unsigned int unExtenPos = szFilename.find_last_of( "." );
			std::string szFileExtension = szFilename.substr( unExtenPos );

			// Make sure that it is the correct filetype
			if( strcmp( szFileExtension.c_str(), SHADER_SEARCH_EXTENSION ) == 0 )
			{
				//Generate the filepath to the effect file
				std::string pathToFile = std::string( _path );
				pathToFile += szFilename;

				//Generate the name of the shader
				std::string szName = szFilename.substr( 0, unExtenPos);

				ID3DXEffect* pCurrEffect(NULL);
				ID3DXBuffer	*errors(NULL);

				// Create the effect file
				D3DXCreateEffectFromFile( m_pDevice, pathToFile.c_str(), NULL, NULL, 0, NULL, &pCurrEffect, &errors );

				if( errors ) // Output errors
				{
					std::cout << "ERROR: " << pathToFile << "\n";
					std::cout << (char*)errors->GetBufferPointer();
					std::cout << "\n";
					errors->Release();
					errors = NULL;
				}
				else // Load and output success
				{
					m_mapEffects[ szName ] = pCurrEffect;
					std::cout << "Loaded Shader: " << pathToFile << "\n";
				}
			}
		}

		if( FindNextFile(handle, &search_data ) == FALSE )
			break;
	}

	//Close the handle after use or memory/resource leak
	FindClose(handle);

}
//================================================================================
void ATHRenderer::UnloadShaders()
{
	std::map< std::string, ID3DXEffect* >::iterator itrShader = m_mapEffects.begin();
	while( itrShader != m_mapEffects.end() )
	{
		itrShader->second->Release();
		itrShader = m_mapEffects.erase( itrShader );
	}

}

ID3DXEffect* ATHRenderer::GetShader( char* _szName )
{
	std::string strName = _szName;
	ID3DXEffect* pToReturn = nullptr;

	if( m_mapEffects.count( strName ) > 0 )
		pToReturn = m_mapEffects.at( strName );

	return pToReturn;
}

//================================================================================
ATHRenderPass*	ATHRenderer::CreateRenderPass( char* _szName, unsigned int _unPriority, RenderFunc _function,  char* _szShaderName, char* _szTechnique )
{
	ATHRenderPass* pToReturn = nullptr;
	std::string	idString = std::string( _szName );

	if( m_mapRenderPasses.count( idString ) == 0 )
	{
		// Get the address of the pass being constructed and inserted into the map
		ATHRenderPass* pNewPass = &( m_mapRenderPasses[ idString ] = ATHRenderPass( _szName, _unPriority, GetShader( _szShaderName ), _function, _szTechnique ) );
		// Also add it to the sorted list.
		m_liSortedRenderPasses.push_back( pNewPass );
		m_liSortedRenderPasses.sort( compare_ATHRenderPass );
	}

	return pToReturn;
}
//================================================================================
ATHRenderPass*	ATHRenderer::FindRenderPass( char* _szName )
{
	ATHRenderPass* pToReturn = nullptr;
	std::string idString = std::string( _szName );

	if( m_mapRenderPasses.count( idString ) > 0 )
		pToReturn = &(m_mapRenderPasses[ idString ]);

	return pToReturn;
}
//================================================================================
bool ATHRenderer::DestroyRenderPass( char* _szName )
{
	std::string idString = std::string( _szName );
	if( m_mapRenderPasses.count( idString ) > 0 )
	{
		m_liSortedRenderPasses.remove( &(m_mapRenderPasses[idString]) );
		m_mapRenderPasses.erase( idString );
		return true;
	}

	return true;
}
//================================================================================
void ATHRenderer::ClearRenderPasses()
{
	m_liSortedRenderPasses.clear();
	m_mapRenderPasses.clear();
}
//================================================================================
ATHRenderNode* ATHRenderer::CreateRenderNode( char* _szPassName ,unsigned int _unPriority )
{
	ATHRenderNode* _toReturn = CreateNode();
	FindRenderPass( _szPassName )->AddNodeToPass( _toReturn, _unPriority );
	return _toReturn;
}
//================================================================================
void ATHRenderer::DestoryRenderNode( ATHRenderNode* _pDestroy )
{
	if( _pDestroy != nullptr )
	{
		std::vector<std::string> vecNames = _pDestroy->GetPassNames();
		std::vector<std::string>::iterator itrNames = vecNames.begin();
		while( itrNames != vecNames.end() )
		{
			FindRenderPass( (char*)(*itrNames).c_str() )->RemoveNodeFromPass( _pDestroy );
			++itrNames;
		}

		DestroyNode( _pDestroy );
	}
}
//================================================================================
void ATHRenderer::DrawMesh( ATHMesh* _pMesh )
{
	m_pDevice->SetVertexDeclaration( _pMesh->GetVertexDecl()->GetShortDecl() );
	m_pDevice->SetStreamSource( 0, _pMesh->GetVertexBuffer(), 0, _pMesh->GetVertexDecl()->GetVertexSize() );
	m_pDevice->SetIndices( _pMesh->GetIndexBuffer() );
	m_pDevice->DrawIndexedPrimitive( _pMesh->GetPrimativeType(), 0, 0, _pMesh->GetVertexCount(), 0, _pMesh->GetPrimativeCount() );
}
//================================================================================
ATHMesh* ATHRenderer::GetQuad()
{
	return &m_Quad;
}
//================================================================================
void ATHRenderer::BuildQuad()
{
	m_Quad = ATHMesh( "Quad", GetVertexDeclaration( ATH_VERTEXDECL_TEXTURED ), D3DPT_TRIANGLELIST );

	m_Quad.m_vecPositions.push_back( float3( -0.5f, 0.5f, -0.5f ) );
	m_Quad.m_vecPositions.push_back( float3( -0.5f, 0.5f, 0.5f ) );
	m_Quad.m_vecPositions.push_back( float3( 0.5f, 0.5f, 0.5f ) );
	m_Quad.m_vecPositions.push_back( float3( 0.5f, 0.5f, -0.5f ) );

	m_Quad.m_vecPositions.push_back( float3( -0.5f, -0.5f, -0.5f ) );
	m_Quad.m_vecPositions.push_back( float3( -0.5f, -0.5f, 0.5f ) );
	m_Quad.m_vecPositions.push_back( float3( 0.5f, -0.5f, 0.5f ) );
	m_Quad.m_vecPositions.push_back( float3( 0.5f, -0.5f, -0.5f ) );

	m_Quad.m_vecUVs.push_back( float2( 0.0f, 0.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 1.0f, 0.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 0.0f, 0.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 1.0f, 0.0f ) );

	m_Quad.m_vecUVs.push_back( float2( 0.0f, 1.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 1.0f, 1.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 0.0f, 1.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 1.0f, 1.0f ) );

	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( -0.5f, 0.5f, -0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 0.0f, 0.0f ) ) );	// 0
	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( -0.5f, 0.5f, 0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 1.0f, 0.0f ) ) );		// 1
	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( 0.5f, 0.5f, 0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 0.0f, 0.0f ) ) );		// 2
	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( 0.5f, 0.5f, -0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 1.0f, 0.0f ) ) );		// 3

	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( -0.5f, -0.5f, -0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 0.0f, 1.0f ) ) );	// 4
	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( -0.5f, -0.5f, 0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 1.0f, 1.0f ) ) );	// 5
	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( 0.5f, -0.5f, 0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 0.0f, 1.0f ) ) );     // 6
	//m_Quad.GetVerts().push_back( sVertPosNormUV( float3( 0.5f, -0.5f, -0.5f ), float3( 0.0f, 0.0f, 0.0f ), float2( 1.0f, 1.0f ) ) );	// 7

	// TOP Indicies
	m_Quad.m_vecIndicies.push_back( 0 );
	m_Quad.m_vecIndicies.push_back( 2 );
	m_Quad.m_vecIndicies.push_back( 3 );

	m_Quad.m_vecIndicies.push_back( 0 );
	m_Quad.m_vecIndicies.push_back( 1 );
	m_Quad.m_vecIndicies.push_back( 2 );

	// BOTTm_vecIndicies
	m_Quad.m_vecIndicies.push_back( 4 );
	m_Quad.m_vecIndicies.push_back( 6 );
	m_Quad.m_vecIndicies.push_back( 5 );

	m_Quad.m_vecIndicies.push_back( 4 );
	m_Quad.m_vecIndicies.push_back( 7 );
	m_Quad.m_vecIndicies.push_back( 6 );

	// FRONm_vecIndicies
	m_Quad.m_vecIndicies.push_back( 0 );
	m_Quad.m_vecIndicies.push_back( 7 );
	m_Quad.m_vecIndicies.push_back( 4 );

	m_Quad.m_vecIndicies.push_back( 0 );
	m_Quad.m_vecIndicies.push_back( 3 );
	m_Quad.m_vecIndicies.push_back( 7 );

	// RIGHm_vecIndicies
	m_Quad.m_vecIndicies.push_back( 3 );
	m_Quad.m_vecIndicies.push_back( 6 );
	m_Quad.m_vecIndicies.push_back( 7 );

	m_Quad.m_vecIndicies.push_back( 3 );
	m_Quad.m_vecIndicies.push_back( 2 );
	m_Quad.m_vecIndicies.push_back( 6 );

	// BACKm_vecIndicies
	m_Quad.m_vecIndicies.push_back( 2 );
	m_Quad.m_vecIndicies.push_back( 1 );
	m_Quad.m_vecIndicies.push_back( 5 );

	m_Quad.m_vecIndicies.push_back( 2 );
	m_Quad.m_vecIndicies.push_back( 5 );
	m_Quad.m_vecIndicies.push_back( 6 );

	// LEFTm_vecIndicies
	m_Quad.m_vecIndicies.push_back( 1 );
	m_Quad.m_vecIndicies.push_back( 4 );
	m_Quad.m_vecIndicies.push_back( 5 );

	m_Quad.m_vecIndicies.push_back( 1 );
	m_Quad.m_vecIndicies.push_back( 0 );
	m_Quad.m_vecIndicies.push_back( 4 );

	m_Quad.RebuildBuffers();

	m_Quad.SetPrimativeType( D3DPT_TRIANGLELIST );

}
