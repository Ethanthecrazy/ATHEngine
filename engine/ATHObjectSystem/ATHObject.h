#ifndef ATHOBJECT_H
#define ATHOBJECT_H

#include <d3dx9.h>
#include <string>
#include <list>
#include <map>
#include "ATHProperty.h"
#include "../../engine/ATHEventSystem/ATHEventListener.h"
#include "../ATHUtil/hDataTypes.h"
static const unsigned int ATHOBJECT_MAX_NAME_LENGTH = 64;

class b2Body;
class ATHRenderNode;

class ATHObject : public ATHEventListener
{
private:

	
	static unsigned int s_unIdCounter;
	// Unique instance ID
	unsigned int m_unID;

	// Name
	std::string m_strName;

	// If the object is not going to be destroyed
	bool m_bAlive;
	// If the object is currently being updated
	bool m_bActive;

	D3DXMATRIX m_matTransform;
	ATHRenderNode* m_pRenderNode;
	b2Body* m_pBody;

	std::map< std::string, ATHProperty > m_mapProperties;

public:
	
	ATHObject();
	~ATHObject();

	const std::string GetName() { return m_strName; }

	bool GetAlive() { return m_bAlive; }
	void SetAlive( bool _bAlive ) { m_bAlive = _bAlive; }

	bool GetActive() { return m_bActive; }
	void SetActive( bool _bActive ) { m_bActive = _bActive; }

	D3DXMATRIX		GetTransform();
	ATHRenderNode*	GetRenderNode();
	b2Body*			GetBody();
	
	virtual void Init( ATHRenderNode* m_pRenderNode = nullptr, b2Body* m_pBody = nullptr );
	virtual void Update( float _fDT );
	virtual void HandleEvent( const ATHEvent* _pEvent ){}

	virtual void SetPosition(float3 _fPos);

	void SetProperty(char* _szName, void* pData, ATHPropertyType _type, unsigned int _szSize = 0);
	int GetPropertyAsInt(char* _szName);
	float GetPropertyAsFloat(char* _szName);
	std::string GetPropertyAsString(char* _szName);

	friend class ATHObjectManager;
};

#endif