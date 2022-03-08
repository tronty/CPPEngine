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

float3 LightPosition[5];  // Light position in eye coords.
float3 LightIntensity[5]; // Light intesity (amb., diff., and spec.)
CONST float Shininess=180.0;
// Material parameters
#if 1
CONST float3 Kd = float3(0.4, 0.4, 0.4);
CONST float3 Ks = float3(0.9, 0.9, 0.9);
CONST float3 Ka = float3(0.1, 0.1, 0.1);
#elif 0
CONST float3 Kd = float3(0.1, 0.1, 0.1);
CONST float3 Ks = float3(0.9, 0.9, 0.9);
CONST float3 Ka = float3(0.1, 0.1, 0.1);
#endif

float3 ads( int lightIndex, float3 position, float3 norm )
{
    float3 s = normalize( LightPosition[lightIndex] - position );
    float3 v = normalize(-position);
    float3 r = reflect( -s, norm );
    float3 I = LightIntensity[lightIndex];
    return
        I * ( Ka +
            Kd * max( dot(s, norm), 0.0 ) +
            Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}


VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection);
	float3 eyeNorm = mul(float4(IN.Normal,1), WorldViewProjection).xyz;
    float3 eyePosition = Out.Pos.xyz;

    // Evaluate the lighting equation, for each light
    Out.Color = float3(0.0);
    for( int i = 0; i < 5; i++ )
        Out.Color += ads( i, eyePosition, eyeNorm );

	//Out.Color = eyeNorm;
    return Out;
}

[Fragment shader]
float4 main(VS_OUTPUT IN) : COLOR
{
    return float4(IN.Color, 1.0);
}

