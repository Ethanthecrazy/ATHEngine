#ifndef ATHOBJECTMANAGER_H
#define ATHOBJECTMANAGER_H

#include <list>

class b2World;
class ATHObject;

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
};

#endif;