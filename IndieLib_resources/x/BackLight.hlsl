/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
struct VS_INPUT
{
   	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
float4x4 matViewProjection;
float4x4 matWorld;
float4x4 matViewInverse;
float3 Light;
float4 LightColor;
float4 MaterialColor;
float BackLightOffset;
float4 BackLightColor;
float4 AmbientColor;

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float3 Normal : NORMAL;
};

struct VS_OUTPUT 
{
   float4 Position : POSITION;
   float4 DiffuseColor : COLOR0;
   float3 BackLightOffset : TEXCOORD0;
   float3 Normal : TEXCOORD1;
   float3 EyePos : TEXCOORD2;
   float4 PosWorldSpace : TEXCOORD3;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;
   
   Output.Position = mul( matViewProjection, float4(Input.Position,1) );
   
   // compute the diffuse color
   float3 Normal = mul(matWorld, Input.Normal);
   Normal = normalize(Normal);
   Output.Normal = Normal;
   float3 LightVector = normalize(Light);
   Output.DiffuseColor = MaterialColor * (saturate(LightColor * dot(Normal, LightVector)) + AmbientColor);
 
   // Shift the backlight in the direction of the normal
   Output.BackLightOffset = BackLightOffset * Normal;
   
   // world space position of the eye
   Output.EyePos = mul( matViewInverse, float4(0,0,0,1) );
   
   // world space position
   Output.PosWorldSpace = float4(Input.Position,1);

   return( Output );
   
}

[Fragment shader]
const float frenelExponent = 4;
const float specularExponent = 32;
float4 BackLightColor;
bool enableBackLight;

struct PS_INPUT
{
   float4 Position : POSITION;
   float4 DiffuseColor : COLOR0;
   float3 BackLightOffset : TEXCOORD0;
   float3 Normal : TEXCOORD1;
   float3 EyePos : TEXCOORD2;
   float4 PosWorldSpace : TEXCOORD3;
};


float4 ps_main(PS_INPUT In) : COLOR0
{  
   // frenel term
   float3 View = normalize(In.EyePos - In.PosWorldSpace);
   float3 Normal = normalize(In.Normal);
   float fresnel = pow(1.0f - dot(View, Normal), frenelExponent);
     
   // make the backlight vector relative to the view vector
   float3 BackLight = normalize(-View + In.BackLightOffset);
    // specular term
   float3 Half = normalize (View + BackLight);
   float specular = pow(dot(Normal, Half), specularExponent);
   
   if(!enableBackLight)
      BackLightColor *= 0;
   
   return In.DiffuseColor + (fresnel * specular * BackLightColor);   
}

