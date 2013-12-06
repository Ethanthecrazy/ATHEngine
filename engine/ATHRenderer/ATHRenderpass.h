#ifndef ATHRENDERPASS_H
#define ATHRENDERPASS_H

#include "ATHRenderNode.h"
#include "ATHRenderFunction.h"

#include <map>
#include <list>

class ATHRenderer;

class ATHRenderPass
{

private:

	std::string		m_szPassName;
	unsigned int	m_unPriority;
	RenderFunc		m_Process;
	ID3DXEffect*	m_pShader;
	char			m_szTechnique[24];
	std::list<ATHRenderNode*> m_liNodes;
	bool			m_bDepthDirty;
	bool			m_bRenderDepth;
	

public:

	ATHRenderPass();
	ATHRenderPass( char* _szName, unsigned int _unPriority, ID3DXEffect* _pShader, RenderFunc _function, bool _bRenderToDepth,  char* _szTechnique );

	// Functionality
	void AddNodeToPass( ATHRenderNode* _node, unsigned int _priority );
	void RemoveNodeFromPass( ATHRenderNode* _node );
	void SortNodes();
	void PreExecute();
	void Execute( ATHRenderer* _pRenderer );
	void PostExecute();
	
	// Accessors 
	void SetRenderToDepth( bool _render ) { m_bRenderDepth = _render; }
	bool GetRenderToDepth() { return m_bRenderDepth; }

	inline unsigned int GetPriority() { return m_unPriority; }

	const std::list<ATHRenderNode*>& GetNodeList() { return m_liNodes; }

};

#endif
