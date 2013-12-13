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
#include "ATHRenderFunctions.h"


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
ATHRenderer::ATHRenderer() : m_meshDebugLines( "DebugLines", GetVertexDeclaration( ATH_VERTEXDECL_COLORED ), D3DPT_LINELIST ), m_Quad( "", nullptr, D3DPT_POINTLIST )
{
	m_FrameCounter		= 0;		// Frame Counter
	m_unScreenWidth		= 0;
	m_unScreenHeight	= 0;
	m_fScreenDepth		= 0;

	m_hWnd;					// Windows handle.
	m_hInstance;

	m_pD3D				= nullptr;		// The Direct3d Object
	m_pDevice			= nullptr;		// The Device

	ZeroMemory( &m_PresentParams, sizeof( D3DPRESENT_PARAMETERS ) );

	m_pCamera			= nullptr;

	m_pNodeInventory	= std::list<ATHRenderNode*>();

	m_d3deffDepth = nullptr;

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

	m_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	InitVertexDecls();

	m_pTextureAtlas = new ATHAtlas();
	m_pTextureAtlas->Initialize( m_pDevice );

	LoadTextures( TEXTURE_LOAD_PATH );

	LoadShaders( SHADER_LOAD_PATH );

	InitStandardRendering();

	return true;
}

