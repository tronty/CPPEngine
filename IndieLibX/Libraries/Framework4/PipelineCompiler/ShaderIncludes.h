
#ifdef VULKAN
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
#else
	#define TextureSample(T,S,uv) T.Sample(S, uv)
#endif