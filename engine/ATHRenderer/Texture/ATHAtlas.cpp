#include "ATHAtlas.h"
#include "../ATHRenderer.h"

#include "../../ATHUtil/NewInclude.h"

ATHAtlas::ATHAtlas()
{
	m_pDevice = nullptr;
	m_pLastTex = nullptr;
}
//================================================================================
void ATHAtlas::Initialize( LPDIRECT3DDEVICE9 _device )
{
	m_pDevice = _device;
}
//================================================================================
void ATHAtlas::Shutdown()
{
	Clear();
}
//================================================================================
bool ATHAtlas::LoadTexture( char* _szHandle, char* _szFilepath )
{
	if( strlen( _szHandle ) < 3 || strlen( _szFilepath ) < 4 )
		return false;

	std::string szHandleString = _szHandle;

	if( m_mapTextures.count( szHandleString ) > 0 )
		return false;

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
		return false;
	}

	pNewTex->m_szName = szHandleString;

	ZeroMemory( &pNewTex->m_SurfDesc, sizeof(pNewTex->m_SurfDesc) );
	pNewTex->m_lpTexture->GetLevelDesc( 0, &pNewTex->m_SurfDesc );

	m_mapTextures[ szHandleString ] = pNewTex;

	return true;

}
//================================================================================
bool ATHAtlas::LoadTextureFromData(const char* _szHandle, unsigned int _unWidth, unsigned int _unHeight, void* _pData, size_t _tDataSize)
{
	if (_pData == nullptr || _tDataSize < 1)
		return false;
	
	std::string szHandleString = _szHandle;

	if (m_mapTextures.count(szHandleString))
		return false;

	HRESULT result = 0;
	// Create the texture in system memory
	LPDIRECT3DTEXTURE9 lpSourceTexture;

	result = D3DXCreateTexture(m_pDevice,
		_unWidth,
		_unHeight,
		D3DX_FROM_FILE,
		D3DUSAGE_DYNAMIC,
		D3DFMT_A32B32G32R32F,
		D3DPOOL_SYSTEMMEM,
		&lpSourceTexture);

	if (FAILED(result))
	{
		return false;
	}

	// Set the information in the texture
	D3DLOCKED_RECT rectLocked;
	result = lpSourceTexture->LockRect(0, &rectLocked, nullptr, 0);

	void* pPixels = rectLocked.pBits;
	memcpy(pPixels, _pData, _tDataSize );

	result = lpSourceTexture->UnlockRect(0);
	lpSourceTexture->GenerateMipSubLevels();

	// Create the texture in graphics memory
	LPDIRECT3DTEXTURE9 lpTargetTexture;

	result = D3DXCreateTexture(m_pDevice,
		_unWidth,
		_unHeight,
		D3DX_FROM_FILE,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A32B32G32R32F,
		D3DPOOL_DEFAULT,
		&lpTargetTexture);

	if (FAILED(result))
	{
		return false;
	}

	// Update the graphics texture with the system texture
	result = m_pDevice->UpdateTexture(lpSourceTexture, lpTargetTexture);
	lpTargetTexture->SetAutoGenFilterType(D3DTEXF_LINEAR);

	// clean up system texture
	lpSourceTexture->Release();

	sTexNode* pNewTex = new sTexNode();
	pNewTex->m_lpTexture = lpTargetTexture;
	pNewTex->m_szName = szHandleString;

	ZeroMemory(&pNewTex->m_SurfDesc, sizeof(pNewTex->m_SurfDesc));
	pNewTex->m_lpTexture->GetLevelDesc(0, &pNewTex->m_SurfDesc);

	m_mapTextures[szHandleString] = pNewTex;

	return true;

}
//================================================================================
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
//================================================================================
ATHAtlas::ATHTextureHandle ATHAtlas::GetTexture( const char* _szHandle )
{
	std::string szHandleString = _szHandle;

	if( m_mapTextures.count( szHandleString ) )
	{
		return ATHTextureHandle( m_mapTextures[ szHandleString ] );
	}

	return nullptr;
}
//================================================================================
float2 ATHAtlas::GetTexDimensions( char* _szHandle )
{
	float2 toReturn;

	std::string szHandleString = _szHandle;

	if( m_mapTextures.count( szHandleString ) )
	{
		toReturn.vX = (float)m_mapTextures[ szHandleString ]->m_SurfDesc.Width;
		toReturn.vY = (float)m_mapTextures[ szHandleString ]->m_SurfDesc.Height;
	}

	return toReturn;
}
//================================================================================
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
//================================================================================