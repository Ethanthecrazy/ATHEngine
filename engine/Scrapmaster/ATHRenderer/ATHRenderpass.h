#ifndef ATHRENDERPASS_H
#define ATHRENDERPASS_H

#include "ATHRenderNode.h"

#include <map>
#include <vector>


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
	RenderFunc m_Process;
	std::vector<NodeContainer> m_vecNodes;
	bool m_bDepthDirty;
	

public:

	ATHRenderPass();

	void AddNodeToPass( ATHRenderNode* _node, unsigned int _priority );
	void RemoveNodeFromPass( ATHRenderNode* _node );
	void PreExecute();
	void Execute();
	void SortNodes();

};

#endif
