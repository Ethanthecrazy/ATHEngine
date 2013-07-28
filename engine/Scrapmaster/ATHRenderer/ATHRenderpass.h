#ifndef ATHRENDERPASS_H
#define ATHRENDERPASS_H

#include "ATHRenderNode.h"

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
	

public:

	ATHRenderPass();
	ATHRenderPass( char* _szName, unsigned int _unPriority, ID3DXEffect* _pShader, RenderFunc _function,  char* _szTechnique );

	// Functionality
	void AddNodeToPass( ATHRenderNode* _node, unsigned int _priority );
	void RemoveNodeFromPass( ATHRenderNode* _node );
	void SortNodes();
	void PreExecute();
	void Execute( ATHRenderer* _pRenderer );
	

	// Accessors 
	inline unsigned int GetPriority() { return m_unPriority; }

};

#endif
