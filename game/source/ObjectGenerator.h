#ifndef OBJECTGENERATOR_H
#define OBJECTGENERATOR_H

#include "../../engine/ATHUtil/hDataTypes.h"
#include <vector>

class ATHObject;
class ATHObjectManager;
class ATHRenderNode;
class ObjectGenerator
{
private:

	ATHObjectManager* m_pObjectManager;
	std::vector<unsigned int> m_vecPlanetSlotBag;
	static unsigned int s_unPlanetTextureCount;

public:

	ObjectGenerator();

	void Init(ATHObjectManager* _pObjectManager);
	void RefreshSlotBag();
	unsigned int GetNextPlanetSlotCount();

	ATHObject* GeneratePlanet( float2 _fPos, float _fMinRadius, float _fMaxRadius, float3 _fColor );
	ATHRenderNode* GeneratePlanetTexture(float3 _fBaseColor, float _fRadius);
};

#endif