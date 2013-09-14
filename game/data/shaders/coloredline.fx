// globals
float4x4	gWVP;

// Define a vertex shader output structure;
struct OutputVS
{
    float4 posH:POSITION0;
    float4 color0:COLOR0;
};

// Vertex shader -------------------------------------------------------------------------------------
OutputVS TransformVS(float3 position:POSITION0, float4 color:COLOR0)
{
	// output structure
	OutputVS outVS;
	
	// create the transformed vertex position
	outVS.posH = mul(float4(position, 1.0f), gWVP);	
	
	outVS.color0 = color;
	 
    return outVS;
}

// Pixel Shader ---------------------------------------------------------------------------
float4 TransformPS( float4 uv0:COLOR0):COLOR
{
    return uv0;
}
// ----------------------------------------------------------------------------------------
technique Default
{
    pass P0
    {
        vertexShader = compile vs_3_0 TransformVS();
        pixelShader  = compile ps_3_0 TransformPS();
    }
}