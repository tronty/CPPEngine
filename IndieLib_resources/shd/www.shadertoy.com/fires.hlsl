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
// https://www.shadertoy.com/view/XsXSWS
// Fires
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
//////////////////////
// Fire Flame shader

// procedural noise_ from IQ
float2 hash( float2 p )
{
	p = float2( dot(p,float2(127.1,311.7)),
			 dot(p,float2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise_( in float2 p )
{
	const float K1 = 0.366025404; // (sqrt(3)-1)/2;
	const float K2 = 0.211324865; // (3-sqrt(3))/6;
	
	float2 i = floor( p + (p.x+p.y)*K1 );
	
	float2 a = p - i + (i.x+i.y)*K2;
	float2 o = (a.x>a.y) ? float2(1.0,0.0) : float2(0.0,1.0);
	float2 b = a - o + K2;
	float2 c = a - 1.0 + 2.0*K2;
	
	float3 h = max( 0.5-float3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	
	float3 n = h*h*h*h*float3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
	
	return dot( n, float3(70, 70, 70) );
}

float fbm(float2 uv)
{
	float f;
	float2x2 m = float2x2( 1.6,  1.2, -1.2,  1.6 );
	f  = 0.5000*noise_( uv ); uv = mul(m,uv);
	f += 0.2500*noise_( uv ); uv = mul(m,uv);
	f += 0.1250*noise_( uv ); uv = mul(m,uv);
	f += 0.0625*noise_( uv ); uv = mul(m,uv);
	f = 0.5 + 0.5*f;
	return f;
}

// no defines, standard redish flames
//#define BLUE_FLAME
//#define GREEN_FLAME

const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;

float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;
	float2 uv = fragCoord.xy / iResolution.xy;
	float2 q = uv;
	q.x *= 5.;
	q.y *= 2.;
	float strength = floor(q.x+1.);
	float T3 = max(3.,1.25*strength)*iTime;
	q.x = mod(q.x,1.)-0.5;
	q.y -= 0.25;
	float n = fbm(strength*q - float2(0,T3));
	float c = 1. - 16. * pow( max( 0., length(q*float2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );
//	float c1 = n * c * (1.5-pow(1.25*uv.y,4.));
	float c1 = n * c * (1.5-pow(2.50*uv.y,4.));
	c1=clamp(c1,0.,1.);

	float3 col = float3(1.5*c1, 1.5*c1*c1*c1, c1*c1*c1*c1*c1*c1);
	
#ifdef BLUE_FLAME
	col = col.zyx;
#endif
#ifdef GREEN_FLAME
	col = 0.85*col.yxz;
#endif
	
	float a = c * (1.-pow(uv.y,3.));
	return float4( mix(float3(0,0,0),col,a), 1.0);
}
