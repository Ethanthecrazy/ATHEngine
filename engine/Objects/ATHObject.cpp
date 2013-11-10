#include "ATHObject.h"

#include "../Box2D/Box2D.h"
#include "../ATHRenderer/ATHRenderer.h"
#include "../ATHRenderer/ATHRenderNode.h"

ATHObject::ATHObject()
{

}
//================================================================================
ATHObject::~ATHObject()
{
	if( m_pBody )
	{
		m_pBody->GetWorld()->DestroyBody( m_pBody );
		m_pBody->SetUserData( nullptr );
	}

	if( m_pRenderNode )
		ATHRenderer::GetInstance()->DestoryRenderNode( m_pRenderNode );

	m_pBody = nullptr;
	m_pRenderNode = nullptr;
}
//================================================================================
D3DXMATRIX ATHObject::GetTransform()
{
	return m_matTransform;
}
//================================================================================
ATHRenderNode* ATHObject::GetRenderNode()
{
	return m_pRenderNode;
}
//================================================================================
b2Body*	ATHObject::GetBody()
{
	return m_pBody;
}
//================================================================================
void ATHObject::Init( ATHRenderNode* _pRenderNode, b2Body* _pBody )
{
	m_bAlive = true;
	m_bActive = true;

	m_pRenderNode = _pRenderNode;
	m_pBody = _pBody;

	if( m_pBody )
		m_pBody->SetUserData( this );

	D3DXMatrixIdentity( &m_matTransform );
}
//================================================================================
void ATHObject::Update( float _fDT )
{
	if( m_pBody )
	{
		D3DXMATRIX matRot;
		D3DXMatrixRotationZ( &matRot, m_pBody->GetAngle() );

		D3DXMATRIX matTrans;
		D3DXMatrixTranslation( &matTrans, m_pBody->GetPosition().x, m_pBody->GetPosition().y, 0.0f );

		m_matTransform = matRot * matTrans;
	}

	if( m_pRenderNode )
	{
		m_pRenderNode->SetTransform( m_matTransform );
	}
}
//================================================================================
