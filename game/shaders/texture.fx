/*****************************************
* Texture.fx - handles a single texture
******************************************/

// globals
float4x4	gWVP;
texture		tex1;
texture		tex2;
float2		xAxis;
float2		yAxis;
float4		multColor;
float2		LightPos;
float		LightIntensity;	
float2		texTopLeft;
float2		texScale;

// texture sampler
sampler		samp1
{
	Texture = <tex1>;
	// texture filtering
	MinFilter = LINEAR;
	MagFilter = POINT;
	MipFilter = LINEAR;
	// texture address modes (available options are WRAP, MIRROR, BORDER, and CLAMP)
	AddressU  = CLAMP;
	AddressV  = CLAMP;
};

sampler		samp2
{
	Texture = <tex2>;
	// texture filtering
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	MipFilter = ANISOTROPIC;
	// texture address modes (available options are WRAP, MIRROR, BORDER, and CLAMP)
	AddressU  = WRAP;
	AddressV  = WRAP;
};
// avilable filter options are NONE, POINT (Nearest Neighbor), LINEAR (average 2x2), and ANIOSOTROPIC. 
	
// Define a vertex shader output structure;
struct OutputVS
{
    float4 posH:POSITION0;
    float2 texCoord0:TEXCOORD0;
};

// Vertex shader -------------------------------------------------------------------------------------
OutputVS TransformVS(float3 position:POSITION0, float4 norm:NORMAL0, float2 uv0:TEXCOORD0)
{
	// output structure
	OutputVS outVS;
	
	// create the transformed vertex position
	outVS.posH = mul(float4(position, 1.0f), gWVP);	
	
	// pass through the texture coordinates
	outVS.texCoord0 = uv0;

    return outVS;
}

// Pixel Shader ---------------------------------------------------------------------------
float4 TransformPS( float2 uv0:TEXCOORD0):COLOR
{
	//return c;
	
	// determine the pixel to sample from
    float4 texCol = tex2D(samp1, uv0);

	if( texCol.a == 0 )
		//discard;
		texCol = float4( 1.0f, 1.0f, 1.0f, 1.0f );
    
    return texCol;
}

float4 sourceRectPS(float4 c:COLOR0, float2 uv0:TEXCOORD0):COLOR
{

	if( uv0.x < xAxis.x )
	discard;

	if( uv0.x > xAxis.y )
	discard; 

	if( uv0.y < yAxis.x )
	discard;

	if( uv0.y > yAxis.y )
	discard; 

	// determine the pixel to sample from
    float4 texCol = tex2D(samp1, uv0);

	if( texCol.a == 0 )
		discard;

	texCol *= multColor;
	//texCol *= _len;
    
    return texCol;
} 

float4 texRectPS(float2 uv0:TEXCOORD0):COLOR
{
	// Put the UV into the other textures space
	float2 newUV;
	newUV.x = texTopLeft.x + ( uv0.x * texScale.x );
	newUV.y = texTopLeft.y + ( uv0.y * texScale.y );

    return tex2D(samp1, newUV);
} 

float4 lightingPS( float4 c:COLOR0, float2 uv0:TEXCOORD0):COLOR
{
	float4 genCol = multColor;
	float multVal = 1.0f - length( float2( 0.5f, 0.5f ) - uv0 ) / 0.5f;
	multVal += multVal * 0.25f;
	saturate( multVal );

	return genCol * multVal;
}

float4 multiplyPS( float4 c:COLOR0, float2 uv0:TEXCOORD0):COLOR
{
	float4 texCol = tex2D(samp1, uv0);
	float4 texColT = tex2D(samp2, uv0);
	float4 result = texCol * texColT;
	return result;
}

float4 BlackRectPS(float4 c:COLOR0, float2 uv0:TEXCOORD0):COLOR
{

	if( uv0.x < xAxis.x )
	discard;

	if( uv0.x > xAxis.y )
	discard; 

	if( uv0.y < yAxis.x )
	discard;

	if( uv0.y > yAxis.y )
	discard; 

	// determine the pixel to sample from
    float4 texCol = tex2D(samp1, uv0);

	if( texCol.a == 0 )
		discard;

    return float4( 0.0f, 0.0f, 0.0f, 1.0f );
} 
// ----------------------------------------------------------------------------------------
technique Default
{
    pass P0
    {
        vertexShader = compile vs_3_0 TransformVS();
        pixelShader  = compile ps_3_0 TransformPS();
 
		// (srcColor * srcBlend) + (destColor * destBlend)
		// turn on alpha testing     
		// AlphaBlendEnable = true;
		SrcBlend = srcAlpha;
		//DestBlend = One;

		/*AlphaTestEnable = true;
		// if alpha is greater than ref, then draw
		// other options are Never, Equal, GreaterEqual, Always. More can be found in the sdk docs
		// AlphaFunc = GreaterEqual;
		
		// what alpha value to look for
		AlphaRef = 0.0f;*/
		
		//ShadeMode = Gouraud;
        //FillMode = Wireframe;
        //CullMode = NONE;
    }
}

technique SourceRect
{
	pass P0
	{
		vertexShader = compile vs_3_0 TransformVS();
        pixelShader  = compile ps_3_0 sourceRectPS();

		SrcBlend = srcAlpha;
		ShadeMode = Gouraud;
        //FillMode = Wireframe;
	}
}

technique RenderLight
{
	pass P0
	{
		vertexShader = compile vs_3_0 TransformVS();
		pixelShader  = compile ps_3_0 lightingPS();

		SrcBlend = srcAlpha;
		DestBlend = One;
		ShadeMode = Gouraud;
        //FillMode = Wireframe;
	}
}

technique MergeTex
{
	pass P0
	{
		vertexShader = compile vs_3_0 TransformVS();
		pixelShader  = compile ps_3_0 multiplyPS();
		SrcBlend = srcAlpha;
	}
}

technique BlackRect
{
	pass P0
	{
		vertexShader = compile vs_3_0 TransformVS();
        pixelShader  = compile ps_3_0 BlackRectPS();

		SrcBlend = srcAlpha;
	}
}

technique TexRect
{
	pass P0
	{
		vertexShader = compile vs_3_0 TransformVS();
        pixelShader  = compile ps_3_0 texRectPS();

		SrcBlend = srcAlpha;
	}
}
