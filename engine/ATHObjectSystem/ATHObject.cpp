#include "ATHObject.h"

#include "../Box2D/Box2D.h"
#include "../ATHRenderer/ATHRenderer.h"
#include "../ATHRenderer/ATHRenderNode.h"

unsigned int ATHObject::s_unIdCounter = 0;

ATHObject::ATHObject()
{
	m_unID = s_unIdCounter;
	s_unIdCounter++;

	m_bAlive = true;
	m_bActive = true;

	D3DXMatrixIdentity( &m_matTransform );
	m_pRenderNode = nullptr;
	m_pBody = nullptr;
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
void ATHObject::FixedUpdate()
{

}
//================================================================================
void ATHObject::SetPosition(float3 _fPos)
{
	if (m_pBody)
	{
		m_pBody->SetTransform(b2Vec2(_fPos.vX, _fPos.vY), m_pBody->GetAngle());
	}
	else
	{
		m_matTransform._41 = _fPos.vX;
		m_matTransform._42 = _fPos.vY;
	}

	m_matTransform._43 = _fPos.vZ;

	Update(0.0f);
}
//================================================================================
void ATHObject::SetProperty(char* _szName, void* pData, ATHPropertyType _type, unsigned int _szSize )
{
	if (_type == APT_STRING)
		_szSize = strlen((char*)pData);

	std::string strName(_szName);
	std::transform(strName.begin(), strName.end(), strName.begin(), tolower );
	
	if (m_mapProperties.count(strName) < 1)
		m_mapProperties.insert( std::pair< std::string, ATHProperty>(strName, ATHProperty()));

	m_mapProperties[strName].SetData(pData, _type, _szSize);
}
//================================================================================
int ATHObject::GetPropertyAsInt(char* _szName)
{
	std::string strName(_szName);
	std::transform(strName.begin(), strName.end(), strName.begin(), tolower);

	if (m_mapProperties[strName].GetPropertyType() != APT_INT)
		return 0;

	if (m_mapProperties.count(strName) < 1)
		return 0;

	return m_mapProperties[strName].GetAsInt();
}
//================================================================================
float ATHObject::GetPropertyAsFloat(char* _szName)
{
	std::string strName(_szName);
	std::transform(strName.begin(), strName.end(), strName.begin(), tolower);

	if (m_mapProperties[strName].GetPropertyType() != APT_FLOAT)
		return 0.0f;

	if (m_mapProperties.count(strName) < 1)
		return 0.0f;

	return m_mapProperties[strName].GetAsFloat();
}
//================================================================================
std::string ATHObject::GetPropertyAsString(char* _szName)
{
	std::string strName(_szName);
	std::transform(strName.begin(), strName.end(), strName.begin(), tolower);

	if (m_mapProperties[strName].GetPropertyType() != APT_STRING )
		return std::string();

	if (m_mapProperties.count(strName) < 1)
		return std::string();
	
	std::string strReturn = m_mapProperties[strName].GetAsString();
	strReturn = strReturn.substr(0, m_mapProperties[strName].GetDataSize());

	return strReturn;;
}
//================================================================================
void ATHObject::OnCollisionEnter(b2Contact* _pContact)
{
	
}
//================================================================================
void ATHObject::OnCollisionExit(b2Contact* _pContact)
{
	
}
//================================================================================