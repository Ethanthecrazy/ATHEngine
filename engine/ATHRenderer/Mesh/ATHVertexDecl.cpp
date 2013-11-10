#include "ATHVertexDecl.h"
#include "../ATHRenderer.h"

void ATHVertexDecl::Rebuild()
{
}
//================================================================================
ATHVertexDecl::ATHVertexDecl()
{
	m_bDeclDirty = true;
	m_unVertexSize = 0;
	m_pCurrVertex = nullptr;
	m_pShortDecl = nullptr;
}
//================================================================================
ATHVertexDecl::~ATHVertexDecl()
{
	if( m_pShortDecl != nullptr )
	{
		m_pShortDecl->Release();
		m_pShortDecl = nullptr;
	}
}
//================================================================================
void ATHVertexDecl::AddVertexElement( D3DDECLUSAGE _Usage )
{
	unsigned int _Type = g_VertexUsageTypes[ _Usage ];
	// Size and offset setup
	unsigned int unCurrAddSize = g_VertexTypeSizes[ _Type ];
	unsigned int unPrevVertexSize = m_unVertexSize;
	m_unVertexSize += unCurrAddSize;

	D3DVERTEXELEMENT9 element = { 0, unPrevVertexSize,  _Type, D3DDECLMETHOD_DEFAULT, _Usage, 0 };
	m_vecDeclElements.push_back( ATHDeclElement( element ) );

	unsigned int unNewIndex = m_vecDeclElements.size() - 1;
	m_vecDeclElements[ unNewIndex ].m_unIndex = unNewIndex;

	// Time to rebuild;
	m_bDeclDirty = true;

}
//================================================================================
const std::vector< ATHDeclElement >& ATHVertexDecl::GetLongDecl()
{
	return m_vecDeclElements;
}
//================================================================================
IDirect3DVertexDeclaration9* ATHVertexDecl::GetShortDecl()
{
	if( m_bDeclDirty )
	{
		BuildDecl();
		m_bDeclDirty = false;
	}

	return m_pShortDecl;
}
//================================================================================
void ATHVertexDecl::BuildDecl()
{
	LPDIRECT3DDEVICE9 pDevice = ATHRenderer::GetInstance()->GetDevice();
	
	if( pDevice )
	{
		unsigned int unElementCount = m_vecDeclElements.size() + 1;

		void* pDeclBuffer = new char[ unElementCount * sizeof( D3DVERTEXELEMENT9 ) ];
		memset( pDeclBuffer, 0, unElementCount * sizeof( D3DVERTEXELEMENT9 ) );

		D3DVERTEXELEMENT9* pElements = ( D3DVERTEXELEMENT9* )pDeclBuffer;

		for( unsigned int i = 0; i < m_vecDeclElements.size(); ++i )
		{
			pElements[i] = m_vecDeclElements[i].m_Element;
		}

		// Add the end of the decl
		D3DVERTEXELEMENT9 endDecl = D3DDECL_END();
		pElements[ unElementCount - 1 ] = endDecl;

		if( m_pShortDecl )
		{
			m_pShortDecl->Release();
			m_pShortDecl = nullptr;
		}

		pDevice->CreateVertexDeclaration( pElements, &m_pShortDecl );

		delete[] pDeclBuffer;
	}

}
//================================================================================
void ATHVertexDecl::VertexSet( void* _pTarget )
{
	m_pCurrVertex = _pTarget;
}
//================================================================================
void ATHVertexDecl::VertexClear()
{
	m_pCurrVertex = nullptr;
}
//================================================================================
void ATHVertexDecl::Clear()
{

}