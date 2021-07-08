/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(GLSL)
	#define __HLSL5__ 1
	#define ROW_MAJOR row_major
	#define MVPSEMANTIC
	#define WSIGN +
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
	#define __GLSL__ 1
	#define WSIGN +
	#define ROW_MAJOR
	#define MVPSEMANTIC
	#define fract frac
	#define mix lerp
	#define atan(x,y) atan2(y,x)
	#define TextureSample(T,S,uv) T.Sample(S, uv)
	#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod(TEX, float4(TEXCOORD.x, TEXCOORD.y, 0.0, 0.0))
	#define SAMPLER2D sampler2D
#endif 

#ifdef GLSL
	#define float4 vec4
	#define float4x4 mat4
	#define Texture2D texture2D
	#define SamplerState sampler
#endif

[Rootsig]

struct PS_IN
{
#ifdef GLSL
	vec2 Tex;
#else
	float4 Pos : SV_Position;
	float2 Tex : TexCoord;
#endif
};

[Vertex: ColoredVS]

#ifdef GLSL
layout (location = 0) in vec4 Pos;
void main()
{
	gl_Position = Constants.WVP * Pos;
}
#else
void main(out float4 Out : SV_Position, float4 Pos : Vertex)
{
	Out = mul(Constants.WVP, Pos);
}
#endif

[Pixel: ColoredPS]

#ifdef GLSL
layout(location = 0) out vec4 Out;
void main()
{
	Out = Color;
}
#else
void main(out float4 Out : SV_Target)
{
	Out = Color;
}
#endif


[Vertex: TexturedVS]

#ifdef GLSL
layout(location = 0) in vec4 Pos;
layout(location = 1) in vec2 Tex;
layout(location = 0) out PS_IN Out;
void main()
{
	gl_Position = Constants.WVP * Pos;
	Out.Tex = Tex;
}
#else
void main(out PS_IN Out, float4 Pos : Vertex, float2 Tex : TexCoord)
{
	Out.Pos = mul(Constants.WVP, Pos);
	Out.Tex = Tex;
}
#endif

[Pixel: TexturedPS]

#ifdef GLSL
layout(location = 0) out vec4 Out;
layout(location = 0) in PS_IN In;
void main()
{
	Out = Color * texture(sampler2D(Texture, Filter), In.Tex);
}
#else
void main(out float4 Out : SV_Target, PS_IN In)
{
	Out = Color * Texture.Sample(Filter, In.Tex);
}
#endif

