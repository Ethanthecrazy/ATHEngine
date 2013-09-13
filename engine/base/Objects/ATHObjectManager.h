#ifndef ATHOBJECTMANAGER_H
#define ATHOBJECTMANAGER_H

class b2World;

class ATHObjectManager
{
private:

	float m_fTimeBuffer;
	b2World* m_pWorld;

public:

	ATHObjectManager();

	void Init();
	void Update( float _fDT );
	void Shutdown();
};

#endif;