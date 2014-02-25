#include "ObjectGenerator.h"
#include "../../engine/ATHObjectSystem/ATHObjectManager.h"
#include "../../engine/Box2D/Box2D.h"
#include "../../engine/ATHUtil/ATHRand.h"
#include "../../engine/ATHRenderer/ATHRenderer.h"

#include "Classes\Planet.h"
#include "PlanetPixelFunc.h"

#include <iostream>

#define PLANET_RAD_MIN 1.0f
#define PLANET_RAD_MAX 10.0f
#define UNIT_TO_PIXEL_RATIO 128
unsigned int ObjectGenerator::s_unPlanetTextureCount = 0;

ObjectGenerator::ObjectGenerator()
{
	m_pObjectManager = nullptr;
}

void ObjectGenerator::Init(ATHObjectManager* _pObjectManager)
{
	m_pObjectManager = _pObjectManager;
}


ATHObject* ObjectGenerator::GeneratePlanet(float2 _fPos, float _fMinRadius, float _fMaxRadius, float3 _fColor)
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

	// Create the image?
	ATHRenderNode* pRenderNode = GeneratePlanetTexture(_fColor, fPlanetRadius);

	// Init the game object
	pNewObject->Init(pRenderNode, pPlanetBody);
	m_pObjectManager->AddObject(pNewObject);

	return pNewObject;
}

ATHRenderNode* ObjectGenerator::GeneratePlanetTexture(float3 _fBaseColor, float _fRadius)
{
	unsigned int unWidth = (unsigned int)(_fRadius * UNIT_TO_PIXEL_RATIO) + 1;
	unsigned int unHeight = (unsigned int)(_fRadius * UNIT_TO_PIXEL_RATIO) + 1;

	char szNumberBuff[32];
	_itoa_s(s_unPlanetTextureCount, szNumberBuff, 32, 10);

	std::string strPlanetTexHandle("PlanetTexture_");
	strPlanetTexHandle += szNumberBuff;
	s_unPlanetTextureCount += 1;

	ATHAtlas::ATHTextureHandle athTExHandle = ATHRenderer::GetInstance()->
		GetAtlas()->GenerateTexture(strPlanetTexHandle.c_str(), 
		unWidth, 
		unHeight, 
		PlanetPixelFunc, 
		new float4( ATHRandom::Rand(0.0f, 1.0f), ATHRandom::Rand(0.0f, 1.0f), ATHRandom::Rand(0.0f, 1.0f), 1.0f ) );

	if (!athTExHandle.Valid())
		std::cout << "Error: Failed to create Planet texture.\n";

	// Create the render object
	ATHRenderNode* pRenderNode = ATHRenderer::GetInstance()->CreateRenderNode("Texture", 0);

	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, _fRadius * 2.0f, _fRadius * 2.0f, 1.0f);
	pRenderNode->SetLocalTransform(matScale);

	pRenderNode->SetTexture(athTExHandle);
	pRenderNode->SetMesh(&ATHRenderer::GetInstance()->m_Quad);

	return pRenderNode;
}