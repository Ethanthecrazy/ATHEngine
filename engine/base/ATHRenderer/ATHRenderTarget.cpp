#include "ATHRenderTarget.h"
#include "ATHRenderer.h"

ATHRenderTarget::ATHRenderTarget(void)
{
	m_dwTargetIndex = -1;
	m_pOldRenderSurface = nullptr;
	m_pRenderSurface = nullptr;
	m_pDevice = nullptr;
}
//================================================================================
ATHRenderTarget::~ATHRenderTarget(void)
{
	if( m_pRenderSurface )
	{
		m_pRenderSurface->Release();
		m_pRenderSurface = nullptr;
	}

	if( m_pOldRenderSurface )
	{
		m_pOldRenderSurface->Release();
		m_pOldRenderSurface = nullptr;
	}

	if( m_pTargetTexture )
	{
		m_pTargetTexture->Release();
		m_pTargetTexture = nullptr;
	}
}
//================================================================================
void ATHRenderTarget::Create( IDirect3DDevice9* _pDevice, UINT _width, UINT _height, D3DFORMAT _format )
{
	m_pDevice = _pDevice;
	D3DXCreateTexture( m_pDevice, _width, _height, 1, D3DUSAGE_RENDERTARGET, 
		_format, D3DPOOL_DEFAULT, &m_pTargetTexture );
	m_pTargetTexture->GetSurfaceLevel( 0, &m_pRenderSurface );
}
//================================================================================
void ATHRenderTarget::ActivateTarget( DWORD _targetIndex )
{
	m_dwTargetIndex = _targetIndex;
	m_pDevice->GetRenderTarget( _targetIndex, &m_pOldRenderSurface );
	m_pDevice->SetRenderTarget( m_dwTargetIndex, m_pRenderSurface );
}
//================================================================================
void ATHRenderTarget::RevertTarget(void)
{
	m_pDevice->SetRenderTarget( m_dwTargetIndex, m_pOldRenderSurface );
	if( m_pOldRenderSurface )
	{
		m_pOldRenderSurface->Release();
		m_pOldRenderSurface = nullptr;
	}
	m_dwTargetIndex = -1;
}
//================================================================================
void ATHRenderTarget::DeleteTarget(void)
{
	if( m_pRenderSurface )
	{
		m_pRenderSurface->Release();
		m_pRenderSurface = nullptr;
	}

	if( m_pOldRenderSurface )
	{
		m_pOldRenderSurface->Release();
		m_pOldRenderSurface = nullptr;
	}

	if( m_pTargetTexture )
	{
		m_pTargetTexture->Release();
		m_pTargetTexture = nullptr;
	}
}
//================================================================================