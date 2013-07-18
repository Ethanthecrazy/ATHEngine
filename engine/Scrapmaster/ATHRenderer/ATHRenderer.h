#ifndef ATHRENDERER_H
#define ATHRENDERER_H

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include <map>
#include <list>

#include "ATHMesh.h"
#include "ATHAtlas.h"
#include "ATHRenderpass.h"
#include "Camera.h"

#define NODE_LAYER_OFFSET (64.0f)

class CCamera;
class ATHRenderNode;

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
	
	IDirect3DVertexDeclaration9*	m_pvdPosNormUV;
	ID3DXEffect*					m_pEffect;
	CCamera*						m_pCamera;
	ATHMesh							m_meshQuad;

	std::list<ATHRenderNode*>		m_pNodeInventory;

	ATHRenderer( const ATHRenderer&);
	ATHRenderer& operator=(const ATHRenderer&);
	~ATHRenderer();

	void BuildQuad();

	// Node inventory management
	ATHRenderNode* CreateNode();
	void DestroyNode( ATHRenderNode* _toDestroy );
	void ClearInventory();

	//Renderpass management
	std::map< std::string, ATHRenderPass > m_mapRenderPasses;
	std::list< ATHRenderPass* > m_liSortedRenderPasses;

public:

	// Since the atlas is so entwined with the renderer, it is now a subset of the rederer.
	ATHAtlas						m_TextureAtlas;

	ATHRenderer();

	static		ATHRenderer* GetInstance();
	static void DeleteInstance();

	// Basic Functions
	bool		Initialize( HWND hWnd, HINSTANCE hInstance, unsigned int nScreenWidth, unsigned int nScreenHeight, bool bFullScreen, bool bVsync );
	void		Shutdown();
	inline		UINT GetFrameNumber(void){ return m_FrameCounter; }
	inline void IncrementFrameCounter(void){ ++m_FrameCounter; }

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

	// RenderPass Management
	ATHRenderPass*	CreateRenderPass( char* _szName, unsigned int _unPriority, RenderFunc _function );
	ATHRenderPass*	FindRenderPass( char* _szName );
	bool			DestroyRenderPass( char* _szName );
	void			ClearRenderPasses();

	void			OutputSuccess();

};

#endif