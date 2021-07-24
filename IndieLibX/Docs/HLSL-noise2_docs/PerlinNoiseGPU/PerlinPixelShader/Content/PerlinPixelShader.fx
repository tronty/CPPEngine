#include "inoise.fxh"

float4 PS_Perlin(float2 p : TEXCOORD): COLOR
{	
    // scale the input coordinates
	float3 pos = float3(p * 10, 0);
	// convert the result from [-1, +1] to [0, +1]
	return inoise(pos)*0.5+0.5;	
}

float4 PS_fBm(float2 p : TEXCOORD): COLOR
{
    // scale the input coordinates
	float3 pos = float3(p * 10, 0);
	// convert the result from [-1, +1] to [0, +1]
	return fBm(pos, 12)*0.5+0.5;
}

technique Perlin
{
    pass p0 
    {
		PixelShader  = compile ps_3_0 PS_Perlin();
    }
}

technique fBm
{
    pass p0 
    {
		PixelShader  = compile ps_3_0 PS_fBm();
    }
}