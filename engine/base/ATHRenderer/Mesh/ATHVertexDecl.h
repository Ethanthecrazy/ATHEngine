#ifndef ATHVERTEXDECL_H
#define ATHVERTEXDECL_H

#include <d3dx9.h>
#include <vector>

// Sized in bytes
static unsigned int g_VertexTypeSizes[ D3DDECLTYPE_UNUSED ] = {	4,	    // D3DDECLTYPE_FLOAT1    =  0,  	1D float expanded to (value, 0., 0., 1.)
																8,		// D3DDECLTYPE_FLOAT2    =  1,  	2D float expanded to (value, value, 0., 1.)
																12,		// D3DDECLTYPE_FLOAT3    =  2,  	3D float expanded to (value, value, value, 1.)
																16,		// D3DDECLTYPE_FLOAT4    =  3,  	4D float
																16,		// D3DDECLTYPE_D3DCOLOR  =  4,  	4D packed unsigned bytes mapped to 0. to 1. range
																		// 									Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
																4,		// D3DDECLTYPE_UBYTE4    =  5,		4D unsigned byte
																4,		// D3DDECLTYPE_SHORT2    =  6,		2D signed short expanded to (value, value, 0., 1.)
																8,		// D3DDECLTYPE_SHORT4    =  7,		4D signed short
																4,		// D3DDECLTYPE_UBYTE4N   =  8,		Each of 4 bytes is normalized by dividing to 255.0
																4,		// D3DDECLTYPE_SHORT2N   =  9,		2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
																8,		// D3DDECLTYPE_SHORT4N   = 10,		4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
																4,		// D3DDECLTYPE_USHORT2N  = 11,		2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
																8,		// D3DDECLTYPE_USHORT4N  = 12,		4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
																12,		// D3DDECLTYPE_UDEC3     = 13,		3D unsigned 10 10 10 format expanded to (value, value, value, 1)
																12,		// D3DDECLTYPE_DEC3N     = 14,		3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
																4,		// D3DDECLTYPE_FLOAT16_2 = 15,		Two 16-bit floating point values, expanded to (value, value, 0, 1)
																16 };	// D3DDECLTYPE_FLOAT16_4 = 16,		Four 16-bit floating point values

struct ATHDeclElement
{
	unsigned int		m_unIndex;
	D3DVERTEXELEMENT9	m_Element;

	ATHDeclElement( D3DVERTEXELEMENT9 _element ) : m_unIndex( 0 ), m_Element( _element ) {}
};

class ATHVertexDecl
{
private:

	bool								m_bDeclDirty;
	unsigned int						m_unVertexSize;
	void*								m_pCurrVertex;
	std::vector< ATHDeclElement >		m_vecDeclElements;
	IDirect3DVertexDeclaration9*		m_pShortDecl;

	void Rebuild();

public:

	ATHVertexDecl();
	~ATHVertexDecl();

	unsigned int GetVertexSize() { return m_unVertexSize; }

	void AddVertexElement( D3DDECLUSAGE _Usage, D3DDECLTYPE _Type );
	const std::vector< ATHDeclElement >& GetLongDecl();
	IDirect3DVertexDeclaration9* GetShortDecl();

	void BuildDecl();

	template< typename type > 
	bool VertexSetVar( unsigned int _unUsageIndex, type _nToSet );

	template< typename type > 
	bool VertexGetVar( unsigned int _unUsageIndex, type& _nOut );


	void VertexSet( void* _pTarget );
	void VertexClear();

	void Clear();



};

template< typename type > 
bool ATHVertexDecl::VertexSetVar( unsigned int _unUsageIndex, type _nToSet )
{
	// If our crurrent vertex is null, we cant do anything
	if( !m_pCurrVertex )
		return false;

	if( _unUsageIndex < m_vecDeclElements.size() )
	{
		// Grab the relavent element
		D3DVERTEXELEMENT9 currElement =  m_vecDeclElements[ _unUsageIndex ].m_Element;

		unsigned int unTypeSize = g_VertexTypeSizes[ currElement.Type ];
		if( sizeof( type ) == unTypeSize )
		{
			char* pVertex = (char*)m_pCurrVertex;
			memcpy( pVertex + currElement.Offset, &_nToSet, unTypeSize );
			return true;
		}
	}

	return false;
}

template< typename type > 
bool ATHVertexDecl::VertexGetVar( unsigned int _unUsageIndex, type& _nOut )
{
		// If our crurrent vertex is null, we cant do anything
	if( !m_pCurrVertex )
		return false;

	if( _unUsageIndex < m_vecDeclElements.size() )
	{
		// Grab the relavent element
		D3DVERTEXELEMENT9 currElement =  m_vecDeclElements[ _unUsageIndex ].m_Element;

		unsigned int unTypeSize = g_VertexTypeSizes[ currElement.Type ];
		if( sizeof( type ) == unTypeSize )
		{
			char* pVertex = (char*)m_pCurrVertex;
			type* pTarget = (type*)(pVertex + currElement.Offset);
			_nOut = *pTarget;
			return true;
		}
	}

	return false;
}


#endif