#ifndef ATHRENDERER_H
#define ATHRENDERER_H

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include <map>
#include <list>

#include "ATHMesh.h"
#include "ATHRenderpass.h"
#include "Camera.h"

#define NODE_LAYER_OFFSET (64.0f)
#define SHADER_LOAD_PATH ".\\data\\shaders\\"
#define SHADER_SEARCH_EXTENSION ".fx"
#define TEXTURE_LOAD_PATH ".\\art\\textures\\"
#define	TEXTURE_SEARCH_EXTENSION ".png"
#define MESH_LOAD_PATH ".\\art\\meshes\\"

enum { ATH_VERTEXDECL_COLORED, ATH_VERTEXDECL_TEXTURED, ATH_VERTEXDECL_ANIMATED };

class CCamera;
class ATHRenderNode;
class ATHAtlas;
class ATHVertexDecl;

class ATHRenderer
{

private:

	static ATHRenderer* m_pInstance;

	unsigned int					m_FrameCounter;		// Frame Counter
	unsigned int					m_unScreenWidth;
	unsigned int					m_unScreenHeight;

	HWND							m_hWnd;				// Windows handle.
	HINSTANCE						m_hInstance;

	LPDIRECT3D9						m_pD3D;			// The Direct3d Object
	IDirect3DDevice9*				m_pDevice;		// The Device
	D3DPRESENT_PARAMETERS			m_PresentParams;	// Present Parameters

	std::map< unsigned int, ATHVertexDecl* >	m_mapVertDecls;
	std::map< std::string, ID3DXEffect* >		m_mapEffects;

	IDirect3DVertexDeclaration9*			m_pvdPosNormUV;
	CCamera*								m_pCamera;

	std::list<ATHRenderNode*>				m_pNodeInventory;
	std::list<ATHRenderNode*>				m_liNodeTotalList;

	ATHRenderer( const ATHRenderer&);
	ATHRenderer& operator=(const ATHRenderer&);
	~ATHRenderer();

	// Node inventory management
	ATHRenderNode* CreateNode();
	void DestroyNode( ATHRenderNode* _toDestroy );
	void DestoryAllNodes();

	//Renderpass management
	std::map< std::string, ATHRenderPass > m_mapRenderPasses;
	std::list< ATHRenderPass* > m_liSortedRenderPasses;

	// Since the atlas is so entwined with the renderer, it is now a subset of the rederer.
	ATHAtlas*	m_pTextureAtlas;

	ATHMesh		m_Quad;

	ATHRenderer();

public:



	static		ATHRenderer* GetInstance();
	static void DeleteInstance();

	// Basic Functions
	bool		Initialize( HWND hWnd, HINSTANCE hInstance, unsigned int nScreenWidth, unsigned int nScreenHeight, bool bFullScreen, bool bVsync );
	void		InitVertexDecls();

	void		Shutdown();
	inline		UINT GetFrameNumber(void){ return m_FrameCounter; }
	inline void IncrementFrameCounter(void){ ++m_FrameCounter; }
	ATHAtlas*	GetAtlas() { return m_pTextureAtlas; }

	// Graphics Management
	
	inline	LPDIRECT3DDEVICE9 GetDevice() { return m_pDevice; }
	inline	CCamera* GetCamera() { return m_pCamera; }
	void	CommitDraws();
	void	RasterTexture( LPDIRECT3DTEXTURE9 _texture, float _left = 0.0f, float _top = 0.0f, float _right = 1.0f, float _bottom = 1.0f );
	void	DRXClear( float3 _color );
	void	DRXBegin();
	void	DRXEnd();
	void	DRXPresent();
	void	ChangeDisplayParam( int nScreenWidth, int nScreenHeight, bool bFullScreen, bool bVsync );
	void	ResetDevice(void);

	void	LoadTextures( char* _path );

	// VertexDecl Management
	ATHVertexDecl*	GetVertexDeclaration( unsigned int _unHandle );
	
	// Shader Management
	void			LoadShaders( char* _path );
	void			UnloadShaders();
	ID3DXEffect*	GetShader( char* _szName );

	// RenderPass Management
	ATHRenderPass*	CreateRenderPass( char* _szName, unsigned int _unPriority, RenderFunc _function,  char* _szShaderName, char* _szTechnique = "Default" );
	ATHRenderPass*	FindRenderPass( char* _szName );
	bool			DestroyRenderPass( char* _szName );
	void			ClearRenderPasses();

	// RenderNode Management
	ATHRenderNode* CreateRenderNode( char* _szPassName, unsigned int _unPriority );
	void DestoryRenderNode( ATHRenderNode* _pDestroy );

	// Utility
	void DrawMesh( ATHMesh* _pMesh );

	ATHMesh* GetQuad();

private:

	void BuildQuad();
};

#endif