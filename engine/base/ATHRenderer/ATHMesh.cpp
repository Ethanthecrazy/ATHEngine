#include "ATHMesh.h"
#include "ATHRenderer.h"
#include "../ATHUtil/NewInclude.h"

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

	ATHRenderer::GetInstance()->GetDevice()->CreateVertexBuffer( m_vecVerts.size() * sizeof(sVertPosNormUV), 
		D3DUSAGE_WRITEONLY, 0,
		D3DPOOL_MANAGED, &m_vertBuff, 0);

	ATHRenderer::GetInstance()->GetDevice()->CreateIndexBuffer(m_vecIndicies.size() * sizeof(DWORD),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, 
		D3DPOOL_MANAGED, &m_indexBuff, 0);

	// fill the buffers
	// This is slow, it should never happen during gameplay.
	sVertPosNormUV* pVertBuffer = nullptr;
	m_vertBuff->Lock(0,0, (void**)&pVertBuffer, 0);

	for( unsigned int i = 0; i < m_vecVerts.size(); ++i )
	{
		pVertBuffer->position = m_vecVerts[i].position;
		pVertBuffer->normal = m_vecVerts[i].normal;
		pVertBuffer->UV = m_vecVerts[i].UV;
		pVertBuffer++;
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