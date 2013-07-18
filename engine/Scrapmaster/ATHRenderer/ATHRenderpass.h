#ifndef ATHRENDERPASS_H
#define ATHRENDERPASS_H

#include "ATHRenderNode.h"

#include <map>
#include <vector>

class ATHRenderer;
class ATHRenderPass
{

private:

	// A custom struct to wrangle ATHRenderNode pointers
	struct NodeContainer{
		private:
			ATHRenderNode* m_pMyNode;
			NodeContainer();
		public:
			NodeContainer( ATHRenderNode* _node ) : m_pMyNode( _node ) {}
			// Custom sorting function
			bool operator< ( NodeContainer& cmp );
			//Accessor
			ATHRenderNode* GetNode(){ return m_pMyNode; } };

	std::string m_szPassName;
	unsigned int m_unPriority;
	RenderFunc m_Process;
	std::vector<NodeContainer> m_vecNodes;
	bool m_bDepthDirty;
	

public:

	ATHRenderPass();
	ATHRenderPass( char* _szName, unsigned int _unPriority, RenderFunc _function );

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
