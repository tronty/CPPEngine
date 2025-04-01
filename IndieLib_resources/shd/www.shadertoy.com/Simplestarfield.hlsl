/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
float mod(float x, float y)
{
  return x - y * floor(x / y);
}


float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
#define STARDISTANCE 150.
#define STARBRIGHTNESS 0.5
#define STARDENCITY 0.05

float hash13(float3 p3)
{
	p3  = fract(p3 * float3(.1031,.11369,.13787));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float stars(float3 ray)
{
    float3 p = ray * STARDISTANCE;
    float brigtness = smoothstep(1.0 - STARDENCITY, 1.0, hash13(floor(p)));
    return smoothstep(STARBRIGHTNESS, 0., length(fract(p) - 0.5)) * brigtness;
}

float3 camera(float2 fragCoord)
{
	float3 ray = normalize(float3( fragCoord.xy-iResolution.xy*.5, iResolution.x));
    float2 angle = float2(3. + iTime * -0.01, 10. + iTime * 0.10);
    float4 cs = float4(cos(angle.x), sin(-angle.x), cos(angle.y), sin(angle.y));
    ray.yz = mul(float2x2(cs.xy,-cs.y,cs.x), ray.yz); 
    ray.xz = mul(float2x2(cs.zw,-cs.w,cs.z), ray.xz); 
    return ray;
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float3 ray = camera(fragCoord);
    float s = stars(ray);
	fragColor = float4(s, s ,s ,1.0);
	return fragColor;
}

