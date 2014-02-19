#include "ObjectGenerator.h"
#include "../../engine/ATHObjectSystem/ATHObjectManager.h"
#include "../../engine/Box2D/Box2D.h"
#include "../../engine/ATHUtil/ATHRand.h"
#include "Classes\Planet.h"

#define PLANET_RAD_MIN 1.0f
#define PLANET_RAD_MAX 10.0f;

ObjectGenerator::ObjectGenerator()
{
	m_pObjectManager = nullptr;
}

void ObjectGenerator::Init(ATHObjectManager* _pObjectManager)
{
	m_pObjectManager = _pObjectManager;
}

ATHObject* ObjectGenerator::GeneratePlanet(float2 _fPos, float _fMinRadius, float _fMaxRadius)
{

	// The new Object
	Planet* pNewObject = new Planet();

	// Decide stats
	float fPlanetRadius = ATHRandom::Rand(_fMinRadius, _fMaxRadius);
	pNewObject->SetProperty("radius", &fPlanetRadius, APT_FLOAT);

	// Set the mass
	float fDensity = 1.0f;
	float fArea = b2_pi * fPlanetRadius * fPlanetRadius;
	pNewObject->SetMass(fArea * fDensity);

	// Create the b2Body
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(_fPos.vX, _fPos.vY);
	bodyDef.type = b2_kinematicBody;
	
	b2Body* pPlanetBody = m_pObjectManager->m_pWorld->CreateBody(&bodyDef);

	// Create the planet fixture
	b2FixtureDef fixtureDef;
	b2CircleShape planetCircleShape;

	planetCircleShape.m_radius = fPlanetRadius;
	fixtureDef.shape = &planetCircleShape;
	pPlanetBody->CreateFixture(&fixtureDef);

	// Create the gravity sensor
	planetCircleShape.m_radius = fPlanetRadius * 5.0f;
	fixtureDef.shape = &planetCircleShape;
	fixtureDef.isSensor = true;
	pPlanetBody->CreateFixture(&fixtureDef);
	pNewObject->SetProperty("gravity-radius", &planetCircleShape.m_radius, APT_FLOAT);

	// Create the render object

	// Create the image?

	pNewObject->Init(nullptr, pPlanetBody);
	m_pObjectManager->AddObject(pNewObject);

	return pNewObject;
}