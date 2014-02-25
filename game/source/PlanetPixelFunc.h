#ifndef PLANETPIXELFUNC_H
#define PLANETPIXELFUNC_H

#include "../../engine/ATHUtil/hDataTypes.h"
#include "../../engine/ATHUtil/ATHRand.h"
#include "../../engine/Box2D/Box2D.h"

float g_ppfOuterLimit = 0.501f * 0.501f;
float g_ppfInnerValue = 0.40f * 0.40f;
float g_ppfBorderValue = 0.497f * 0.497f;

void PlanetPixelFunc(float _fPosX, float _fPosY, float4* _pfTargetPixel, void* _pData)
{

	float4* fBaseColor = (float4*)_pData;

	if (_pfTargetPixel == nullptr)
	{
		delete fBaseColor;
		return;
	}

	b2Vec2 b2Pos(_fPosX - 0.5f, _fPosY - 0.5f);

	float fDist = b2Pos.LengthSquared();

	if (fDist <= g_ppfOuterLimit)
	{
		_pfTargetPixel->cA = 1.0f;
		_pfTargetPixel->cR = fBaseColor->cR - ATHRandom::FastRand(-0.2f, 0.0f); // 247
		_pfTargetPixel->cG = fBaseColor->cG - ATHRandom::FastRand(-0.2f, 0.0f);
		_pfTargetPixel->cB = fBaseColor->cB - ATHRandom::FastRand(-0.2f, 0.0f);

		if (fDist > g_ppfInnerValue)
		{
			if (fDist > g_ppfBorderValue)
			{
				_pfTargetPixel->cR = 0.0f;
				_pfTargetPixel->cG = 0.0f;
				_pfTargetPixel->cB = 0.0f;
			}
			else
			{
				_pfTargetPixel->cR *= 1.0f - 0.33f * (fDist - g_ppfInnerValue) / (g_ppfOuterLimit - g_ppfInnerValue);
				_pfTargetPixel->cG *= 1.0f - 0.33f * (fDist - g_ppfInnerValue) / (g_ppfOuterLimit - g_ppfInnerValue);
				_pfTargetPixel->cB *= 1.0f - 0.33f * (fDist - g_ppfInnerValue) / (g_ppfOuterLimit - g_ppfInnerValue);
			}
		}
	}
	else
	{
		_pfTargetPixel->cA = 0.0f;
		_pfTargetPixel->cR = 0.0f;
		_pfTargetPixel->cG = 0.0f;
		_pfTargetPixel->cB = 0.0f;
	}
}

#endif