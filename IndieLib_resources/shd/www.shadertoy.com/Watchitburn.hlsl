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
// https://www.shadertoy.com/view/lsSGWw
// Watch it burn!
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
const float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
const float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
sampler2D iChannel0;
sampler2D iChannel1;


#define ITERATIONS 12
#define SPEED 10.0
#define DISPLACEMENT 0.05
#define TIGHTNESS 10.0
#define YOFFSET 0.1
#define YSCALE 0.25
#define FLAMETONE float3(50.0, 5.0, 1.0)

float shape(in float2 pos) // a blob shape to distort
{
	return clamp( sin(pos.x*3.1416) - pos.y+YOFFSET, 0.0, 1.0 );
}

float noise_( in float3 x ) // iq noise_ function
{
	float3 p = floor(x);
    float3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
	float2 rg = tex2D( iChannel0, (uv+ 0.5)/256.0).yx;
	return mix( rg.x, rg.y, f.z ) * 2.0 - 1.0;
}

float ifrac;
float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;
	float2 uv = fragCoord.xy / iResolution.xy;
	float nx = 0.0;
	float ny = 0.0;
	for (int i=1; i<ITERATIONS+1; i++)
	{
		float ii = pow(float(i), 2.0);
		float ifrac = float(i)/float(ITERATIONS);
		float t = ifrac * iTime * SPEED;
		float d = (1.0-ifrac) * DISPLACEMENT;
		nx += noise_( float3(uv.x*ii-iTime*ifrac, uv.y*YSCALE*ii-t, 0.0)) * d * 2.0;
		ny += noise_( float3(uv.x*ii+iTime*ifrac, uv.y*YSCALE*ii-t, iTime*ifrac/ii)) * d;
	}
	float flame = shape( float2(uv.x+nx, uv.y+ny) );
	float3 col = pow(flame, TIGHTNESS) * FLAMETONE;
    
    // tonemapping
    col = col / (1.0+col);
    float f=1.0/2.2;
    col = pow(col, float3(f,f,f));
    col = clamp(col, 0.0, 1.0);
	
	return float4(col.x, col.y, col.z, 1.0);
}
