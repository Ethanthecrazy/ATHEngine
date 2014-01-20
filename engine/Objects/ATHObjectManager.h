#ifndef ATHOBJECTMANAGER_H
#define ATHOBJECTMANAGER_H

#include <list>
#include "RapidXML\rapidxml.hpp"

class b2World;
class b2Body;
class ATHRenderNode;
class ATHObject;
class b2Shape;


class ATHObjectManager
{
private:

	float m_fTimeBuffer;

	std::list<ATHObject*> m_liObjects;
	std::list<ATHObject*> m_liStaticObjects;
	std::list< ATHObject* > m_liToRemove;

public:

	b2World* m_pWorld;

	ATHObjectManager();

	void Init();
	void Update( float _fDT );
	void Shutdown();

	void AddObject( ATHObject* pObject );
	void AddObjectStatic( ATHObject* pObject );

	void ClearObjects();

	void LoadObjectsFromXML();
	void LoadXML( const char* _szPath );

	b2Body* GenerateB2Body(rapidxml::xml_node<>* pXMLNode);
	b2Shape* GenerateB2PolygonShape(rapidxml::xml_node<>* pXMLShapeNode);
	b2Shape* GenerateB2CircleShape(rapidxml::xml_node<>* pXMLShapeNode);

	ATHRenderNode* GenerateRenderNode(rapidxml::xml_node<>* pXMLNode );
};

#endif;