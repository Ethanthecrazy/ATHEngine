#include "ObjectGenerator.h"
#include "../../engine/ATHObjectSystem/ATHObjectManager.h"
#include "../../engine/Box2D/Box2D.h"
#include "../../engine/ATHUtil/ATHRand.h"
#include "../../engine/ATHRenderer/ATHRenderer.h"
#include "Classes\Planet.h"

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
	unsigned int unPixelCount = unWidth * unHeight;

	unsigned int PosX = 0;
	unsigned int PosY = 0;
	float4* pPixels = new float4[unPixelCount];

	float fPixelWidth = 1.0f / unWidth;
	float fPixelHeight = 1.0f / unHeight;
	b2Vec2 b2Center(0.5f, 0.5f);
	float fOuterLimit = 0.501f * 0.501f;
	float fInnerValue = 0.40f * 0.40f;
	float fBorderValue = 0.497f * 0.497f;

	for (unsigned int i = 0; i < (unPixelCount); ++i)
	{
		b2Vec2 b2Pos(PosX / (float)unWidth + fPixelWidth / 2.0f, PosY / (float)unHeight + fPixelHeight / 2.0f);

		float fDist = (b2Pos - b2Center).LengthSquared();
		
		if (fDist <= fOuterLimit)
		{
			pPixels[i].cA = 1.0f;
			pPixels[i].cR = _fBaseColor.cR + ATHRandom::Rand(-0.1f, 0.0f); // 247
			pPixels[i].cG = _fBaseColor.cG + ATHRandom::Rand(-0.1f, 0.0f); // 138
			pPixels[i].cB = _fBaseColor.cB + ATHRandom::Rand(-0.1f, 0.0f); // 71

			if (fDist > fInnerValue )
			{
				if (fDist > fBorderValue)
				{
					pPixels[i].cR = 0.0f;
					pPixels[i].cG = 0.0f;
					pPixels[i].cB = 0.0f;
				}
				else
				{
					pPixels[i].cR *= 1.0f - 0.33f * (fDist - fInnerValue) / (fOuterLimit - fInnerValue);
					pPixels[i].cG *= 1.0f - 0.33f * (fDist - fInnerValue) / (fOuterLimit - fInnerValue);
					pPixels[i].cB *= 1.0f - 0.33f * (fDist - fInnerValue) / (fOuterLimit - fInnerValue);
				}
			}
		}
		else
		{
			pPixels[i].cA = 0.0f;
			pPixels[i].cR = 0.0f;
			pPixels[i].cG = 0.0f;
			pPixels[i].cB = 0.0f;
		}

		PosX++;
		if (PosX >= unWidth)
		{
			PosX = 0;
			PosY++;
		}
	}
	
	char szNumberBuff[32];
	_itoa_s(s_unPlanetTextureCount, szNumberBuff, 32, 10);

	std::string strPlanetTexHandle("PlanetTexture_");
	strPlanetTexHandle += szNumberBuff;
	s_unPlanetTextureCount += 1;

	ATHAtlas::ATHTextureHandle athTExHandle = ATHRenderer::GetInstance()->
		GetAtlas()->
		LoadTextureFromData(strPlanetTexHandle.c_str(),
		unWidth,
		unHeight,
		(void*)pPixels,
		sizeof(float4)* unPixelCount);

	delete pPixels;

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