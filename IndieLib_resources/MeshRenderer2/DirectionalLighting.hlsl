#define CONST
// Example code for the OpenGL Shading Language Cookbook - 2nd Edition (3rd Edition now available) 
// https://github.com/daw42/glslcookbook
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Color : TEXCOORD0;
};

[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;

struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

// Normalized direction towards light source in eye coords.
CONST float4 LightPosition=float4(1.0, 0.0, 0.0, 0.0);
CONST float3 LightIntensity=float3(0.8,0.8,0.8);
CONST float3 Kd=float3(0.8f, 0.8f, 0.8f);
CONST float3 Ks=float3(0.9f, 0.9f, 0.9f);
CONST float3 Ka=float3(0.1f, 0.1f, 0.1f);
CONST float Shininess=180.0;

float3 ads( float4 position, float3 norm )
{
    float3 s;
    if( LightPosition.w == 0.0 )
      s = normalize(LightPosition.xyz);
    else
      s = normalize( LightPosition.xyz - position.xyz );

    float3 v = normalize(-position.xyz);
    float3 r = reflect( -s, norm );

    return
        LightIntensity * ( Ka +
           Kd * max( dot(s, norm), 0.0 ) +
           Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4 Pos = float4(IN.Pos,1);
	float4 Pos2 = mul(Pos, WorldViewProjection);
    float3 eyeNorm = normalize( mul(Pos, WorldViewProjection)).xyz;

    // Evaluate the lighting equation
    Out.Color = ads( Pos2, eyeNorm );

	Out.Pos = Pos2;

    return Out;
}
[Fragment shader]
float4 main(VS_OUTPUT IN) : COLOR
{
    return float4(IN.Color, 1.0);
}

