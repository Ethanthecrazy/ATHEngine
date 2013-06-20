#include "ATHRenderNode.h"

ATHRenderNode::ATHRenderNode()
{	
	D3DXMatrixIdentity( &m_matTransform );

	m_fColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_fCropping = float4( 0.0f, 0.0f, 1.0f, 1.0f );
	m_pNext = nullptr;
}

ATHRenderNode::~ATHRenderNode()
{

}