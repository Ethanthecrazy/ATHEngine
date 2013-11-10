#include "ATHObjectManager.h"

#include <fstream>

#include "../ATHRenderer/ATHRenderer.h"
#include "../ATHUtil/NewInclude.h"
#include "../Box2D/Box2D.h"
#include "RapidXML/rapidxml.hpp"
#include "ATHObject.h"

const unsigned int	NUM_VELOCITY_ITERATIONS = 8;
const unsigned int	NUM_POSITION_ITERATIONS = 5;
const float			TIMESTEP_LENGTH = (1.0f/30.0f);
const float			MAX_TIMEBUFFER = 1.0f;
const char			DEFAULT_XML_LOAD_PATH[] = "data\\base.xml";
const float GLOBAL_LOAD_SCALE = 0.01f;

ATHObjectManager::ATHObjectManager() :	m_fTimeBuffer( 0.0f ),
										m_pWorld( nullptr )
{
}
//================================================================================
void ATHObjectManager::Init()
{
	// Box2D Init
	m_pWorld = new b2World( b2Vec2( 0.0f, -3.0f ) );

	ATHBox2DRenderer* pDebugRenderer = ATHRenderer::GetInstance()->GetDebugRenderer();
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
	flags += b2Draw::e_centerOfMassBit;
	pDebugRenderer->SetFlags(flags);
	m_pWorld->SetDebugDraw( pDebugRenderer );

	b2Vec2 vertices[3];
	vertices[0].Set(0.0f, 0.0f);
	vertices[1].Set(1.0f, 0.0f);
	vertices[2].Set(0.0f, 1.0f);

	int32 count = 3;
	b2PolygonShape polygon;
	b2BodyDef bodyDef;

	bodyDef.type = b2_staticBody;
	bodyDef.position = b2Vec2( 0.0f, 0.0f );
	polygon.SetAsBox( 20.0f, 1.0f, b2Vec2( 0.0f, -10.0f ), 0.0f );
	m_pWorld->CreateBody( &bodyDef )->CreateFixture( &polygon, 1.0f );
}
//================================================================================
void ATHObjectManager::Update( float _fDT )
{
	m_fTimeBuffer += _fDT;

	// Cap the time to avoide a complete explosive meltodwn
	if( m_fTimeBuffer > MAX_TIMEBUFFER )
		m_fTimeBuffer = MAX_TIMEBUFFER;

	while( m_fTimeBuffer > TIMESTEP_LENGTH )
	{
		m_pWorld->Step( TIMESTEP_LENGTH, NUM_VELOCITY_ITERATIONS, NUM_POSITION_ITERATIONS );
		m_fTimeBuffer -= TIMESTEP_LENGTH;
	}

	m_pWorld->DrawDebugData();

	std::list<ATHObject*>::iterator itrObjects = m_liObjects.begin();
	std::list<ATHObject*>::iterator itrObjectsEnd = m_liObjects.end();
	while( itrObjects != itrObjectsEnd )
	{
		ATHObject* pCurrObj = (*itrObjects);
		if( pCurrObj->GetAlive() )
		{
			if( pCurrObj->GetActive() )
				pCurrObj->Update( _fDT );
		}
		else
			m_liToRemove.push_back( pCurrObj );

		++itrObjects;
	}

	itrObjects = m_liToRemove.begin();
	itrObjectsEnd = m_liToRemove.end();
	while( itrObjects != itrObjectsEnd )
	{
		m_liObjects.remove( (*itrObjects) );
		delete (*itrObjects);
		itrObjects = m_liToRemove.erase( itrObjects );
	}
}
//================================================================================
void ATHObjectManager::Shutdown()
{
	ClearObjects();
	delete m_pWorld;
}
//================================================================================
void ATHObjectManager::AddObject( ATHObject* pObject )
{
	if( pObject )
		m_liObjects.push_back( pObject );
}
//================================================================================
void ATHObjectManager::AddObjectStatic( ATHObject* pObject )
{
	if( pObject )
		m_liStaticObjects.push_back( pObject );
}
//================================================================================
void ATHObjectManager::ClearObjects()
{
	std::list<ATHObject*>::iterator itrObjects = m_liObjects.begin();
	std::list<ATHObject*>::iterator itrObjectsEnd = m_liObjects.end();
	while( itrObjects != itrObjectsEnd )
	{
		delete (*itrObjects);
		itrObjects = m_liObjects.erase( itrObjects );
	}

	itrObjects = m_liStaticObjects.begin();
	itrObjectsEnd = m_liStaticObjects.end();
	while( itrObjects != itrObjectsEnd )
	{
		delete (*itrObjects);
		itrObjects = m_liObjects.erase( itrObjects );
	}
}
//================================================================================
void ATHObjectManager::LoadObjectsFromXML()
{
	LoadXML( DEFAULT_XML_LOAD_PATH );
}
//================================================================================
void ATHObjectManager::LoadXML( const char* _szPath )
{
	std::string line;
	std::ifstream myfile ( _szPath );
	if (!myfile.is_open())
		return;

	// Get the size of the file
	size_t begin = (size_t)myfile.tellg();
	myfile.seekg(0, std::ios::end );
	size_t end = (size_t)myfile.tellg();
	size_t size = end - begin;
	myfile.seekg(0, std::ios_base::beg);

	// Load the data into an array and give it a null terminator.
	char* pString = new char[size + 1];
	myfile.read( pString, size );
	pString[ size ] = '\0'; 

	myfile.close();

	// Create the rapidxml document
	rapidxml::xml_document<> doc;
	doc.parse<0>( pString );

	// Start iterating through objects
	rapidxml::xml_node<>* nodeWorld = doc.first_node();
	rapidxml::xml_node<>* nodeObjects = nodeWorld->first_node( "Objects" );
	rapidxml::xml_node<>* currObject = nodeObjects->first_node();
	while( currObject )
	{
		ATHObject* testObject = new ATHObject();

		ATHRenderNode* pRenderNode = GenerateRenderNode( currObject );
		b2Body*	pBody = GenerateBody( currObject );

		testObject->Init( pRenderNode, pBody );

		AddObject( testObject );


		currObject = currObject->next_sibling();
	}

	delete[] pString;

}
//================================================================================
b2Body* ATHObjectManager::GenerateBody( rapidxml::xml_node<>* pNodeObject )
{
	b2Body* pReturnBody =  nullptr;

	
	rapidxml::xml_node<>* nodePos = pNodeObject->first_node( "Position" );
	rapidxml::xml_attribute<>* nodePosAttrX = nodePos->first_attribute("X");
	rapidxml::xml_attribute<>* nodePosAttrY = nodePos->first_attribute("Y");

	float fObjectPosX = (float)atof( nodePosAttrX->value() ) * GLOBAL_LOAD_SCALE;
	float fObjectPosY = (float)atof( nodePosAttrY->value() ) * GLOBAL_LOAD_SCALE;

	rapidxml::xml_node<>* nodeCollGeo = pNodeObject->first_node( "Collision_Geometry" );
	if( nodeCollGeo )
	{
		rapidxml::xml_attribute<>* attrVertCount = nodeCollGeo->first_attribute( "Number_of_Verts" );

		int unVertexCount =  atoi( attrVertCount->value() );
		int unCurrentVertex = 0;

		b2Vec2* vertices = new b2Vec2[unVertexCount];

		rapidxml::xml_node<>* currVertex = nodeCollGeo->first_node();
		while( currVertex )
		{
			rapidxml::xml_attribute<>* attrX = currVertex->first_attribute( "X" );
			float posX = (float)atof( attrX->value() ) * GLOBAL_LOAD_SCALE;

			rapidxml::xml_attribute<>* attrY = currVertex->first_attribute( "Y" );
			float posY = (float)atof( attrY->value() ) * GLOBAL_LOAD_SCALE;

			vertices[ unCurrentVertex ].Set( posX, posY );

			unCurrentVertex++;
			currVertex = currVertex->next_sibling();
		}

		b2PolygonShape polygon;
		b2BodyDef bodyDef;
		polygon.Set(vertices, unVertexCount);

		bodyDef.type = b2_dynamicBody;

		bodyDef.position = b2Vec2( fObjectPosX, fObjectPosY );

		pReturnBody = m_pWorld->CreateBody( &bodyDef );
		pReturnBody->CreateFixture( &polygon, 1.0f );

		delete[] vertices;
	}

	return pReturnBody;
}
//================================================================================
ATHRenderNode* ATHObjectManager::GenerateRenderNode( rapidxml::xml_node<>* pNodeObject )
{
	ATHRenderNode* pReturnNode = nullptr;

	rapidxml::xml_attribute<>* pTexturePathAttr = pNodeObject->first_attribute( "Image_Path" );
	char* szTexturePath = pTexturePathAttr->value();

	if( strlen( szTexturePath ) > 0 )
	{
		pReturnNode = ATHRenderer::GetInstance()->CreateRenderNode( "test", 0 );
		ATHAtlas::ATHTextureHandle texHandle = ATHRenderer::GetInstance()->GetAtlas()->GetTexture( szTexturePath );

		pReturnNode->SetMesh( &ATHRenderer::GetInstance()->m_Quad );
		pReturnNode->SetTexture( texHandle );


		rapidxml::xml_node<>* pSizeNode = pNodeObject->first_node( "Size" );
		rapidxml::xml_attribute<>* pWidthAttr = pSizeNode->first_attribute( "Width" );
		rapidxml::xml_attribute<>* pHeightAttr = pSizeNode->first_attribute( "Height" );

		float fObjectWidth = (float)atof( pWidthAttr->value() ) * GLOBAL_LOAD_SCALE;
		float fObjectHeight = (float)atof( pHeightAttr->value() ) * GLOBAL_LOAD_SCALE;

		D3DXMATRIX matScale;
		D3DXMatrixScaling( &matScale, fObjectWidth, fObjectHeight, 1.0f );

		pReturnNode->SetLocalTransform( matScale );

	}

	return pReturnNode;
}
//================================================================================