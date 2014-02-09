#ifndef ATHOBJECTMANAGER_H
#define ATHOBJECTMANAGER_H

#include <list>
#include "RapidXML\rapidxml.hpp"
#include "../ATHUtil/hDataTypes.h"
#include "../Box2D/Dynamics/b2WorldCallbacks.h"

class b2World;
class b2Body;
class ATHRenderNode;
class ATHObject;
class b2Shape;


class ATHObjectManager : public b2ContactListener
{
private:

	float m_fTimeBuffer;

	// The library script needs to be kept in memory
	// The parser doesnt copy that data, it only keeps
	// pointers to the source
	char* m_szLibraryBuffer;
	rapidxml::xml_document<> m_xmlLibraryDoc;
	rapidxml::xml_node<>* m_pLibraryObjectsNode;

	std::list<ATHObject*> m_liObjects;
	std::list<ATHObject*> m_liStaticObjects;
	std::list< ATHObject* > m_liToRemove;

public:

	b2World* m_pWorld;

	ATHObjectManager();

	void Init();
	void InitBox2D();
	void Update( float _fDT );
	void Shutdown();

	// Object Management
	void AddObject( ATHObject* pObject );
	void AddObjectStatic( ATHObject* pObject );
	ATHObject* InstanceObject(float3 _fPos, char* _szName);
	void ClearObjects();

	// Collision functions
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);

	char* GetFileAsText(const char* _szPath);

	// Object Loading
	void LoadObjectsFromXML();
	void LoadObjLibFromXML();
	void LoadXML( const char* _szPath );

	// Object parsing
	ATHObject* GenerateObject(rapidxml::xml_node<>* pRootObjNode );
	void LoadProperties(ATHObject* _pLoadTarget, rapidxml::xml_node<>* _pXMLPropertiesNode);
	
	// Box2d Parsing
	b2Body* GenerateB2Body(rapidxml::xml_node<>* pXMLNode);
	b2Shape* GenerateB2PolygonShape(rapidxml::xml_node<>* pXMLShapeNode);
	b2Shape* GenerateB2CircleShape(rapidxml::xml_node<>* pXMLShapeNode);

	// Render parsing
	ATHRenderNode* GenerateRenderNode(rapidxml::xml_node<>* pXMLNode );


};

#endif;