#include "ATHRenderNode.h"
#include "../ATHUtil/NewInclude.h"

ATHRenderNode::ATHRenderNode() : m_bDirty( false )
{	
	D3DXMatrixIdentity( &m_matTransform );


}

ATHRenderNode::~ATHRenderNode()
{

}

void ATHRenderNode::AddPassName( std::string _name )
{
	bool bContainsName = false;

	std::vector< std::string >::iterator itrNames = m_vecPassNames.begin();
	while( itrNames != m_vecPassNames.end() )
	{
		if( (*itrNames) == _name )
			bContainsName = true;
		itrNames++;
	}

	if( !bContainsName )
	{
		m_vecPassNames.push_back( _name );
	}
}

void ATHRenderNode::RemovePassname( std::string _name )
{
	std::vector< std::string >::iterator itrNames = m_vecPassNames.begin();
	while( itrNames != m_vecPassNames.end() )
	{
		if( (*itrNames) == _name )
			itrNames = m_vecPassNames.erase( itrNames );
		else
			itrNames++;
	}
}



std::vector< std::string > ATHRenderNode::GetPassNames()
{
	return m_vecPassNames;
}

void ATHRenderNode::SetRenderPriority( unsigned int _priority )
{
	m_unRenderPriority = _priority;
	m_bDirty = true;
}