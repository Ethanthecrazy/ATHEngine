#include "ATHAtlas.h"
#include "ATHRenderer.h"

ATHAtlas* ATHAtlas::m_pInstance = nullptr;

ATHAtlas* ATHAtlas::GetInstance()
{
	if( !m_pInstance )
		m_pInstance = new ATHAtlas();
	return m_pInstance;
}

void ATHAtlas::DeleteInstance()
{
	if( m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

ATHAtlas::ATHAtlas()
{
	m_pDevice = nullptr;
	m_pLastTex = nullptr;
}

ATHAtlas::~ATHAtlas()
{

}

void ATHAtlas::Initialize()
{
	m_pDevice = ATHRenderer::GetInstance()->GetDevice();
}

void ATHAtlas::Shutdown()
{
	Clear();
}

void ATHAtlas::LoadTexture( char* _szHandle, char* _szFilepath )
{
	if( strlen( _szHandle ) < 3 || strlen( _szFilepath ) < 3 )
		return;

	std::string szHandleString = _szHandle;

	if( m_mapTextures.count( szHandleString ) )
		return;

	sTexNode* pNewTex = new sTexNode();

	HRESULT result = 0;
	result = D3DXCreateTextureFromFileExA( m_pDevice,
		(LPCSTR)(_szFilepath),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_FROM_FILE,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_UNKNOWN,
		D3DPOOL_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_ARGB( 0, 0, 0, 0 ),
		NULL,
		NULL,
		&pNewTex->m_lpTexture );
	
	if( FAILED( result ) )
	{
		delete pNewTex;
		return;
	}
	else
	{
		pNewTex->m_szName = szHandleString;

		D3DSURFACE_DESC d3dSurfDesc;

		ZeroMemory( &d3dSurfDesc, sizeof(d3dSurfDesc) );
		pNewTex->m_lpTexture->GetLevelDesc( 0, &d3dSurfDesc );

		pNewTex->m_uHeight	= d3dSurfDesc.Height;
		pNewTex->m_uWidth	= d3dSurfDesc.Width;

		m_mapTextures[ szHandleString ] = pNewTex;
	}

}

void ATHAtlas::UnloadTexture( LPDIRECT3DTEXTURE9 _texture )
{
	std::map< std::string, sTexNode* >::iterator itrCurr = m_mapTextures.begin();

	while( itrCurr != m_mapTextures.end() )
	{
		if( (*itrCurr).second->m_lpTexture == _texture )
		{
			(*itrCurr).second->m_lpTexture->Release();
			delete (*itrCurr).second;
			m_mapTextures.erase( itrCurr );
			return;
		}

		itrCurr++;
	}
}

ATHAtlas::ATHTextureHandle ATHAtlas::GetTexture( char* _szHandle )
{
	std::string szHandleString = _szHandle;

	if( m_mapTextures.count( szHandleString ) )
	{
		return ATHTextureHandle( m_mapTextures[ szHandleString ] );
	}

	return nullptr;
}

float2 ATHAtlas::GetTexDimensions( char* _szHandle )
{
	float2 toReturn;

	std::string szHandleString = _szHandle;

	if( m_mapTextures.count( szHandleString ) )
	{
		toReturn.vX = (float)m_mapTextures[ szHandleString ]->m_uWidth;
		toReturn.vY = (float)m_mapTextures[ szHandleString ]->m_uHeight;
	}

	return toReturn;
}

void ATHAtlas::Clear()
{
	std::map< std::string, sTexNode* >::iterator itrCurr = m_mapTextures.begin();

	while( itrCurr != m_mapTextures.end() )
	{
		(*itrCurr).second->m_lpTexture->Release();
		delete (*itrCurr).second;
		
		itrCurr++;
	}

	m_mapTextures.clear();
}