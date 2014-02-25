#ifndef ATHATLAS_H
#define ATHATLAS_H

#include <map>
#include <list>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>

#include "../../ATHUtil/hDataTypes.h"

// If the target pixel is null, then this function should clean up
typedef void(*PixelFunc)(float _fPosX, float _fPosY, float4* _pfTargetPixel, void* _pData);

struct sTexNode
{
	// TODO: Add references to these.
	std::string m_szName;
	LPDIRECT3DTEXTURE9 m_lpTexture;
	D3DSURFACE_DESC m_SurfDesc;

	sTexNode()
	{
		ZeroMemory(&m_SurfDesc, sizeof(m_SurfDesc));
		m_lpTexture = nullptr;
	}
};

class ATHObject;
struct ATHTexGenJob
{
	sTexNode*		m_pTexNode; // Texture node to add the texture to
	unsigned int	m_unTextureLevel;
	std::string		m_szTextureName;

	unsigned int	m_unTextureWidth;
	unsigned int	m_unTextureHeight;

	unsigned int	m_unCurrPosX;
	unsigned int	m_unCurrPosY;

	float4*			m_fPixels;
	PixelFunc		m_PixelFunc;
	void*			m_pData;
	unsigned int	m_unCurrPixel;
};

class ATHAtlas
{

private:

	LPDIRECT3DDEVICE9 m_pDevice;

	std::map< std::string, sTexNode* > m_mapTextures;
	sTexNode* m_pLastTex;

	std::list<ATHTexGenJob*> m_liTextureJobs;

public:

	class ATHTextureHandle
	{
	private:

		sTexNode* m_pTexNode;
		
		ATHTextureHandle( sTexNode* _target ) : m_pTexNode( _target ) {}

	public:

		ATHTextureHandle() : m_pTexNode( nullptr ) {}

		std::string GetName() { return m_pTexNode->m_szName; }
		float2 GetDimensions() { return float2( (float)m_pTexNode->m_SurfDesc.Width, (float)m_pTexNode->m_SurfDesc.Height ); }
		LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexNode->m_lpTexture; }
		bool Valid() { return m_pTexNode && true; }

		friend class ATHRenderer;
		friend class ATHAtlas;

	};

	ATHAtlas();

	void Initialize( LPDIRECT3DDEVICE9 _device );
	void Update();
	void Shutdown();

	ATHTextureHandle LoadTexture(char* _szHandle, char* _szFilepath);
	ATHTextureHandle LoadTextureFromData(const char* _szHandle, unsigned int _unWidth, unsigned int _unHeight, void* _pData, size_t _tDataSize);
	void UnloadTexture( LPDIRECT3DTEXTURE9 _texture );

	ATHTextureHandle GenerateTexture( const char* _szName, unsigned int _unWidth, unsigned int _unHeight, PixelFunc _PixelOperation, void* _pData = nullptr);

	ATHTextureHandle GetTexture( const char* _szHandle );
	float2 GetTexDimensions( char* _szHandle );

	void Clear();

};

#endif