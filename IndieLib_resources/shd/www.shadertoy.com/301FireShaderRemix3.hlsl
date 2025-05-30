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
// https://www.shadertoy.com/view/4ttGWM
// 301's Fire Shader - Remix 3
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
const float2      iChannelResolution=float2(1,1);
const float2      iResolution=float2(1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
const float2 iMouse=float2(1,1);
sampler2D iChannel0;
sampler2D iChannel1;
// I started working a bit on the colors of Remix 2, ended up with something like this. :)
// Remix 2 here: https://www.shadertoy.com/view/MtcGD7
// Remix 1 here: https://www.shadertoy.com/view/llc3DM
// Original here: https://www.shadertoy.com/view/XsXXRN

float rand(float2 n) {
    return fract(sin(cos(dot(n, float2(12.9898,12.1414)))) * 83758.5453);
}

float noise_(float2 n) {
    const float2 d = float2(0.0, 1.0);
    float2 b = floor(n), f = smoothstep(float2(0, 0), float2(1, 1), fract(n));
    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float fbm(float2 n) {
    float total = 0.0, amplitude = 1.0;
    for (int i = 0; i <5; i++) {
        total += noise_(n) * amplitude;
        n += n*1.7;
        amplitude *= 0.47;
    }
    return total;
}


float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;
    const float3 c1 = float3(0.5, 0.0, 0.1);
    const float3 c2 = float3(0.9, 0.1, 0.0);
    const float3 c3 = float3(0.2, 0.1, 0.7);
    const float3 c4 = float3(1.0, 0.9, 0.1);
    const float3 c5 = float3(0.1, 0.1, 0.1);
    const float3 c6 = float3(0.9, 0.9, 0.9);

    float2 speed = float2(0.1, 0.9);
    float shift = 1.327+sin(iTime*2.0)/2.4;
    float alpha = 1.0;
    
	float dist = 3.5-sin(iTime*0.4)/1.89;
    
    float2 uv = fragCoord.xy / iResolution.xy;
    float2 p = fragCoord.xy * dist / iResolution.xx;
    p += sin(p.yx*4.0+float2(.2,-.3)*iTime)*0.04;
    p += sin(p.yx*8.0+float2(.6,+.1)*iTime)*0.01;
    
    p.x -= iTime/1.1;
    float q = fbm(p - iTime * 0.3+1.0*sin(iTime+0.5)/2.0);
    float qb = fbm(p - iTime * 0.4+0.1*cos(iTime)/2.0);
    float q2 = fbm(p - iTime * 0.44 - 5.0*cos(iTime)/2.0) - 6.0;
    float q3 = fbm(p - iTime * 0.9 - 10.0*cos(iTime)/15.0)-4.0;
    float q4 = fbm(p - iTime * 1.4 - 20.0*sin(iTime)/14.0)+2.0;
    q = (q + qb - .4 * q2 -2.0*q3  + .6*q4)/3.8;
    float2 r = float2(fbm(p + q /2.0 + iTime * speed.x - p.x - p.y), fbm(p + q - iTime * speed.y));
    float3 c = mix(c1, c2, fbm(p + r)) + mix(c3, c4, r.x) - mix(c5, c6, r.y);
    float fff=1.0/(pow(c+1.61,float3(4, 4, 4))) * cos(shift * fragCoord.y / iResolution.y);
    float3 color = float3(fff, fff, fff);
    
    color=float3(1.0,.2,.05)/(pow((r.y+r.y)* max(.0,p.y)+0.1, 4.0));;
    color += (tex2D(iChannel0,uv*0.6+float2(.5,.1)).xyz*0.01*pow((r.y+r.y)*.65,5.0)+0.055)*mix( float3(.9,.4,.3),float3(.7,.5,.2), uv.y);
    color = color/(1.0+max(float3(0, 0, 0),color));
    return float4(color.x, color.y, color.z, alpha);
}
