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
const float			MAX_TIMEBUFFER = 0.5f;
const char			DEFAULT_XML_LOAD_PATH[] = "data\\base.xml";
const char			DEFAULT_OBJ_LIBRARY_PATH[] = "data\\object_library.xml";
const float GLOBAL_LOAD_SCALE = 0.01f;

ATHObjectManager::ATHObjectManager() :	m_fTimeBuffer( 0.0f ),
										m_pWorld( nullptr ),
										m_szLibraryBuffer( nullptr ),
										m_pLibraryObjectsNode( nullptr )
{
}
//================================================================================
void ATHObjectManager::Init()
{
	InitBox2D();
	LoadObjLibFromXML();
	LoadObjectsFromXML();
}
//================================================================================
void ATHObjectManager::InitBox2D()
{
	// Box2D Init
	m_pWorld = new b2World(b2Vec2(0.0f, 0.0f));

	ATHBox2DRenderer* pDebugRenderer = ATHRenderer::GetInstance()->GetDebugRenderer();
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
	flags += b2Draw::e_centerOfMassBit;

	pDebugRenderer->SetFlags(flags);
	m_pWorld->SetDebugDraw(pDebugRenderer);

	m_pWorld->SetContactListener(this);

	// https://dl.dropboxusercontent.com/u/22926149/ATHEngine/Comments/ATHObjectManager.Init.txt
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

	if (m_szLibraryBuffer)
		delete m_szLibraryBuffer;
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
ATHObject* ATHObjectManager::InstanceObject(float3 _fPos, char* _szName)
{
	if (!m_pLibraryObjectsNode)
		return nullptr;

	rapidxml::xml_node<>* pObjectNode = m_pLibraryObjectsNode->first_node(_szName);
	if (!pObjectNode)
	{
		std::cout << "Failed to instance object " << _szName << "\n";
		return nullptr;
	}
		

	ATHObject* pNewObject = GenerateObject(pObjectNode);
	pNewObject->SetPosition(_fPos);

	AddObject(pNewObject);

	return pNewObject;

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
void ATHObjectManager::BeginContact(b2Contact* contact)
{
	ATHObject* pObjectA = (ATHObject*)contact->GetFixtureA()->GetBody()->GetUserData();
	ATHObject* pObjectB = (ATHObject*)contact->GetFixtureB()->GetBody()->GetUserData();

	IF(pObjectA)->OnCollisionEnter(pObjectB);
	IF(pObjectB)->OnCollisionEnter(pObjectA);
}
//================================================================================
void ATHObjectManager::EndContact(b2Contact* contact)
{
	ATHObject* pObjectA = (ATHObject*)contact->GetFixtureA()->GetBody()->GetUserData();
	ATHObject* pObjectB = (ATHObject*)contact->GetFixtureB()->GetBody()->GetUserData();

	IF(pObjectA)->OnCollisionExit(pObjectB);
	IF(pObjectB)->OnCollisionExit(pObjectA);
}
//================================================================================
char* ATHObjectManager::GetFileAsText(const char* _szPath)
{
	std::ifstream myfile(_szPath);
	if (!myfile.is_open())
		return nullptr;

	// Get the size of the file
	size_t begin = (size_t)myfile.tellg();
	myfile.seekg(0, myfile.end);
	size_t end = (size_t)myfile.tellg();
	size_t tSize = end - begin;
	myfile.seekg(0, myfile.beg);

	// Load the data into an array and give it a null terminator.
	char* pString = new char[tSize];
	memset(pString, 0, tSize);
	myfile.read(pString, tSize);

	myfile.close();

	return pString;
}
//================================================================================
void ATHObjectManager::LoadObjectsFromXML()
{
	LoadXML( DEFAULT_XML_LOAD_PATH );
}
//================================================================================
void ATHObjectManager::LoadObjLibFromXML()
{
	m_szLibraryBuffer = GetFileAsText(DEFAULT_OBJ_LIBRARY_PATH);
	// Create the rapidxml document
	m_xmlLibraryDoc.parse<0>(m_szLibraryBuffer);

	m_pLibraryObjectsNode = m_xmlLibraryDoc.first_node();
}
//================================================================================
void ATHObjectManager::LoadXML( const char* _szPath )
{
	char* pString = GetFileAsText( _szPath );

	// Create the rapidxml document
	rapidxml::xml_document<> doc;
	doc.parse<0>( pString );

	// Start iterating through objects
	rapidxml::xml_node<>* nodeWorld = doc.first_node();
	rapidxml::xml_node<>* nodeObjects = nodeWorld->first_node( "Objects" );
	rapidxml::xml_node<>* currObject = nodeObjects->first_node( "Object" );
	while (currObject)
	{
		ATHObject* pNewObject = GenerateObject(currObject);
		AddObject(pNewObject);

		currObject = currObject->next_sibling("Object");
	}

	rapidxml::xml_node<>* currReference = nodeObjects->first_node("Reference");
	while (currReference)
	{
		GenerateObjectFromReference(currReference);
	
		currReference = currReference->next_sibling("Reference");
	}
	delete[] pString;

}
//================================================================================
ATHObject* ATHObjectManager::GenerateObject(rapidxml::xml_node<>* pRootObjNode)
{
	ATHObject* pReturnObject = nullptr;

	rapidxml::xml_attribute<>* currObjName = pRootObjNode->first_attribute("Name");

	pReturnObject = new ATHObject();

	// Assign a name to the object
	if (currObjName)
		pReturnObject->m_strName = currObjName->value();
	else
		pReturnObject->m_strName = "Object";

	// Handle Property setup
	LoadProperties( pReturnObject, pRootObjNode->first_node("Properties") );

	ATHRenderNode* pRenderNode = GenerateRenderNode(pRootObjNode);
	b2Body*	pBody = GenerateB2Body(pRootObjNode);

	pReturnObject->Init(pRenderNode, pBody);

	return pReturnObject;
}
//================================================================================
ATHObject* ATHObjectManager::GenerateObjectFromReference(rapidxml::xml_node<>* pRefNode)
{
	ATHObject* pReturnObject = nullptr;
	rapidxml::xml_attribute<>* currObjName = pRefNode->first_attribute("Name");

	float3 fPos(0.0f);

	rapidxml::xml_node<>* currObjPosNode = pRefNode->first_node("Position");
	if (currObjPosNode)
	{
		fPos.vX = (float)atof(currObjPosNode->first_attribute("X")->value()) * GLOBAL_LOAD_SCALE;
		fPos.vY = (float)atof(currObjPosNode->first_attribute("Y")->value()) * GLOBAL_LOAD_SCALE;
		fPos.vZ = (float)atof(currObjPosNode->first_attribute("Z")->value()) * GLOBAL_LOAD_SCALE;
	}

	pReturnObject = InstanceObject(fPos, currObjName->value());

	return pReturnObject;
}
//================================================================================
void ATHObjectManager::LoadProperties(ATHObject* _pLoadTarget, rapidxml::xml_node<>* _pXMLPropertiesNode)
{
	if (_pXMLPropertiesNode == nullptr)
		return;

	rapidxml::xml_node<>* pPropertyNode = _pXMLPropertiesNode->first_node("Property");
	while (pPropertyNode)
	{
		rapidxml::xml_attribute<>* pAttrName = pPropertyNode->first_attribute("Name");
		rapidxml::xml_attribute<>* pAttrType = pPropertyNode->first_attribute("Type");
		rapidxml::xml_attribute<>* pAttrValue = pPropertyNode->first_attribute("Value");

		char* szAttrType = pAttrType->value();
		if (strcmp(szAttrType, "INTEGER") == 0)
		{
			int nVal = atoi(pAttrValue->value());
			_pLoadTarget->SetProperty(pAttrName->value(), &nVal, APT_INT);
		}
		else if (strcmp(szAttrType, "FLOAT") == 0)
		{
			float fVal = (float)atof(pAttrValue->value());
			_pLoadTarget->SetProperty(pAttrName->value(), &fVal, APT_FLOAT);
		}
		else if (strcmp(szAttrType, "STRING") == 0)
		{
			_pLoadTarget->SetProperty(pAttrName->value(), pAttrValue->value(), APT_STRING, strlen(pAttrValue->value()));
		}

		pPropertyNode = pPropertyNode->next_sibling("Property");
	}

	//Testcode
	int nTest = _pLoadTarget->GetPropertyAsInt("MaxHealth");
	float fTest = _pLoadTarget->GetPropertyAsFloat("Damage");
	std::string strTest = _pLoadTarget->GetPropertyAsString("CommandName");

}
//================================================================================
b2Body* ATHObjectManager::GenerateB2Body(rapidxml::xml_node<>* pXMLNode)
{
	b2Body* pReturnBody = nullptr;
	
	rapidxml::xml_node<>* pBodyNode = pXMLNode->first_node("B2Body");
	if (!pBodyNode)
		return nullptr;

	// Get the position of the object
	float fPosX = 0.0f;
	float fPosY = 0.0f;
	float fPosZ = 0.0f;

	if (rapidxml::xml_node<>* nodePos = pXMLNode->first_node("Position"))
	{
		fPosX = (float)atof(nodePos->first_attribute("X")->value()) * GLOBAL_LOAD_SCALE;
		fPosY = (float)atof(nodePos->first_attribute("Y")->value()) * GLOBAL_LOAD_SCALE;
		fPosZ = (float)atof(nodePos->first_attribute("Z")->value()) * GLOBAL_LOAD_SCALE;
	}

	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2( fPosX, fPosY );

	// Set the type of the body to be created;
	char* szBodyType = pBodyNode->first_attribute("Type")->value();
	if (!strcmp(szBodyType, "static"))
		bodyDef.type = b2_staticBody;
	else if (!strcmp(szBodyType, "kinematic"))
		bodyDef.type = b2_kinematicBody;
	else if (!strcmp(szBodyType, "dynamic"))
		bodyDef.type = b2_dynamicBody;

	float fBodyDensity = (float)atof(pBodyNode->first_attribute("Density")->value());

	// Create the body
	pReturnBody = m_pWorld->CreateBody(&bodyDef);

	// Grab the shape node
	rapidxml::xml_node<>* pNodeShape = pBodyNode->first_node("B2Shape");
	if (!pNodeShape)
		return nullptr;

	while (pNodeShape)
	{
		b2FixtureDef* pFixtureDef = new b2FixtureDef();
		pFixtureDef->density = fBodyDensity;
		
		// Check if its a sensor
		rapidxml::xml_attribute<>* pSensorAttr = pNodeShape->first_attribute("IsSensor");
		if (pSensorAttr)
		{
			if (strcmp(pSensorAttr->value(), "true") == 0)
				pFixtureDef->isSensor = true;
		}
			

		// chain	unsupported
		// circle	supported
		// edge		unsupported
		// polygon: supported
		char* szShapeType = pNodeShape->first_attribute("Type")->value();
		if (!strcmp(szShapeType, "circle"))
		{
			pFixtureDef->shape = GenerateB2CircleShape(pNodeShape);
		}
		else if (!strcmp(szShapeType, "polygon"))
		{
			pFixtureDef->shape = GenerateB2PolygonShape(pNodeShape);
		}

		// Clone the shape onto the body and clean up
		pReturnBody->CreateFixture(pFixtureDef);

		if (pFixtureDef->shape )
			delete pFixtureDef->shape;

		delete pFixtureDef;

		pNodeShape = pNodeShape->next_sibling("B2Shape");
	}



	return pReturnBody;
}
//================================================================================
b2Shape* ATHObjectManager::GenerateB2PolygonShape(rapidxml::xml_node<>* pXMLShapeNode)
{
	b2PolygonShape* pB2Polygon = new b2PolygonShape();
	b2Vec2* vertices = new b2Vec2[b2_maxPolygonVertices];

	unsigned int unCurrentVertex = 0;
	rapidxml::xml_node<>* currVertex = pXMLShapeNode->first_node("Vertex");
	while (currVertex)
	{
		rapidxml::xml_attribute<>* attrX = currVertex->first_attribute("X");
		float posX = (float)atof(attrX->value()) * GLOBAL_LOAD_SCALE;

		rapidxml::xml_attribute<>* attrY = currVertex->first_attribute("Y");
		float posY = (float)atof(attrY->value()) * GLOBAL_LOAD_SCALE;

		vertices[unCurrentVertex].Set(posX, posY);

		unCurrentVertex++;
		currVertex = currVertex->next_sibling("Vertex");
	}

	pB2Polygon->Set(vertices, unCurrentVertex);

	delete[] vertices;

	return pB2Polygon;
}
//================================================================================
b2Shape* ATHObjectManager::GenerateB2CircleShape(rapidxml::xml_node<>* pXMLShapeNode)
{
	b2CircleShape* pB2Circle = new b2CircleShape();

	pB2Circle->m_radius = (float)(atof(pXMLShapeNode->first_attribute("Radius")->value())) * GLOBAL_LOAD_SCALE;

	return pB2Circle;
}
//================================================================================
ATHRenderNode* ATHObjectManager::GenerateRenderNode(rapidxml::xml_node<>* pXMLNode )
{
	ATHRenderNode* pReturnNode = nullptr;

	rapidxml::xml_node<>* pRenderNode = pXMLNode->first_node("RenderNode");
	if (!pRenderNode)
		return nullptr;

	char* szPassName = pRenderNode->first_attribute("PassName")->value();
	int unNodePriority = atoi( pRenderNode->first_attribute("Priority")->value() );

	// Get the dimensions of the render object
	float fDimX = 1.0f;
	float fDimY = 1.0f;
	float fDimZ = 1.0f;

	if (rapidxml::xml_node<>* nodeDims = pRenderNode->first_node("Dimension"))
	{
		fDimX = (float)atof(nodeDims->first_attribute("X")->value()) * GLOBAL_LOAD_SCALE;
		fDimY = (float)atof(nodeDims->first_attribute("Y")->value()) * GLOBAL_LOAD_SCALE;
		fDimZ = (float)atof(nodeDims->first_attribute("Z")->value()) * GLOBAL_LOAD_SCALE;
	}

	// Get the texture path
	char* szTexturePath = nullptr;
	if (rapidxml::xml_node<>* pNodeTexture = pRenderNode->first_node("Texture"))
	{
		szTexturePath = pNodeTexture->first_attribute("Path")->value();
	}

	// Get mesh path
	char* szMeshPath = nullptr;
	if (rapidxml::xml_node<>* pNodeMesh = pRenderNode->first_node("Mesh"))
	{
		szMeshPath = pNodeMesh->first_attribute("Path")->value();
	}

	// Create the render node
	pReturnNode = ATHRenderer::GetInstance()->CreateRenderNode( szPassName, unNodePriority);
	
	// Scale the render node
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, fDimX, fDimY, fDimZ );
	pReturnNode->SetLocalTransform(matScale);

	// Set the texture of the render node
	ATHAtlas::ATHTextureHandle texHandle = ATHRenderer::GetInstance()->GetAtlas()->GetTexture(szTexturePath);
	if (texHandle.Valid())
		pReturnNode->SetTexture(texHandle);
	else
		std::cout << "Failed to find texture at path '" << szTexturePath << "'\n";

	// Set the mesh of the render node
	if (!strcmp( szMeshPath, "QUAD" ))
		pReturnNode->SetMesh(&ATHRenderer::GetInstance()->m_Quad);

	return pReturnNode;
}
//================================================================================