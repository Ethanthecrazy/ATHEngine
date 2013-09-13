#include "CommonShader.fx"

#ifndef GBUFFER_FX
#define GBUFFER_FX

float fFrustrumLength;

///WRITE OUT THE DEPTH BUFFER
DepthVertexOut DepthCamVertex(float3 position : POSITION0, 
	float3 normal : NORMAL, float3 tangent : TANGENT0, float2 texCoord : TEXCOORD0)
{
	DepthVertexOut depthOut;
	depthOut.position = mul(float4(position,1.0), gWVP );
	depthOut.depth.xy = depthOut.position.zw;
	return depthOut;
}

float4 DepthCamFragment( DepthVertexOut inVertex ) : COLOR
{
	return inVertex.depth.x / fFrustrumLength;
}


technique DepthPass
{
	pass P0
	{
		vertexShader	= compile vs_3_0 DepthCamVertex();
		pixelShader		= compile ps_3_0 DepthCamFragment();

		CullMode = CCW;
	}
}























#endif //GBUFFER_FX