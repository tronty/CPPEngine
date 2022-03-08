// https://digitalerr0r.wordpress.com/2009/03/23/xna-shader-programming-tutorial-4-normal-mapping/
/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]
ROW_MAJOR float4x4 matWorldViewProj MVPSEMANTIC;   
ROW_MAJOR float4x4 matWorld;   
float4 vecLightDir;
float4 vecEye;

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex : TEXCOORD0;
    float3 Light : TEXCOORD1;
    float3 View : TEXCOORD2;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
      Out.Pos = mul(float4(IN.Position, 1), matWorldViewProj); // transform Position
      // Create tangent space to get normal and light to the same space.
      float3x3 worldToTangentSpace;
      worldToTangentSpace[0] = mul(normalize(float4(IN.Tangent,1)), matWorld).xyz;
      worldToTangentSpace[1] = mul(normalize(float4(IN.BiNormal,1)), matWorld).xyz;
      worldToTangentSpace[2] = mul(normalize(float4(IN.Normal,1)), matWorld).xyz;
      // Just pass textures trough
      Out.Tex = IN.uv;
      float4 PosWorld = mul(float4(IN.Position, 1), matWorld);
      // Pass out light and view directions, pre-normalized
      Out.Light = normalize(mul(worldToTangentSpace, vecLightDir.xyz));
      Out.View = normalize(mul(worldToTangentSpace, (vecEye-PosWorld).xyz));
      return Out;
}

[Fragment shader]

sampler2D ColorMapSampler; /*
sampler ColorMapSampler = sampler_state
{
   Texture = <ColorMap>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;  
   AddressU  = Clamp;
   AddressV  = Clamp;
}; */
 
sampler2D NormalMapSampler; /*
sampler NormalMapSampler = sampler_state
{
   Texture = <NormalMap>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;  
   AddressU  = Clamp;
   AddressV  = Clamp;
}; */

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex : TEXCOORD0;
    float3 Light : TEXCOORD1;
    float3 View : TEXCOORD2;
};
float4 main(VS_OUTPUT IN) : COLOR
{
      // Get the color from ColorMapSampler using the texture coordinates in Tex.
      float4 Color = tex2D(ColorMapSampler, IN.Tex);
      // Get the Color of the normal. The color describes the direction of the normal vector
      // and make it range from 0 to 1.
      float3 N = (2.0 * (tex2D(NormalMapSampler, IN.Tex)))-1.0;
      // diffuse
      float D = saturate(dot(N, IN.Light));
      // reflection
      float3 R = normalize(2 * D * N-IN.Light);
      // specular
      float S = pow(saturate(dot(R, IN.View)), 2);
      // calculate light (ambient + diffuse + specular)
      const float4 Ambient = float4(0.3, 0.3, 0.3, 1.0);
      return Color*Ambient + Color * D + Color*S;
}

