#include "ATHRenderpass.h"

ATHRenderPass::ATHRenderPass() :	m_Process( NULL ),
									m_bDepthDirty( false )
{

}

ATHRenderPass::ATHRenderPass( char* _szName, unsigned int _unPriority, RenderFunc _function ) : m_szPassName( _szName ),
																								m_unPriority( _unPriority ),
																								m_Process( _function )
{

}

void ATHRenderPass::AddNodeToPass( ATHRenderNode* _node, unsigned int _priority )
{
	_node->AddPassName( m_szPassName );
	m_liNodes.push_back( _node );
	m_bDepthDirty  = true;
}

void ATHRenderPass::RemoveNodeFromPass( ATHRenderNode* _node )
{
	_node->RemovePassname( m_szPassName );
	m_liNodes.remove( _node );
}

void ATHRenderPass::SortNodes()
{
	m_liNodes.sort();
}

void ATHRenderPass::PreExecute()
{
	if( m_bDepthDirty )
	{
		SortNodes();
	}
}
void ATHRenderPass::Execute( ATHRenderer* _pRenderer )
{
	if( m_Process != nullptr )
	{
		std::list<ATHRenderNode*>::iterator itrNode = m_liNodes.begin();
		while( itrNode != m_liNodes.end() )
		{
			m_Process( _pRenderer, (*itrNode) );
			++itrNode;
		}

	}
}