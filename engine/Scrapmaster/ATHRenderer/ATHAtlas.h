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

	static ATHAtlas* m_pInstance;

	ATHAtlas();
	ATHAtlas( const ATHAtlas&);
	ATHAtlas& operator=(const ATHAtlas&);
	~ATHAtlas();

	struct sTexNode
	{

		// TODO: Add references to these.
		std::string m_szName;
		unsigned int m_uWidth;
		unsigned int m_uHeight;

		LPDIRECT3DTEXTURE9 m_lpTexture;

		sTexNode()
		{
			m_uWidth		= 0;
			m_uHeight		= 0;
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
		float2 GetDimensions() { return float2( (float)m_pTexNode->m_uWidth, (float)m_pTexNode->m_uHeight ); }
		LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexNode->m_lpTexture; }

		friend class ATHRenderer;
		friend class ATHAtlas;

	};

	static ATHAtlas* GetInstance();
	static void DeleteInstance();

	void Initialize(); 
	void Shutdown();

	void LoadTexture( char* _szHandle, char* _szFilepath );
	void UnloadTexture( LPDIRECT3DTEXTURE9 _texture );

	ATHTextureHandle GetTexture( char* _szHandle );
	float2 GetTexDimensions( char* _szHandle );

	void Clear();

};

#endif