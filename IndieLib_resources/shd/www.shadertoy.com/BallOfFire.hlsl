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
// https://www.shadertoy.com/view/lsf3RH
// Ball Of Fire
float3 mod(float3 x, float y)
{
  return x - y * floor(x / y);
}

float snoise(float3 uv, float res)
{
	const float3 s = float3(1e0, 1e2, 1e3);
	
	uv *= res;
	
	float3 uv0 = floor(mod(uv, res))*s;
	float3 uv1 = floor(mod(uv+float3(1,1,1), res))*s;
	
	float3 f = fract(uv); f = f*f*(3.0-2.0*f);

	float4 v = float4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);

	float4 r = fract(sin(v*1e-1)*1e3);
	float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	r = fract(sin((v + uv1.z - uv0.z)*1e-1)*1e3);
	float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	return mix(r0, r1, f.z)*2.-1.;
}

const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;
	float2 p = -.5 + fragCoord.xy / iResolution.xy;
	p.x *= iResolution.x/iResolution.y;
	
	float color = 3.0 - (3.*length(2.*p));
	
	float3 coord = float3(atan(p.x,p.y)/6.2832+.5, length(p)*.4, .5);
	
	for(int i = 1; i <= 7; i++)
	{
		float power = pow(2.0, float(i));
		color += (1.5 / power) * snoise(coord + float3(0.,-iTime*.05, iTime*.01), power*16.);
	}
	return float4( color, pow(max(color,0.),2.)*0.4, pow(max(color,0.),3.)*0.15 , 1.0);
}
