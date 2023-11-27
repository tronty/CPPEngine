// https://digitalerr0r.wordpress.com/2009/03/23/xna-shader-programming-tutorial-4-normal-mapping/
/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]
 mat4 matWorldViewProj ;   
 mat4 matWorld;   
vec4 vecLightDir;
vec4 vecEye;

struct VS_INPUT
{
	vec3 Position : POSITION;
	vec3 Normal : NORMAL;
	vec3 BiNormal : BINORMAL;
	vec3 Tangent : TANGENT;
	vec3 color : TEXCOORD0;
	vec2 uv : TEXCOORD1;
};
struct VS_OUTPUT
{
    vec4 Pos  : POSITION;
    vec2 Tex : TEXCOORD0;
    vec3 Light : TEXCOORD1;
    vec3 View : TEXCOORD2;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
      __Pos = mul(vec4(_Position, 1), matWorldViewProj); // transform Position
      // Create tangent space to get normal and light to the same space.
      float3x3 worldToTangentSpace;
      worldToTangentSpace[0] = mul(normalize(vec4(_Tangent,1)), matWorld).xyz;
      worldToTangentSpace[1] = mul(normalize(vec4(_BiNormal,1)), matWorld).xyz;
      worldToTangentSpace[2] = mul(normalize(vec4(_Normal,1)), matWorld).xyz;
      // Just pass textures trough
      __Tex = _uv;
      vec4 PosWorld = mul(vec4(_Position, 1), matWorld);
      // Pass out light and view directions, pre-normalized
      __Light = normalize(mul(worldToTangentSpace, vecLightDir.xyz));
      __View = normalize(mul(worldToTangentSpace, (vecEye-PosWorld).xyz));
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
    vec4 Pos  : POSITION;
    vec2 Tex : TEXCOORD0;
    vec3 Light : TEXCOORD1;
    vec3 View : TEXCOORD2;
};
vec4 main(VS_OUTPUT IN) : COLOR
{
      // Get the color from ColorMapSampler using the texture coordinates in Tex.
      vec4 Color = texture2D(ColorMapSampler, _Tex);
      // Get the Color of the normal. The color describes the direction of the normal vector
      // and make it range from 0 to 1.
      vec3 N = (2.0 * (texture2D(NormalMapSampler, _Tex)))-1.0;
      // diffuse
      float D = saturate(dot(N, _Light));
      // reflection
      vec3 R = normalize(2 * D * N-_Light);
      // specular
      float S = pow(saturate(dot(R, _View)), 2);
      // calculate light (ambient + diffuse + specular)
      const vec4 Ambient = vec4(0.3, 0.3, 0.3, 1.0);
      return Color*Ambient + Color * D + Color*S;
}

