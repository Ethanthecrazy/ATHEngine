#ifndef ATHOBJECT_H
#define ATHOBJECT_H

#include <d3dx9.h>
#include <list>
#include "../ATHUtil/hDataTypes.h"

static const unsigned int ATHOBJECT_MAX_NAME_LENGTH = 64;

class b2Body;
class ATHRenderNode;

class ATHObject
{
private:

	// Unique instance ID
	unsigned int m_unID;

	//Name
	char m_szName[ ATHOBJECT_MAX_NAME_LENGTH ];

	// If the object is not going to be destroyed
	bool m_bAlive;
	// If the object is currently being updated
	bool m_bActive;

	D3DXMATRIX m_matTransform;
	ATHRenderNode* m_pRenderNode;
	b2Body* m_pBody;

public:
	
	ATHObject();
	~ATHObject();

	bool GetAlive() { return m_bAlive; }
	void SetAlive( bool _bAlive ) { m_bAlive = _bAlive; }

	bool GetActive() { return m_bActive; }
	void SetActive( bool _bActive ) { m_bActive = _bActive; }

	D3DXMATRIX		GetTransform();
	ATHRenderNode*	GetRenderNode();
	b2Body*			GetBody();
	
	virtual void Init( ATHRenderNode* m_pRenderNode = nullptr, b2Body* m_pBody = nullptr );
	virtual void Update( float _fDT );


};

#endif