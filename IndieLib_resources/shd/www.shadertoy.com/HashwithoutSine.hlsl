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
// https://www.shadertoy.com/view/4djSRW
// Hash without Sine
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
float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
float iTime=0.0;
// Hash without Sine
// Creative Commons Attribution-ShareAlike 4.0 International Public License
// Created by David Hoskins.

// https://www.shadertoy.com/view/4djSRW
// Trying to find a Hash function that is the same on ALL systens
// and doesn't rely on trigonometry functions that change accuracy 
// depending on GPU. 
// New one on the left, sine function on the right.
// It appears to be the same speed, but I suppose that depends.

// * Note. It still goes wrong eventually!
// * Try full-screen paused to see details.


#define ITERATIONS 4


// *** Change these to suit your range of random numbers..

// *** Use this for integer stepped ranges, ie Value-Noise/Perlin noise_ functions.
#define HASHSCALE1 .1031
#define HASHSCALE3 float3(.1031, .1030, .0973)
#define HASHSCALE4 float4(.1031, .1030, .0973, .1099)

// For smaller input rangers like audio tick or 0-1 UVs use these...
//#define HASHSCALE1 443.8975
//#define HASHSCALE3 float3(443.897, 441.423, 437.195)
//#define HASHSCALE4 float4(443.897, 441.423, 437.195, 444.129)



//----------------------------------------------------------------------------------------
//  1 out, 1 in...
float hash11(float p)
{
	float3 p3  = frac(float3(p, p, p) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash12(float2 p)
{
	float3 p3  = frac(float3(p.xy, p.xy, p.xy) * HASHSCALE1); // ???
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash13(float3 p3)
{
	p3  = frac(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  2 out, 1 in...
float2 hash21(float p)
{
	float3 p3 = frac(float3(p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.xx+p3.yz)*p3.zy);

}

//----------------------------------------------------------------------------------------
///  2 out, 2 in...
float2 hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);

}

//----------------------------------------------------------------------------------------
///  2 out, 3 in...
float2 hash23(float3 p3)
{
	p3 = frac(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);
}

//----------------------------------------------------------------------------------------
//  3 out, 1 in...
float3 hash31(float p)
{
   float3 p3 = frac(float3(p) * HASHSCALE3);
   p3 += dot(p3, p3.yzx+19.19);
   return frac((p3.xxy+p3.yzz)*p3.zyx); 
}


//----------------------------------------------------------------------------------------
///  3 out, 2 in...
float3 hash32(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19);
    return frac((p3.xxy+p3.yzz)*p3.zyx);
}

//----------------------------------------------------------------------------------------
///  3 out, 3 in...
float3 hash33(float3 p3)
{
	p3 = frac(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19);
    return frac((p3.xxy + p3.yxx)*p3.zyx);

}

//----------------------------------------------------------------------------------------
// 4 out, 1 in...
float4 hash41(float p)
{
	float4 p4 = frac(float4(p) * HASHSCALE4);
    p4 += dot(p4, p4.wzxy+19.19);
    return frac((p4.xxyz+p4.yzzw)*p4.zywx);
    
}

//----------------------------------------------------------------------------------------
// 4 out, 2 in...
float4 hash42(float2 p)
{
	float4 p4 = frac(float4(p.xyxy) * HASHSCALE4);
    p4 += dot(p4, p4.wzxy+19.19);
    return frac((p4.xxyz+p4.yzzw)*p4.zywx);

}

//----------------------------------------------------------------------------------------
// 4 out, 3 in...
float4 hash43(float3 p)
{
	float4 p4 = frac(float4(p.xyzx)  * HASHSCALE4);
    p4 += dot(p4, p4.wzxy+19.19);
    return frac((p4.xxyz+p4.yzzw)*p4.zywx);
}

//----------------------------------------------------------------------------------------
// 4 out, 4 in...
float4 hash44(float4 p4)
{
	p4 = frac(p4  * HASHSCALE4);
    p4 += dot(p4, p4.wzxy+19.19);
    return frac((p4.xxyz+p4.yzzw)*p4.zywx);
}

//###############################################################################

//----------------------------------------------------------------------------------------
float hashOld12(float2 p)
{
    // Two typical hashes...
	return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
    
    // This one is better, but it still stretches out quite quickly...
    // But it's really quite bad on my Mac(!)
    //return frac(sin(dot(p, float2(1.0,113.0)))*43758.5453123);

}

float3 hashOld33( float3 p )
{
	p = float3( dot(p,float3(127.1,311.7, 74.7)),
			  dot(p,float3(269.5,183.3,246.1)),
			  dot(p,float3(113.5,271.9,124.6)));

	return frac(sin(p)*43758.5453123);
}

//----------------------------------------------------------------------------------------


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
	float2 position = fragCoord.xy;
    float2 uv = fragCoord.xy / iResolution.xy;
#if 1
	float a = 0.0, b = a;
    for (int t = 0; t < ITERATIONS; t++)
    {
        float v = float(t+1)*.152;
        float2 pos = (position * v + iTime * 1500. + 50.0);
        a += hash12(pos);
    	b += hashOld12(pos);
    }
    float3 col = float3(lerp(b, a, step(uv.x, .5))) / float(ITERATIONS);
#else
	float3 a = float3(0.0), b = a;
    for (int t = 0; t < ITERATIONS; t++)
    {
        float v = float(t+1)*.132;
        float3 pos = float3(position, iTime*.3) + iTime * 500. + 50.0;
        a += hash33(pos);
        b += hashOld33(pos);
    }
    float3 col = float3(lerp(b, a, step(uv.x, .5))) / float(ITERATIONS);
#endif

    col = lerp(float3(.4, 0.0, 0.0), col, smoothstep(.5, .495, uv.x) + smoothstep(.5, .505, uv.x));
	fragColor = float4(col, 1.0);
	return fragColor;
}
