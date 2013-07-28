#ifndef ATHATLAS_H
#define ATHATLAS_H

#include <map>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>

#include "../ATHUtil/hDataTypes.h"

class ATHAtlas
{

private:

	struct sTexNode
	{

		// TODO: Add references to these.
		std::string m_szName;
		LPDIRECT3DTEXTURE9 m_lpTexture;
		D3DSURFACE_DESC m_SurfDesc;

		sTexNode()
		{
			ZeroMemory( &m_SurfDesc, sizeof(m_SurfDesc) );
			m_lpTexture		= nullptr;
		}
	};

	LPDIRECT3DDEVICE9 m_pDevice;

	std::map< std::string, sTexNode* > m_mapTextures;

	sTexNode* m_pLastTex;

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

		friend class ATHRenderer;
		friend class ATHAtlas;

	};

	ATHAtlas();

	void Initialize( LPDIRECT3DDEVICE9 _device ); 
	void Shutdown();

	void LoadTexture( char* _szHandle, char* _szFilepath );
	void UnloadTexture( LPDIRECT3DTEXTURE9 _texture );

	ATHTextureHandle GetTexture( char* _szHandle );
	float2 GetTexDimensions( char* _szHandle );

	void Clear();

};

#endif