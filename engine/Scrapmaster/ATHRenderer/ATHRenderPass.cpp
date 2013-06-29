#include "ATHRenderpass.h"

bool ATHRenderPass::NodeContainer::operator< ( NodeContainer& cmp )
{
	return ( GetNode()->GetRenderPriority() < cmp.GetNode()->GetRenderPriority() );
}

ATHRenderPass::ATHRenderPass() :	m_Process( NULL ),
									m_bDepthDirty( false )
{

}

void ATHRenderPass::AddNodeToPass( ATHRenderNode* _node, unsigned int _priority )
{
	_node->AddPassName( m_szPassName );
	m_vecNodes.push_back( NodeContainer( _node ) );
	m_bDepthDirty  = true;
}

void ATHRenderPass::RemoveNodeFromPass( ATHRenderNode* _node )
{
	_node->RemovePassname( m_szPassName );
}

void ATHRenderPass::SortNodes()
{

}