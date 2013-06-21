#include "ATHRenderNode.h"

ATHRenderNode::ATHRenderNode()
{	
	D3DXMatrixIdentity( &m_matTransform );

	m_pNext = nullptr;
}

ATHRenderNode::~ATHRenderNode()
{

}