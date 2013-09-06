#include "ATHMesh.h"
#include "ATHRenderer.h"
#include "../ATHUtil/NewInclude.h"
#include "ATHVertexDecl.h"

ATHMesh::~ATHMesh()
{
	if( m_vertBuff )
		m_vertBuff->Release();

	if( m_indexBuff )
		m_indexBuff->Release();
}

void ATHMesh::RebuildBuffers()
{
	if( m_vertBuff )
		m_vertBuff->Release();

	if( m_indexBuff )
		m_indexBuff->Release();

	if( !m_vecVerts.size() )
		return;

	ATHRenderer::GetInstance()->GetDevice()->CreateVertexBuffer( m_vecVerts.size() * m_pVertDecl->GetVertexSize(), 
		D3DUSAGE_WRITEONLY, 0,
		D3DPOOL_MANAGED, &m_vertBuff, 0);

	ATHRenderer::GetInstance()->GetDevice()->CreateIndexBuffer(m_vecIndicies.size() * sizeof(DWORD),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, 
		D3DPOOL_MANAGED, &m_indexBuff, 0);

	unsigned int unPosIndex = 0;
	unsigned int unTexIndex = 0;

	std::vector<ATHDeclElement>::const_iterator itrElems = m_pVertDecl->GetLongDecl().cbegin();
	while( itrElems != m_pVertDecl->GetLongDecl().cend() )
	{
		if( itrElems->m_Element.Usage == D3DDECLUSAGE_POSITION )
		{
			unPosIndex = itrElems->m_unIndex;
		}
		if( itrElems->m_Element.Usage == D3DDECLUSAGE_TEXCOORD )
		{
			unTexIndex = itrElems->m_unIndex;
		}

		++itrElems;
	}

	// fill the buffers
	// This is slow, it should never happen during gameplay.
	char* pVertBuffer = nullptr;
	m_vertBuff->Lock(0,0, (void**)&pVertBuffer, 0);

	for( unsigned int i = 0; i < m_vecVerts.size(); ++i )
	{
		m_pVertDecl->VertexSet( pVertBuffer + m_pVertDecl->GetVertexSize() * i );
		m_pVertDecl->VertexSetVar( unPosIndex, m_vecVerts[i].position );
		m_pVertDecl->VertexSetVar( unTexIndex, m_vecVerts[i].UV );	
	}

	m_vertBuff->Unlock();

	int* pIndexBuffer = nullptr;
	m_indexBuff->Lock(0,0, (void**)&pIndexBuffer,0);

	for( unsigned int i = 0; i < m_vecIndicies.size(); ++i )
	{
		(*pIndexBuffer) = m_vecIndicies[i];
		pIndexBuffer++;
	}

	m_indexBuff->Unlock();


	switch( m_PrimativeType )
	{
		case D3DPT_POINTLIST:
			{
				m_unPrimativeCount = m_vecIndicies.size();
				break;
			}
		case D3DPT_LINELIST:
			{
				m_unPrimativeCount = m_vecIndicies.size() / 2;
				break;
			}
		case D3DPT_LINESTRIP:
			{
				m_unPrimativeCount = 0;
				break;
			}
		case D3DPT_TRIANGLELIST:  
			{
				m_unPrimativeCount = m_vecIndicies.size() / 3;
				break;
			}
		case D3DPT_TRIANGLESTRIP:
			{
				m_unPrimativeCount = 0;
				break;
			}
		case D3DPT_TRIANGLEFAN:
			{
				m_unPrimativeCount = 0;
				break;
			}

	}

}

void ATHMesh::Clear()
{
	if( m_vertBuff )
	{
		m_vertBuff->Release();
		m_vertBuff = nullptr;
	}

	if( m_indexBuff )
	{
		m_indexBuff->Release();
		m_indexBuff = nullptr;
	}

	m_vecVerts.clear();
	m_vecIndicies.clear();
}