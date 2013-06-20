#ifndef ATHRENDERNODE_H
#define ATHRENDERNODE_H

#include <d3d9.h>
#include <d3dx9.h>

#include "../ATHUtil/hDataTypes.h"
#include "ATHAtlas.h"

class ATHRenderNode
{

private:

	ATHAtlas::ATHTextureHandle	m_pTexture;
	D3DXMATRIX					m_matTransform;
	float4						m_fColor;
	float4						m_fCropping;

	ATHRenderNode*				m_pNext;

public:

	ATHRenderNode();
	~ATHRenderNode();

	void SetImage( ATHAtlas::ATHTextureHandle _texture ) { m_pTexture = _texture; }
	void SetTransform( D3DXMATRIX _transform ){ m_matTransform = _transform; }
	void SetColor( float4 _color ) { m_fColor = _color; }
	void SetCropping( float4 rect ) { m_fCropping = rect; }

	friend class ATHRenderer;
};

#endif