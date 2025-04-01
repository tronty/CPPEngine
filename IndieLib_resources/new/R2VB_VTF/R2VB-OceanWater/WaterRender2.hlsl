//--------------------------------------------------------------------------------------
// File: WaterRender.shd
// 
// Copyright (c) ATI Research 2006
//--------------------------------------------------------------------------------------

/*
  Copyright (c) 2022 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 Pos   : POSITION;      // Clip-space position
    float2 Tex   : TEXCOORD0;     // Pass-through texture coordinates
    float3 WSPos : TEXCOORD1;     // World-space position for per-pixel lighting
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

[Vertex shader]
ROW_MAJOR float4x4 g_mWorldViewProjection MVPSEMANTIC;    // World * View * Projection matrix
ROW_MAJOR float4x4 g_mWorld MVPSEMANTIC;                  // World matrix
//--------------------------------------------------------------------------------------
// water vertex shader
//--------------------------------------------------------------------------------------
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	float4 ip=float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.Pos = ip;
	Out.Tex.x = In.uv.x;Out.Tex.y = 1.0-In.uv.y;
	float4 pos4=mul(g_mWorld, ip);
	Out.WSPos = pos4.xyz;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	float4 ip=float4(In.position, 1.0);
	Out.Pos = mul(g_mWorldViewProjection, ip);
	Out.Tex.x = In.uv.x;Out.Tex.y = 1.0-In.uv.y;
	float4 pos4=mul(g_mWorld, ip);
	Out.WSPos = pos4.xyz;
	return Out;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

[Fragment shader]

static float4 g_cDownWaterColor = {42.0f/255.0f, 56.0f/255.0f, 76.0f/255.0f,  1.0f};
static float4 g_cUpWaterColor   = {53.0f/255.0f, 99.0f/255.0f, 97.0f/255.0f,  1.0f};
static float4 g_cSunColor = {251.0f/255.0f, 180.0f/255.0f, 90.0f/255.0f, 1.0f};
static float3 g_vSunDirection = {-1.0f, 0.0f, -1.0f};
static float4 g_cConstantColor = {0.0f, 255.0f, 0.0f, 1.0f};
float4 g_vWSEyePos;               // Eye position in world space
float g_useConstantColor=1.0;         // if non-zero, don't do lighting, just use the constant color

// Samplers

sampler2D HighFreqWaterNormalSampler;
samplerCUBE SkySampler;

//--------------------------------------------------------------------------------------
// bx2
//--------------------------------------------------------------------------------------
float3 bx2(float3 x)
{
    return 2.0f * x - 1.0f;
}


//--------------------------------------------------------------------------------------
// water pixel shader
//--------------------------------------------------------------------------------------
float4 main(VsOut In) : COLOR0
{
	return g_cUpWaterColor;
	return g_cDownWaterColor;
	float2 Tex=In.Tex;
	float3 oWSPos=In.WSPos;

    if (g_useConstantColor > 0)
    {
        return g_cConstantColor;
    }
    else
    {
        // Sample the normals
        float3 vNormal = normalize(bx2(tex2D(HighFreqWaterNormalSampler, Tex ).xyz));

        // View vector
        float3 vView = normalize(g_vWSEyePos.xyz - oWSPos);

        // Fresnel term
        float fFresnel = 1-(saturate(dot(vView, vNormal)));
        fFresnel = 0.0204f + 0.9796 * (fFresnel * fFresnel * fFresnel * fFresnel * fFresnel);

        // Reflect view through normal and sample the environment map
        float3 vReflect = reflect(-vView, vNormal).zxy;
        vReflect.z = saturate(vReflect.z + 0.15f); // push the vector up a bit so that it doesn't sample the junk near the horizon

        float4 cEnvMap = texCUBE(SkySampler, vReflect);

        // Separate sun term using Phong
        float sunPower = 100.0f;
        float4 cSun = fFresnel * pow(saturate(dot(vReflect, normalize(g_vSunDirection))), sunPower) * g_cSunColor;

        // Light scattered up from within water
        // Simply blend between darker bluish water for downward angle and lighter greenish water for shallower angle
        float4 cWaterColor = lerp( g_cUpWaterColor, g_cDownWaterColor, saturate(dot(vView, vNormal)));

        float4 cColor = lerp(cWaterColor, cEnvMap, fFresnel);

        return cColor + cSun * fFresnel;
    }
}

