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
	float3 Normal : NORMAL;
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
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection);
    	Out.Normal = normalize( mul(float4(IN.Normal,1), WorldViewProjection));
	return Out;
}
[Fragment shader]
CONST float3 SpotLightposition = float3(10.0, 10.0, 10.0);   // Position in eye coords
CONST float3 SpotLightintensity = float3(0.9,0.9,0.9);
CONST float3 SpotLightdirection = float3( -1.0, 0, 0);  // Direction of the spotlight in eye coords.
CONST float SpotLightexponent=30.0;  // Angular attenuation exponent
CONST float SpotLightcutoff=15.0;    // Cutoff angle (between 0 and 90}
CONST float Shininess=100.0;    // Specular shininess factor
#if 1
CONST float3 Kd = float3(0.9f, 0.5f, 0.3f);
CONST float3 Ks = float3(0.95f, 0.95f, 0.95f);
CONST float3 Ka = float3(0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
#elif 0
CONST float3 Kd = float3(0.9f, 0.5f, 0.3f);
CONST float3 Ks = float3(0.95f, 0.95f, 0.95f);
CONST float3 Ka = float3(0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
#elif 0
CONST float3 Kd = float3(0.7f, 0.7f, 0.7f);
CONST float3 Ks = float3(0.9f, 0.9f, 0.9f);
CONST float3 Ka = float3(0.2f, 0.2f, 0.2f);
#endif

float3 adsWithSpotlight(VS_OUTPUT IN)
{
    float3 n = normalize(IN.Normal);
    float3 s = normalize( SpotLightposition - IN.Pos.xyz );
    float3 spotDir = normalize( SpotLightdirection);
    float angle = acos( dot(-s, spotDir) );
    float cutoff = radians( clamp( SpotLightcutoff, 0.0, 90.0 ) );
    float3 ambient = SpotLightintensity * Ka;
    if( angle < cutoff )
    {
        float spotFactor = pow( dot(-s, spotDir), SpotLightexponent );
        float3 v = normalize(-IN.Pos.xyz);
        float3 h = normalize( v + s );
        return
            ambient +
            spotFactor * SpotLightintensity * (
              Kd * max( dot(s, n), 0.0 ) +
              Ks * pow( max( dot(h,n), 0.0 ), Shininess )
           );
    }
        return ambient;    
}

float4 main(VS_OUTPUT IN) : COLOR
{
	//return float4(1.0, 1.0, 1.0, 1.0);
    //return float4(IN.Normal, 1.0);
	float3 a=adsWithSpotlight(IN);
    return float4(a, 1.0);
}

