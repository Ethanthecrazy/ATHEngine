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

	ATHRenderNode*					m_pNodeInventory;

	std::map< int, ATHRenderNode* > m_mapRenderLists;
	
	ATHAtlas						m_TextureAtlas;

	ATHRenderer( const ATHRenderer&);
	ATHRenderer& operator=(const ATHRenderer&);
	~ATHRenderer();

	void BuildQuad();
	void ProcessNode( ATHRenderNode* _target );

	ATHRenderNode* CreateNode();
	void DestroyNode( ATHRenderNode* _toDestroy );
	void ClearInventory();

public:

	ATHRenderer();

	static ATHRenderer* GetInstance();
	static void DeleteInstance();

	bool Initialize( HWND hWnd, HINSTANCE hInstance, unsigned int nScreenWidth, unsigned int nScreenHeight, bool bFullScreen, bool bVsync );

	inline UINT GetFrameNumber(void){ return m_FrameCounter; }
	inline void IncrementFrameCounter(void){ ++m_FrameCounter; }

	inline LPDIRECT3DDEVICE9 GetDevice() { return m_pDevice; }
	inline CCamera* GetCamera() { return m_pCamera; }

	void CommitDraws();

	void RasterTexture( LPDIRECT3DTEXTURE9 _texture, float _left = 0.0f, float _top = 0.0f, float _right = 1.0f, float _bottom = 1.0f );

	void DRXClear( float3 _color );
	void DRXBegin();
	void DRXEnd();
	void DRXPresent();

	void Shutdown();

	void ChangeDisplayParam( int nScreenWidth, int nScreenHeight, bool bFullScreen, bool bVsync );

	void ResetDevice(void);

};

#endif