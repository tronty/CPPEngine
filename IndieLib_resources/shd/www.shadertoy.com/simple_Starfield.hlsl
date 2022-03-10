/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
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
float iTime=0.0;
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);

// https://www.shadertoy.com/view/XsXBzM
// simple Starfield
//modified version of https://www.shadertoy.com/view/4ljXDt

float Cell(float2 c) {
	float2 uv = frac(c);c -= uv;
	return (1.-length(uv*2.-1.)) * step(frac(sin(c.x+c.y*1e2)*1e3), .04);
}

float4 main(float2 fragCoord : TEXCOORD0) : COLOR0
{
	float2 p = fragCoord.xy / iResolution.xy -.5;
	// ??? atan: float a = frac(atan(p.x, p.y) / 6.2832);
	float a = frac(atan2(p.x, p.y) / 6.2832);
	float d = length(p);
	float z = iTime / 1.5;
    float3 col;
    
    for(int i=0; i<3 ;i++)
	{
    	z += 0.02;
		float2 coord = float2(pow(d, .04), a)*256.;
		float2 delta = float2(1. + z*20., 1.);
		float c = Cell(coord-=delta);
		c += Cell(coord-=delta);
        col[i]=c*d*3.;
    }    
	
	return float4(col,1);
}
