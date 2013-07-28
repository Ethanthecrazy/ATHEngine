#ifndef ATHMESH_H
#define ATHMESH_H

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "../ATHUtil/hDataTypes.h"

struct sVertPosNormUV
{
	D3DXVECTOR3 	position;
	D3DXVECTOR3		normal;
	D3DXVECTOR2		UV;

	sVertPosNormUV() : position( D3DXVECTOR3() ), normal( D3DXVECTOR3() ), UV( D3DXVECTOR2() ) {}
	sVertPosNormUV( float3 _pos, float3 _normal, float2 _UV ) : position( D3DXVECTOR3( _pos.vX, _pos.vY, _pos.vZ ) ), normal( D3DXVECTOR3( _normal.vX, _normal.vY, _normal.vZ ) ), UV( D3DXVECTOR2( _UV.vX, _UV.vY ) ) {} 

};

class ATHMesh
{
private:

	IDirect3DVertexBuffer9* m_vertBuff;
	IDirect3DIndexBuffer9*	m_indexBuff;
	std::string				m_szMeshName;
	D3DPRIMITIVETYPE		m_PrimativeType;

	std::vector< sVertPosNormUV > m_vecVerts;
	std::vector< unsigned int > m_vecIndicies;

public:

	ATHMesh() : m_vertBuff( nullptr ), m_indexBuff( nullptr ) {}
	~ATHMesh();
	std::vector< sVertPosNormUV >& GetVerts() { return m_vecVerts; }
	std::vector< unsigned int >& GetIndicies() { return m_vecIndicies; }

	void RebuildBuffers();

	IDirect3DVertexBuffer9* GetVertexBuffer() { return m_vertBuff; }
	IDirect3DIndexBuffer9* GetIndexBuffer() { return m_indexBuff; }

	std::string GetMeshName()			{return m_szMeshName;}

	unsigned int			GetVertexCount() { return m_vecVerts.size(); }
	unsigned int			GetIndexCount() { return m_vecIndicies.size(); }

	void					SetPrimativeType( D3DPRIMITIVETYPE _type ) { m_PrimativeType = _type; }
	D3DPRIMITIVETYPE		GetPrimativeType() { return m_PrimativeType; }

	void Clear();

};

#endif