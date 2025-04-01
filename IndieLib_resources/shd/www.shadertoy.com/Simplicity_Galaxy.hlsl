#define texture(a1,a2) tex2D(a1,a2)
/*
  Copyright (c) 2022 Tuomas Roenty   http://www.tuomasronty.fi/
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

// https://www.shadertoy.com/view/fsXczN
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
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords

// https://www.shadertoy.com/view/MslGWN
// Simplicity Galaxy
//CBS
//Parallax scrolling fractal galaxy.
//Inspired by JoshP's Simplicity shader: https://www.shadertoy.com/view/lslGWr

// http://www.fractalforums.com/new-theories-and-research/very-simple-formula-for-fractal-patterns/
float field(in float3 p,float s) {
	float strength = 7. + .03 * log(1.e-6 + frac(sin(iTime) * 4373.11));
	float accum = s/4.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 26; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + float3(-.5, -.4, -1.5);
		float w = exp(-float(i) / 7.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.2));
		tw += w;
		prev = mag;
	}
	return max(0., 5. * accum / tw - .7);
}

// Less iterations for second layer
float field2(in float3 p, float s) {
	float strength = 7. + .03 * log(1.e-6 + frac(sin(iTime) * 4373.11));
	float accum = s/4.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 18; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + float3(-.5, -.4, -1.5);
		float w = exp(-float(i) / 7.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.2));
		tw += w;
		prev = mag;
	}
	return max(0., 5. * accum / tw - .7);
}

float3 nrand3( float2 co )
{
	float3 a = frac( cos( co.x*8.3e-3 + co.y )*float3(1.3e5, 4.7e5, 2.9e5) );
	float3 b = frac( sin( co.x*0.3e-3 + co.y )*float3(8.1e5, 1.0e5, 0.1e5) );
	float3 c = lerp(a, b, 0.5);
	return c;
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 uv = 2. * fragCoord.xy / iResolution.xy - 1.;
	float2 uvs = uv * iResolution.xy / max(iResolution.x, iResolution.y);
	float3 p = float3(uvs / 4., 0) + float3(1., -1.3, 0.);
	p += .2 * float3(sin(iTime / 16.), sin(iTime / 12.),  sin(iTime / 128.));
	
	float freqs[4];
	//Sound
	freqs[0] = texture( iChannel0, float2( 0.01, 0.25 ) ).x;
	freqs[1] = texture( iChannel0, float2( 0.07, 0.25 ) ).x;
	freqs[2] = texture( iChannel0, float2( 0.15, 0.25 ) ).x;
	freqs[3] = texture( iChannel0, float2( 0.30, 0.25 ) ).x;

	float t = field(p,freqs[2]);
	float v = (1. - exp((abs(uv.x) - 1.) * 6.)) * (1. - exp((abs(uv.y) - 1.) * 6.));
	
    //Second Layer
	float3 p2 = float3(uvs / (4.+sin(iTime*0.11)*0.2+0.2+sin(iTime*0.15)*0.3+0.4), 1.5) + float3(2., -1.3, -1.);
	p2 += 0.25 * float3(sin(iTime / 16.), sin(iTime / 12.),  sin(iTime / 128.));
	float t2 = field2(p2,freqs[3]);
	float4 c2 = lerp(.4, 1., v) * float4(1.3 * t2 * t2 * t2 ,1.8  * t2 * t2 , t2* freqs[0], t2);
	
	
	//Let's add some stars
	//Thanks to http://glsl.heroku.com/e#6904.0
	float2 seed = p.xy * 2.0;	
	seed = floor(seed * iResolution.x);
	float3 rnd = nrand3( seed );
	float4 starcolor = float4(pow(rnd.y,40.0));
	
	//Second Layer
	float2 seed2 = p2.xy * 2.0;
	seed2 = floor(seed2 * iResolution.x);
	float3 rnd2 = nrand3( seed2 );
	starcolor += float4(pow(rnd2.y,40.0));
	
	return lerp(freqs[3]-.3, 1., v) * float4(1.5*freqs[2] * t * t* t , 1.2*freqs[1] * t * t, freqs[3]*t, 1.0)+c2+starcolor;
}
