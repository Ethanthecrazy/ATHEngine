#ifndef ATHMESH_H
#define ATHMESH_H

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <map>
#include "../../ATHUtil/hDataTypes.h"

struct sVertPosNormUV
{
	D3DXVECTOR3 	position;
	D3DXVECTOR3		normal;
	D3DXVECTOR2		UV;

	sVertPosNormUV() : position( D3DXVECTOR3() ), normal( D3DXVECTOR3() ), UV( D3DXVECTOR2() ) {}
	sVertPosNormUV( float3 _pos, float3 _normal, float2 _UV ) : position( D3DXVECTOR3( _pos.vX, _pos.vY, _pos.vZ ) ), normal( D3DXVECTOR3( _normal.vX, _normal.vY, _normal.vZ ) ), UV( D3DXVECTOR2( _UV.vX, _UV.vY ) ) {} 

};

class ATHVert
{
private:
public:
};

class ATHVertexDecl;

class ATHMesh
{
private:

	std::string				m_szMeshName;
	
	ATHVertexDecl*					m_pVertDecl;
	IDirect3DVertexBuffer9*			m_vertBuff;
	IDirect3DIndexBuffer9*			m_indexBuff;
	D3DPRIMITIVETYPE				m_PrimativeType;
	unsigned int m_unPrimativeCount;

	ATHMesh();

public:

	// Vertex information. All sizes based off of the postion vector.
	std::vector< float3 > m_vecPositions;
	std::vector< float4 > m_vecBlendweights;
	std::vector< float4 > m_vecBlendIndicies;
	std::vector< float3 > m_vecNormals;
	std::vector< float2 > m_vecUVs;
	std::vector< float3 > m_vecTangents;
	std::vector< float4 > m_vecColors;



	std::vector< unsigned int > m_vecIndicies;

	ATHMesh(	char* _szMeshName, 
				ATHVertexDecl* _pVertexDecl, 
				D3DPRIMITIVETYPE _PrimativeType ) :	m_szMeshName( _szMeshName ),
													m_pVertDecl( _pVertexDecl ),
													m_vertBuff( nullptr ), 
													m_indexBuff( nullptr ), 
													m_PrimativeType( _PrimativeType ), 
													m_unPrimativeCount( 0 ) 
	{
	}

	~ATHMesh();

	void							SetVertexDecl( ATHVertexDecl* _pDecl ) { m_pVertDecl = _pDecl; }
	ATHVertexDecl*					GetVertexDecl() { return m_pVertDecl; }

	void RebuildBuffers();

	IDirect3DVertexBuffer9* GetVertexBuffer() { return m_vertBuff; }
	IDirect3DIndexBuffer9* GetIndexBuffer() { return m_indexBuff; }

	std::string GetMeshName()			{return m_szMeshName;}

	unsigned int			GetVertexCount() { return m_vecPositions.size(); }
	unsigned int			GetIndexCount() { return m_vecIndicies.size(); }

	void					SetPrimativeType( D3DPRIMITIVETYPE _type ) { m_PrimativeType = _type; }
	D3DPRIMITIVETYPE		GetPrimativeType() { return m_PrimativeType; }

	unsigned int			GetPrimativeCount() { return m_unPrimativeCount; }

	void Clear();

};

#endif