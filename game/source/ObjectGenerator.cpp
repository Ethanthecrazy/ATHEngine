#include "ObjectGenerator.h"
#include "../../engine/ATHObjectSystem/ATHObjectManager.h"
#include "../../engine/Box2D/Box2D.h"
#include "../../engine/ATHUtil/ATHRand.h"
#include "../../engine/ATHRenderer/ATHRenderer.h"
#include "Classes\Planet.h"

#include <iostream>

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

	std::cout << "Radius: " << fPlanetRadius << "\n";

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

	

	struct sPixel
	{
		float R;
		float G;
		float B;
		float A;
	};

	// Create the image?
	unsigned int unWidth = 512;
	unsigned int unHeight = 512;
	unsigned int unPixelCount = unWidth * unHeight;

	unsigned int PosX = 0;
	unsigned int PosY = 0;
	sPixel* pPixels = new sPixel[unPixelCount];
	float fPixelWidth = 1.0f / unWidth;
	float fPixelHeight = 1.0f / unHeight;

	for (unsigned int i = 0; i < (unPixelCount); ++i)
	{
		b2Vec2 b2Center(0.5f, 0.5f);
		b2Vec2 b2Pos( PosX / (float)unWidth + fPixelWidth / 2.0f, PosY / (float)unHeight + fPixelHeight / 2.0f);

		float fDist = ( b2Pos - b2Center ).Length();
		if (fDist <= 0.501f)
		{
			pPixels[i].A = 1.0f;
			pPixels[i].R = 0.864f - fDist + 0.2f; // 247
			pPixels[i].G = 0.339f + ATHRandom::Rand(-0.1f, 0.0f); // 138
			pPixels[i].B = 0.177f + ATHRandom::Rand(-0.1f, 0.0f); // 71

			if (fDist > 0.48f)
			{
				pPixels[i].R -= (fDist - 0.48f) * 8.0f;
				pPixels[i].G -= (fDist - 0.48f) * 8.0f;
				pPixels[i].B -= (fDist - 0.48f) * 8.0f;
			}
		}
		else
		{
			pPixels[i].A = 0.0f;
			pPixels[i].R = 0.0f;
			pPixels[i].G = 0.0f;
			pPixels[i].B = 0.0f;
		}
		
		PosX++;
		if (PosX >= unWidth)
		{
			PosX = 0;
			PosY++;
		}
	}

	static unsigned int unPlanetTextureCount = 0;
	std::string strPlanetTexHandle("PlanetTexture_");
	strPlanetTexHandle += unPlanetTextureCount;
	unPlanetTextureCount += 1;

	ATHRenderer::GetInstance()->
		GetAtlas()->
		LoadTextureFromData(strPlanetTexHandle.c_str(),
		unWidth,
		unHeight,
		(void*)pPixels,
		sizeof(sPixel)* unPixelCount);

	delete pPixels;

	// Create the render object
	ATHRenderNode* pRenderNode = ATHRenderer::GetInstance()->CreateRenderNode("Texture", 0);

	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, fPlanetRadius * 2.0f, fPlanetRadius * 2.0f, 1.0f);
	pRenderNode->SetLocalTransform(matScale);

	pRenderNode->SetTexture(ATHRenderer::GetInstance()->GetAtlas()->GetTexture(strPlanetTexHandle.c_str()));
	pRenderNode->SetMesh(&ATHRenderer::GetInstance()->m_Quad);

	// Init the game object
	pNewObject->Init(pRenderNode, pPlanetBody);
	m_pObjectManager->AddObject(pNewObject);

	return pNewObject;
}