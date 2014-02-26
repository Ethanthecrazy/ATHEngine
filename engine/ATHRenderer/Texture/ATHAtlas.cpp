#include "ATHAtlas.h"
#include "../ATHRenderer.h"

#include <ppl.h>

#include "../../ATHObjectSystem/ATHObject.h"

#define MAX_PIXELS_PER_UPDATE 10000
#define MAX_FRAMERATE 120
#define MAX_FRAMERATE_TIME ( 1.0f / MAX_FRAMERATE )
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
void ATHAtlas::Update( float _fDT )
{
	if (m_liTextureJobs.size() < 1)
		return;

	ATHTexGenJob* pCurrJob = m_liTextureJobs.front();

	unsigned int unNumPixels = pCurrJob->m_unTextureWidth * pCurrJob->m_unTextureHeight;
	float fPixelWidth = 1.0f / pCurrJob->m_unTextureWidth;
	float fPixelHeight = 1.0f / pCurrJob->m_unTextureHeight;
	
	unsigned int unPixelsThisLoop = MAX_PIXELS_PER_UPDATE;
	if (pCurrJob->m_unCurrPixel + unPixelsThisLoop >= unNumPixels)
	{
		unPixelsThisLoop -= pCurrJob->m_unCurrPixel + unPixelsThisLoop - unNumPixels;
	}

	Concurrency::parallel_for(pCurrJob->m_unCurrPixel, pCurrJob->m_unCurrPixel + unPixelsThisLoop, [&](unsigned int i)
	{
		unsigned int unPosX = i % pCurrJob->m_unTextureWidth;
		unsigned int unPosY = i / pCurrJob->m_unTextureWidth;

		pCurrJob->m_PixelFunc(unPosX / (float)pCurrJob->m_unTextureWidth + fPixelWidth / 2.0f,
			unPosY / (float)pCurrJob->m_unTextureHeight + fPixelHeight / 2.0f,
			pCurrJob->m_fPixels + i, pCurrJob->m_pData);

	});

	pCurrJob->m_unCurrPixel += unPixelsThisLoop;

	if (pCurrJob->m_unCurrPixel >= unNumPixels)
	{
		bool bExists = false;
		// Check to make sure that target node still exists
		std::map< std::string, sTexNode* >::iterator itrCurr = m_mapTextures.begin();
		while (itrCurr != m_mapTextures.end())
		{
			if ((*itrCurr).second == pCurrJob->m_pTexNode)
			{
				bExists = true;
				break;
			}
			itrCurr++;
		}

		if (!bExists) // If it doesnt exist
		{
			// End it now
			delete pCurrJob->m_fPixels;
			pCurrJob->m_PixelFunc(0, 0, nullptr, pCurrJob->m_pData);
			delete pCurrJob;
			m_liTextureJobs.pop_front();
			return;
		}

		ATHAtlas::ATHTextureHandle athTExHandle = LoadTextureFromData("ATH_TEXTURE_GENERATION",
			pCurrJob->m_unTextureWidth,
			pCurrJob->m_unTextureWidth,
			(void*)pCurrJob->m_fPixels,
			sizeof(float4)* unNumPixels);

		delete pCurrJob->m_fPixels;

		// Get the info from the created tex node and destroy it
		// When creating the job, a texture node was created empty 
		// and a reference to it was given out, so we need to move
		// the information from one node to another

		// Give the waiting tex node its texture
		pCurrJob->m_pTexNode->m_lpTexture = athTExHandle.m_pTexNode->m_lpTexture;
		pCurrJob->m_pTexNode->m_SurfDesc = athTExHandle.m_pTexNode->m_SurfDesc;
		pCurrJob->m_pTexNode->m_szName = pCurrJob->m_szTextureName;

		// clean up the precreated node
		itrCurr = m_mapTextures.begin();
		while (itrCurr != m_mapTextures.end())
		{
			if ((*itrCurr).second->m_lpTexture == athTExHandle.m_pTexNode->m_lpTexture)
			{
				delete (*itrCurr).second;
				m_mapTextures.erase(itrCurr);
				break;
			}
			itrCurr++;
		}


		pCurrJob->m_PixelFunc(0, 0, nullptr, pCurrJob->m_pData);
		delete pCurrJob;
		m_liTextureJobs.pop_front();

		return;
	}

}
//================================================================================
void ATHAtlas::Shutdown()
{
	Clear();
}
//================================================================================
ATHAtlas::ATHTextureHandle ATHAtlas::LoadTexture(char* _szHandle, char* _szFilepath)
{
	if( strlen( _szHandle ) < 3 || strlen( _szFilepath ) < 4 )
		return ATHTextureHandle();

	std::string szHandleString = _szHandle;

	if( m_mapTextures.count( szHandleString ) > 0 )
		return ATHTextureHandle(m_mapTextures[szHandleString]);

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
		return ATHTextureHandle();
	}

	pNewTex->m_szName = szHandleString;

	ZeroMemory( &pNewTex->m_SurfDesc, sizeof(pNewTex->m_SurfDesc) );
	pNewTex->m_lpTexture->GetLevelDesc( 0, &pNewTex->m_SurfDesc );

	m_mapTextures[ szHandleString ] = pNewTex;

	return ATHTextureHandle(pNewTex);

}
//================================================================================
ATHAtlas::ATHTextureHandle ATHAtlas::LoadTextureFromData(const char* _szHandle, unsigned int _unWidth, unsigned int _unHeight, void* _pData, size_t _tDataSize)
{
	if (_pData == nullptr || _tDataSize < 1)
		return ATHTextureHandle();
	
	std::string szHandleString = _szHandle;

	if (m_mapTextures.count(szHandleString) > 0)
		return ATHTextureHandle(m_mapTextures[szHandleString]);

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
		return ATHTextureHandle();
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
		return ATHTextureHandle();
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

	return ATHTextureHandle(pNewTex);

}
//================================================================================
void ATHAtlas::UnloadTexture(ATHTextureHandle _pNode)
{
	std::map< std::string, sTexNode* >::iterator itrCurr = m_mapTextures.begin();

	while( itrCurr != m_mapTextures.end() )
	{
		if ((*itrCurr).second == _pNode.m_pTexNode )
		{
			IF((*itrCurr).second->m_lpTexture)->Release();
			delete (*itrCurr).second;
			m_mapTextures.erase( itrCurr );
			return;
		}

		itrCurr++;
	}
}
//================================================================================
ATHAtlas::ATHTextureHandle ATHAtlas::GenerateTexture(const char* _szName, unsigned int _unWidth, unsigned int _unHeight, PixelFunc _PixelOperation, void* _pData )
{
	if (_unWidth == 0 || _unHeight == 0)
		return ATHTextureHandle();

	std::string szHandleString( _szName );
	sTexNode* pEmptyNode = new sTexNode();

	m_mapTextures[szHandleString] = pEmptyNode;

	ATHTexGenJob* pNewJob = new ATHTexGenJob();
	pNewJob->m_pTexNode = pEmptyNode;
	pNewJob->m_unTextureLevel = 0;
	pNewJob->m_szTextureName = szHandleString;

	pNewJob->m_unTextureWidth = _unWidth;
	pNewJob->m_unTextureHeight = _unHeight;

	pNewJob->m_unCurrPosX = 0;
	pNewJob->m_unCurrPosY = 0;

	pNewJob->m_fPixels = new float4[_unHeight * _unWidth];
	pNewJob->m_PixelFunc = _PixelOperation;
	pNewJob->m_pData = _pData;
	pNewJob->m_unCurrPixel = 0;

	m_liTextureJobs.push_back(pNewJob);

	return ATHTextureHandle(pEmptyNode);
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
		IF((*itrCurr).second->m_lpTexture)->Release();
		delete (*itrCurr).second;
		
		itrCurr++;
	}

	m_mapTextures.clear();

	std::list< ATHTexGenJob* >::iterator itrJob = m_liTextureJobs.begin();
	while (itrJob != m_liTextureJobs.end())
	{
		delete (*itrJob)->m_fPixels;
		(*itrJob)->m_PixelFunc(0, 0, nullptr, (*itrJob)->m_pData);
		delete (*itrJob);
		itrJob++;
	}

	m_liTextureJobs.clear();
}
//================================================================================