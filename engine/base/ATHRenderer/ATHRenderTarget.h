#ifndef ATHRENDERTARGET_H_
#define ATHRENDERTARGET_H_

#include <d3dx9.h>

class ATHRenderTarget
{
private:

	LPDIRECT3DTEXTURE9	m_pTargetTexture;
	LPDIRECT3DSURFACE9	m_pRenderSurface;
	LPDIRECT3DSURFACE9	m_pOldRenderSurface;
	IDirect3DDevice9*	m_pDevice;
	DWORD m_dwTargetIndex;

public:
	ATHRenderTarget(void);
	~ATHRenderTarget(void);

	void Create( IDirect3DDevice9* _pDevice, UINT _width, UINT _height, D3DFORMAT _format = D3DFMT_A2R10G10B10 );

	void ActivateTarget( DWORD _targetIndex );

	void RevertTarget(void);

	LPDIRECT3DTEXTURE9 GetTexture() { return m_pTargetTexture; }

	void DeleteTarget(void);

};

#endif //ATHRenderTarget_H_