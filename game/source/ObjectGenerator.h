#ifndef OBJECTGENERATOR_H
#define OBJECTGENERATOR_H

#include "../../engine/ATHUtil/hDataTypes.h"

class ATHObject;
class ATHObjectManager;
class ATHRenderNode;
class ObjectGenerator
{
private:

	ATHObjectManager* m_pObjectManager;
	static unsigned int s_unPlanetTextureCount;

public:

	ObjectGenerator();

	void Init(ATHObjectManager* _pObjectManager);
	void Update();

	ATHObject* GeneratePlanet( float2 _fPos, float _fMinRadius, float _fMaxRadius, float3 _fColor );
	ATHRenderNode* GeneratePlanetTexture(float3 _fBaseColor, float _fRadius);
};

#endif