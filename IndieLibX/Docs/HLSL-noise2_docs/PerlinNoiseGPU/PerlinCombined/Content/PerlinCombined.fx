#include "inoise.fxh"

float4x4 World;
float4x4 View;
float4x4 Projection;

bool enableLighting = true;
float4 lightDirection = {1,-0.7,1,0};

texture sandscale;
sampler sandscaleSampler = sampler_state 
{
    texture = <sandscale>;
    AddressU  = Wrap;        
    AddressV  = Wrap;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;   
};

texture grassscale;
sampler grassscaleSampler = sampler_state 
{
    texture = <grassscale>;
    AddressU  = Wrap;        
    AddressV  = Wrap;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;   
};

texture rockscale;
sampler rockscaleSampler = sampler_state 
{
    texture = <rockscale>;
    AddressU  = Wrap;        
    AddressV  = Wrap;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;   
};

texture snowscale;
sampler snowscaleSampler = sampler_state 
{
    texture = <snowscale>;
    AddressU  = Wrap;        
    AddressV  = Wrap;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;   
};

struct VertexShaderInput
{
    float3 Position : POSITION0;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
    float4 TexWeights : TEXCOORD0;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // calc the height displacement using 12 octaves of fBm
    float heightValue = fBm(input.Position.xyz, 12);
    
    // add the height displacement to the vertex position
    input.Position.xyz = input.Position.xyz + heightValue;
    
    // multiply by the WVP matrices
    float4 worldPosition = mul(float4(input.Position, 1), World);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);
    
    // calc the normal of the vertex
    output.Normal = normalize(input.Position.xyz);
    
    // calc texture weights that are based upon the height value ranging from 0-1
    output.TexWeights = 0;
    output.TexWeights.x = saturate( 1.0f - abs(heightValue - 0.0));
    output.TexWeights.y = saturate( 1.0f - abs(heightValue - 0.3));
    output.TexWeights.z = saturate( 1.0f - abs(heightValue - 0.6));
    output.TexWeights.w = saturate( 1.0f - abs(heightValue - 0.9));
    float totalWeight = output.TexWeights.x + output.TexWeights.y + output.TexWeights.z + output.TexWeights.w;
    output.TexWeights /= totalWeight;

    return output;
}

// calculate gradient of fBm noise (very expensive!)
float3 fBmGradient(float3 pos, float delta, float octaves)
{
	float f0 = fBm(pos, octaves);
	float fx = fBm(pos + float3(delta, 0, 0), octaves);	
	float fy = fBm(pos + float3(0, delta, 0), octaves);
	float fz = fBm(pos + float3(0, 0, delta), octaves);
	return float3(fx - f0, fy - f0, fz - f0) / delta;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	// calc 4 colors using Perlin noise lookups into gradient textures
    // then blend those colors together based on the terrain height
    float noiseResult1 = turbulence(input.Normal*10, 2);
    float4 noiseColor1 = tex1D(sandscaleSampler, saturate(noiseResult1));
    float noiseResult2 = turbulence(input.Normal*10, 2);
    float4 noiseColor2 = tex1D(grassscaleSampler, saturate(noiseResult2));
    float noiseResult3 = turbulence(input.Normal*10, 2);
    float4 noiseColor3 = tex1D(rockscaleSampler, saturate(noiseResult3));
    float noiseResult4 = turbulence(input.Normal*10, 2);
    float4 noiseColor4 = tex1D(snowscaleSampler, saturate(noiseResult4));
    float4 resultColor = noiseColor1 * input.TexWeights.x + noiseColor2 * input.TexWeights.y + noiseColor3 * input.TexWeights.z + noiseColor4 * input.TexWeights.w;

	// lighting calculation
	if (enableLighting)
	{
		// use Perlin Noise gradient to calc each pixel's normal vector
		input.Normal = normalize(input.Normal - fBmGradient(input.Normal, 0.000001, 12));
		
	    float lightingFactor = saturate(dot(input.Normal, -lightDirection));
	    resultColor = resultColor * lightingFactor;
	}
	    
	return resultColor;
}

technique Combined
{
    pass Pass1
    {
        VertexShader = compile vs_2_0 VertexShaderFunction();
        PixelShader = compile ps_2_0 PixelShaderFunction();
    }
}
