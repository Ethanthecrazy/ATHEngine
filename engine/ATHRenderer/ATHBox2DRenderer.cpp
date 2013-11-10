#include "ATHBox2DRenderer.h"

/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "ATHRenderer.h"

void ATHBox2DRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	ATHRenderer* pRenderer = ATHRenderer::GetInstance();
	for (int32 i = 1; i < vertexCount; ++i)
	{
		pRenderer->DebugLinesAdd( float3( vertices[i].x, vertices[i].y, 0.0f ), float3( vertices[i-1].x, vertices[i-1].y, 0.0f ), float4( color.r, color.g, color.b, 1.0f ) );
	}

	pRenderer->DebugLinesAdd( float3( vertices[0].x, vertices[0].y, 0.0f ), float3( vertices[vertexCount-1].x, vertices[vertexCount-1].y, 0.0f ), float4( color.r, color.g, color.b, 1.0f ) );
}
//================================================================================
void ATHBox2DRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	ATHRenderer* pRenderer = ATHRenderer::GetInstance();
	for (int32 i = 1; i < vertexCount; ++i)
	{
		pRenderer->DebugLinesAdd( float3( vertices[i].x, vertices[i].y, 0.0f ), float3( vertices[i-1].x, vertices[i-1].y, 0.0f ), float4( color.r, color.g, color.b, 1.0f ) );
	}

	pRenderer->DebugLinesAdd( float3( vertices[0].x, vertices[0].y, 0.0f ), float3( vertices[vertexCount-1].x, vertices[vertexCount-1].y, 0.0f ), float4( color.r, color.g, color.b, 1.0f ) );

}
//================================================================================
void ATHBox2DRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		theta += k_increment;
	}
}
//================================================================================
void ATHBox2DRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{

}
//================================================================================
void ATHBox2DRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	ATHRenderer::GetInstance()->DebugLinesAdd( float3( p1.x, p1.y, 0.0f ), float3( p2.x, p2.y, 0.0f ), float4( color.r, color.b, color.g, 1.0f ) );
}
//================================================================================
void ATHBox2DRenderer::DrawTransform(const b2Transform& xf)
{

}
//================================================================================
void ATHBox2DRenderer::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{

}
//================================================================================
void ATHBox2DRenderer::DrawString(int x, int y, const char *string, ...)
{

}
//================================================================================
void ATHBox2DRenderer::DrawAABB(b2AABB* aabb, const b2Color& c)
{

}
//================================================================================