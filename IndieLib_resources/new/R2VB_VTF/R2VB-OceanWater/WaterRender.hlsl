//--------------------------------------------------------------------------------------
// File: WaterRender.shd
// 
// Copyright (c) ATI Research 2006
//--------------------------------------------------------------------------------------

/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

[Vertex shader]

float4x4 g_mWorldViewProjection;    // World * View * Projection matrix
float4x4 g_mWorld;                  // World matrix


//--------------------------------------------------------------------------------------
// water vertex shader
//--------------------------------------------------------------------------------------
SAMPLER2D DMAP;
void main(  // Inputs
                float4 pos_    : POSITION,
                //float4 Normal : NORMAL,
                float2 Tex_    : TEXCOORD0,

                // Outputs
                out float4 oPos   : POSITION,
                out float2 oTex   : TEXCOORD0,
                out float3 oWSPos : TEXCOORD1 )
{
	float2 Tex=Tex_+0.5;
	float4 Pos = SAMPLE2D(DMAP, Tex);

    // Outputs
    oPos = mul(g_mWorldViewProjection, Pos);     // Clip-space position
    oTex = Tex;                                  // Pass-through texture coordinates
    oWSPos = mul(g_mWorld, Pos);                 // World-space position for per-pixel lighting
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
float g_useConstantColor;         // if non-zero, don't do lighting, just use the constant color

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
float4 main( float2 Tex : TEXCOORD0, float3 oWSPos : TEXCOORD1 ) : COLOR0
{

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

