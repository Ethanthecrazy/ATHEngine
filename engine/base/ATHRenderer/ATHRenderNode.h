#ifndef ATHRENDERNODE_H
#define ATHRENDERNODE_H

#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <string>

#include "../ATHUtil/hDataTypes.h"
#include "Texture/ATHAtlas.h"
#include "Mesh/ATHMesh.h"

// Forward declare the render node so that the function pointer can reference it
// Then the node can contain a function pointer that references its own class.
class ATHRenderNode;
class ATHRenderer;

const int RENDERNODE_TEXTURE_COUNT = 3;

//Function pointer
typedef void (*RenderFunc)(ATHRenderer* renderer, ID3DXEffect* _pShader, ATHRenderNode* node);

class ATHRenderNode
{

private:

	ATHAtlas::ATHTextureHandle	m_pTexture[RENDERNODE_TEXTURE_COUNT];
	ATHMesh*					m_pMesh;
	D3DXMATRIX					m_matTransform;

	bool						m_bDirty;
	unsigned int				m_unRenderPriority;

	// Nothing except the ATHRenderer is allowed to destroy these.
	~ATHRenderNode();

	
	std::vector<std::string> m_vecPassNames;

	//Only the managers can manipulate the name list;
	void AddPassName( std::string _name );
	void RemovePassname( std::string _name );


public:

	ATHRenderNode();

	std::vector< std::string > GetPassNames();

	void SetRenderPriority( unsigned int _priority );
	unsigned int GetRenderPriority() { return m_unRenderPriority; }

	void						SetTexture( ATHAtlas::ATHTextureHandle _texture, int _index = 0 ) { m_pTexture[_index] = _texture; }
	ATHAtlas::ATHTextureHandle	GetTexture( unsigned int _nIndex = 0 ) { return m_pTexture[ _nIndex ]; }
	void						SetTransform( D3DXMATRIX _transform ){ m_matTransform = _transform; }
	D3DXMATRIX					GetTrasform() { return m_matTransform; }
	void						SetMesh( ATHMesh* _pMesh ) { m_pMesh = _pMesh; }
	ATHMesh*					GetMesh() { return m_pMesh; }

	friend class ATHRenderer;
	friend class ATHRenderPass;
	friend class ATHRenderNodeHandle;
};

#endif