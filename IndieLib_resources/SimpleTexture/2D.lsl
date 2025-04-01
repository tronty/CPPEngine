#ifndef GLSL
	#define int2 ivec2
	#define int3 ivec3
	#define int4 ivec4
	#define uint2 uvec2
	#define uint3 uvec3
	#define uint4 uvec4
	#define float2 vec2
	#define float3 vec3
	#define float4 vec4
	#define float4x4 mat4
	#define Texture2D texture2D
	#define SamplerState sampler
	#define saturate(x) clamp(x, 0.0f, 1.0f)
	#define TextureSample(T,S,uv) texture(sampler2D(T, S), uv)
	float4 GetPixelColor_(Texture2D<float4> atex, float2 aTexCoord)
	{
		uint width_;
		uint height_;
		atex.GetDimensions(width_, height_);
		float2 dim_;
		dim_.x=float(width_-1);
		dim_.y=float(height_-1);
		return atex.Load(int3(dim_.x*aTexCoord.x, dim_.y*aTexCoord.y, 0));
	}
	#define SAMPLE2D(TEX, TEXCOORD) GetPixelColor_(TEX, TEXCOORD)
	#define SAMPLER2D Texture2D<float4>
#elif defined(GLSL)
	#define TextureSample(T,S,uv) T.Sample(S, uv)
	#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod(TEX, float4(TEXCOORD.x, TEXCOORD.y, 0.0, 0.0))
	#define SAMPLER2D sampler2D
#endif
/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn {
	float2 position: POSITION;
	float2 texCoord: TEXCOORD0;
};

struct VsOut
{
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};

[Vertex shader]

ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut mainVS(VsIn In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}

[Fragment shader]

sampler2D Base;
float4 mainPS(VsOut IN): COLOR
{
	return tex2D(Base, IN.texCoord);
}