void ATHRenderer::InitVertexDecls()
{
	ATHVertexDecl* pNewDecl = nullptr;

	// Colored dcl
	pNewDecl = new ATHVertexDecl();
	pNewDecl->AddVertexElement( D3DDECLUSAGE_POSITION );
	pNewDecl->AddVertexElement( D3DDECLUSAGE_COLOR );
	pNewDecl->BuildDecl();
	m_mapVertDecls.insert( std::pair< unsigned int, ATHVertexDecl* >( ATH_VERTEXDECL_COLORED, pNewDecl ) );

	// Textured decl
	pNewDecl = new ATHVertexDecl();
	pNewDecl->AddVertexElement( D3DDECLUSAGE_POSITION );
	pNewDecl->AddVertexElement( D3DDECLUSAGE_TEXCOORD );
	pNewDecl->BuildDecl();
	m_mapVertDecls.insert( std::pair< unsigned int, ATHVertexDecl* >( ATH_VERTEXDECL_TEXTURED, pNewDecl ) );

}
//================================================================================
void ATHRenderer::InitStandardRendering()
{

	// Initialize the standard depth shader
	ID3DXEffect* d3deffDepth = GetShader( SHADER_DEPTH_NAME );
	if( d3deffDepth )
		m_d3deffDepth = d3deffDepth;
	else
		std::cout << "[ERROR] ATHRenderer::Initialize() - Could not locate default depth shader " << SHADER_DEPTH_NAME << "\n"; 

	m_rtDepth.Create( m_pDevice, m_unScreenWidth, m_unScreenHeight, D3DFMT_R32F );

	m_fScreenDepth = 100.0f;
	m_pCamera = new CCamera();
	//m_pCamera->BuildPerspective(D3DX_PI / 2.0f, ((float)(m_unScreenWidth))/m_unScreenHeight, 0.1f, m_fScreenDepth );

	m_pCamera->BuildOrthoPerspective( 20.0f, ((float)(m_unScreenWidth))/m_unScreenHeight, 0.01f, m_fScreenDepth );
	m_pCamera->SetViewPosition(0.0f, 0.0f, -5.0f);

	//Default texture rendering
	CreateRenderPass( "Texture", 1, ATHRenderFuncTexture, "texture", true );

	//Setup the debug line rendering
	m_meshDebugLines.SetVertexDecl( GetVertexDeclaration( ATH_VERTEXDECL_COLORED ) );

	CreateRenderPass( "debugline", 0, ATHRenderFuncDebugLines, "coloredline" );
	ATHRenderNode* pNode =  CreateRenderNode( "debugline", 0 );
	pNode->SetMesh( &m_meshDebugLines );
	
	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat );
	pNode->SetTransform( mat );

	BuildQuad();

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
void ATHRenderer::RenderDepth()
{
	// Build the list of depth buffer nodes
	std::list< ATHRenderNode* > liNodes;

	std::map< std::string, ATHRenderPass >::iterator itrPass = m_mapRenderPasses.begin();
	while( itrPass != m_mapRenderPasses.end() )
	{
		if( (*itrPass).second.GetRenderToDepth() )
		{
			const std::list<ATHRenderNode*> liCurrNodes = (*itrPass).second.GetNodeList();

			std::list< ATHRenderNode* >::const_iterator itrNode = liCurrNodes.cbegin();
			while( itrNode != liCurrNodes.cend() )
			{
				liNodes.push_back( (*itrNode) );
				++itrNode;
			}
		}
		++itrPass;
	}

	m_rtDepth.ActivateTarget( 0 );
	DRXClear( float3( 1.0f ) );

	m_d3deffDepth->SetTechnique("DepthPass");
	m_d3deffDepth->SetFloat( "fFrustrumLength", m_fScreenDepth );
	unsigned int passes(0);
	m_d3deffDepth->Begin( &passes, 0 );
	for( unsigned int pass = 0; pass < passes; ++pass )
	{
		m_d3deffDepth->BeginPass( pass );
		{

			std::list< ATHRenderNode* >::iterator itrRenderNode = liNodes.begin();
			while( itrRenderNode != liNodes.end() )
			{

				D3DXMATRIX matMVP = GetCamera()->GetViewMatrix() * GetCamera()->GetProjectionMatrix();
				m_d3deffDepth->SetMatrix( "gWVP", &( (*itrRenderNode)->GetTrasform() * matMVP ) );

				m_d3deffDepth->CommitChanges();

				DrawMesh( (*itrRenderNode)->GetMesh() );
				++itrRenderNode;
			}
		}
		m_d3deffDepth->EndPass();
	}

	m_rtDepth.RevertTarget();
}
//================================================================================
void ATHRenderer::RenderForward()
{
	DRXClear( float3( 1.0f, 1.0f, 1.0f ) );
	std::list< ATHRenderPass* >::iterator itrPass = m_liSortedRenderPasses.begin();
	while( itrPass != m_liSortedRenderPasses.end() )
	{
		(*itrPass)->PreExecute();
		(*itrPass)->Execute( this );
		(*itrPass)->PostExecute();
		itrPass++;
	}
}
//================================================================================
void ATHRenderer::CommitDraws()
{
	RenderDepth();
	RenderForward();
	DebugLinesCleanup();
}
//================================================================================
// TODO: REIMPLEMENT RASTER TEXTURE
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
	//Clear the current render target and the Z-Buffer
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
			if( strcmp( szFileExtension.c_str(), TEXTURE_SEARCH_EXTENSION ) == 0 || strcmp( szFileExtension.c_str(), ".jpg" ) == 0 )
			{
				//Generate the filepath to the image
				std::string pathToFile = std::string( _path );
				pathToFile += szFilename;

				// Remove the leading ".\"
				unsigned int unStartPos = pathToFile.find_first_of( "\\" );
				pathToFile = pathToFile.substr( unStartPos + 1 );

				//Generate the name of the texture
				std::string szName = szFilename.substr( 0, unExtenPos);

				GetAtlas()->LoadTexture( (char*)pathToFile.c_str(), (char*)pathToFile.c_str() );
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
//================================================================================
ID3DXEffect* ATHRenderer::GetShader( char* _szName )
{
	std::string strName = _szName;
	ID3DXEffect* pToReturn = nullptr;

	if( m_mapEffects.count( strName ) > 0 )
		pToReturn = m_mapEffects.at( strName );

	return pToReturn;
}

//================================================================================
ATHRenderPass*	ATHRenderer::CreateRenderPass( char* _szName, unsigned int _unPriority, RenderFunc _function,  char* _szShaderName, bool _bRenderToDepth, char* _szTechnique )
{
	ATHRenderPass* pToReturn = nullptr;
	std::string	idString = std::string( _szName );

	if( m_mapRenderPasses.count( idString ) == 0 )
	{
		// Get the address of the pass being constructed and inserted into the map
		ATHRenderPass* pNewPass = &( m_mapRenderPasses[ idString ] = ATHRenderPass( _szName, _unPriority, GetShader( _szShaderName ), _function, _bRenderToDepth, _szTechnique ) );
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
void ATHRenderer::DebugLinesAdd( float3 _fStart, float3 _fEnd, float4 _fColor )
{
	m_meshDebugLines.m_vecPositions.push_back( _fStart );
	m_meshDebugLines.m_vecPositions.push_back( _fEnd );
	m_meshDebugLines.m_vecColors.push_back( _fColor );
	m_meshDebugLines.m_vecColors.push_back( _fColor );
	m_meshDebugLines.m_vecIndicies.push_back( m_meshDebugLines.m_vecIndicies.size() );
	m_meshDebugLines.m_vecIndicies.push_back( m_meshDebugLines.m_vecIndicies.size() );
}
//================================================================================
void ATHRenderer::DebugLinesCleanup()
{
	m_meshDebugLines.Clear();
}
//================================================================================
void ATHRenderer::BuildQuad()
{
	m_Quad = ATHMesh( "Quad", GetVertexDeclaration( ATH_VERTEXDECL_TEXTURED ), D3DPT_TRIANGLELIST );

	m_Quad.m_vecPositions.push_back( float3( -0.5f, 0.5f, 0.0f ) );
	m_Quad.m_vecPositions.push_back( float3( 0.5f, 0.5f, 0.0f ) );
	m_Quad.m_vecPositions.push_back( float3( 0.5f, -0.5f, 0.0f ) );
	m_Quad.m_vecPositions.push_back( float3( -0.5f, -0.5f, 0.0f ) );

	m_Quad.m_vecUVs.push_back( float2( 0.0f, 0.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 1.0f, 0.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 1.0f, 1.0f ) );
	m_Quad.m_vecUVs.push_back( float2( 0.0f, 1.0f ) );

	m_Quad.m_vecIndicies.push_back( 0 );
	m_Quad.m_vecIndicies.push_back( 1 );
	m_Quad.m_vecIndicies.push_back( 2 );

	m_Quad.m_vecIndicies.push_back( 0 );
	m_Quad.m_vecIndicies.push_back( 2 );
	m_Quad.m_vecIndicies.push_back( 3 );

	m_Quad.RebuildBuffers();

	m_Quad.SetPrimativeType( D3DPT_TRIANGLELIST );

}
//================================================================================
