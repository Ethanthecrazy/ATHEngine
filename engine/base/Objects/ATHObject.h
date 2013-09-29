#ifndef ATHOBJECT_H
#define ATHOBJECT_H

#include <d3d9.h>
#include <list>
#include "../ATHUtil/hDataTypes.h"

class b2Body;
class ATHRenderNode;

class ATHObject
{
private:

	// If the object is not going to be destroyed
	bool m_bAlive;
	// If the object is currently being updated
	bool m_bActive;

	D3DMATRIX m_matTransform;
	ATHRenderNode* m_pRenderNode;
	b2Body* m_pBody;

public:
	
	bool GetAlive() { return m_bAlive; }
	void SetAlive( bool _bAlive ) { m_bAlive = _bAlive; }

	bool GetActive() { return m_bActive; }
	void SetAtive( bool _bActive ) { m_bActive = _bActive; }

	D3DMATRIX		GetTransform();
	ATHRenderNode*	GetRenderNode();
	b2Body*			GetBody();
	
	virtual void Init( ATHRenderNode* m_pRenderNode = nullptr, b2Body* m_pBody = nullptr );
	virtual void Update( float _fDT );
	virtual void Shutdown();


};

#endif