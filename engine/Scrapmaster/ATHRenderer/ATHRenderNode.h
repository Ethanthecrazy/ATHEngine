#ifndef ATHRENDERNODE_H
#define ATHRENDERNODE_H

#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <string>

#include "../ATHUtil/hDataTypes.h"
#include "ATHAtlas.h"
#include "ATHMesh.h"

// Forward declare the render node so that the function pointer can reference it
// Then the node can contain a function pointer that references its own class.
class ATHRenderNode;

const int RENDERNODE_TEXTURE_COUNT = 3;

//Function pointer
typedef void (*RenderFunc)(ATHRenderNode* node);

class ATHRenderNode
{

private:

	ATHAtlas::ATHTextureHandle	m_pTexture[RENDERNODE_TEXTURE_COUNT];

	RenderFunc					m_fpRenderFunction;
	D3DXMATRIX					m_matTransform;

	ATHRenderNode*				m_pNext;

	// Nothing except the ATHRenderer is allowed to destroy these.
	~ATHRenderNode();

public:

	ATHRenderNode();

	void SetImage( ATHAtlas::ATHTextureHandle _texture, int _index = 0 ) { m_pTexture[_index] = _texture; }
	void SetTransform( D3DXMATRIX _transform ){ m_matTransform = _transform; }

	friend class ATHRenderer;
};

#endif