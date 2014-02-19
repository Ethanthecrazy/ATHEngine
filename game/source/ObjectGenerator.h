#ifndef OBJECTGENERATOR_H
#define OBJECTGENERATOR_H

#include "../../engine/ATHUtil/hDataTypes.h"

class ATHObject;
class ATHObjectManager;
class ObjectGenerator
{
private:

	ATHObjectManager* m_pObjectManager;

public:

	ObjectGenerator();

	void Init(ATHObjectManager* _pObjectManager);

	ATHObject* GeneratePlanet( float2 _fPos, float _fMinRadius, float _fMaxRadius );
};

#endif