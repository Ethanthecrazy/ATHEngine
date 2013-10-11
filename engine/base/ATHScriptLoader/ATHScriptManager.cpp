#include "ATHScriptManager.h"
#include <fstream>

#include "rapidxml.hpp"
#include "../ATHUtil/NewInclude.h"
#include "../Box2D/Box2D.h"
#include "../Objects/ATHObjectManager.h"
#include "../Objects/ATHObject.h"

void ATHScriptManager::LoadXMLScript( ATHObjectManager* _pManager, char* _szFilepath )
{

	if( !_pManager )
		return;

	std::string line;
	std::ifstream myfile ( _szFilepath );
	if (!myfile.is_open())
		return;

	
	size_t begin = (size_t)myfile.tellg();

	myfile.seekg(0, std::ios::end );

	size_t end = (size_t)myfile.tellg();
	size_t size = end - begin;

	myfile.seekg(0, std::ios_base::beg);

	char* pString = new char[size + 1];

	myfile.read( pString, size );

	pString[ size ] = '\0'; 

	myfile.close();

	rapidxml::xml_document<> doc;

	doc.parse<0>( pString );

	rapidxml::xml_node<>* nodeWorld = doc.first_node();
	rapidxml::xml_node<>* nodeObjects = nodeWorld->first_node( "Objects" );

	rapidxml::xml_node<>* currObject = nodeObjects->first_node();
	while( currObject )
	{

		rapidxml::xml_node<>* nodePos = currObject->first_node( "Position" );
		rapidxml::xml_attribute<>* nodePosAttrX = nodePos->first_attribute("X");
		rapidxml::xml_attribute<>* nodePosAttrY = nodePos->first_attribute("Y");

		int unObjectPositionX = 0;//atoi( nodePosAttrX->value() );
		int unObjectPositionY = atoi( nodePosAttrY->value() );


		rapidxml::xml_node<>* nodeCollGeo = currObject->first_node( "Collision_Geometry" );
		if( nodeCollGeo )
		{
			rapidxml::xml_attribute<>* attrVertCount = nodeCollGeo->first_attribute( "Number_of_Verts" );

			int unVertexCount =  atoi( attrVertCount->value() );
			int unCurrentVertex = 0;
			
			b2Vec2 vertices[8];

			rapidxml::xml_node<>* currVertex = nodeCollGeo->first_node();
			while( currVertex )
			{
				rapidxml::xml_attribute<>* attrX = currVertex->first_attribute( "X" );
				int posX = atoi( attrX->value() );

				rapidxml::xml_attribute<>* attrY = currVertex->first_attribute( "Y" );
				int posY = atoi( attrY->value() );

				vertices[ unCurrentVertex ].Set( float( unObjectPositionX + posX ), float ( unObjectPositionY + posY ) );

				unCurrentVertex++;
				currVertex = currVertex->next_sibling();
			}

			b2PolygonShape polygon;
			b2BodyDef bodyDef;
			polygon.Set(vertices, unVertexCount);
			//polygon.SetAsBox( 10.0f, 1.0f, b2Vec2( 0.0f, 0.0f ), 0.0f );

			bodyDef.type = b2_dynamicBody;

			bodyDef.position = b2Vec2( unObjectPositionX, unObjectPositionY );

			b2Body* pBody = _pManager->m_pWorld->CreateBody( &bodyDef );
			pBody->CreateFixture( &polygon, 1.0f );

			ATHObject* testObject = new ATHObject();
			testObject->Init( nullptr, pBody );

			_pManager->AddObject( testObject );

		}

		currObject = currObject->next_sibling();
	}

	delete[] pString;

}