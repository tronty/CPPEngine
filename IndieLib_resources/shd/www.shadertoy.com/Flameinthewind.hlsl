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
// https://www.shadertoy.com/view/4tXXRn
// Flame in the wind
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
sampler2D iChannel0;
sampler2D iChannel1;
float2x2 rotz(float angle)
{
    float2x2 m;
    m[0][0] = cos(angle); m[0][1] = -sin(angle);
    m[1][0] = sin(angle); m[1][1] = cos(angle);
    return m;
}

float rand(float2 co){
    return frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453);
}


float fbm(float2 uv)
{
    float n = (tex2D(iChannel1, uv).r - 0.5) * 0.5;
    n += (tex2D(iChannel1, uv * 2.0).r - 0.5) * 0.5 * 0.5;
    n += (tex2D(iChannel1, uv * 3.0).r - 0.5) * 0.5 * 0.5 * 0.5;
    
	return n + 0.5;
}


// -----------------------------------------------


float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;
    
    float2 uv = fragCoord.xy / iResolution.xy;
    float2 _uv = uv;
    uv -= float2(0.5, 0.5);
    uv.y /= iResolution.x / iResolution.y;
    float2 centerUV = uv;
    
    // height variation from fbm
    float variationH = fbm(float2(iTime * .3, iTime * .3)) * 1.1;
    
    // flame "speed"
    float2 offset = float2(0.0, -iTime * 0.15);
    
    // flame turbulence
    float f = fbm(uv * 0.1 + offset); // rotation from fbm
    float l = max(0.1, length(uv)); // rotation amount normalized over distance
	uv = mul(rotz( ((f - 0.5) / l) * smoothstep(-0.2, .4, _uv.y) * 0.45),  uv); // ???    
    
    // flame thickness
    float flame = 1.3 - length(uv.x) * 5.0;
    
    // bottom of flame 
    float blueflame = pow(flame * .9, 15.0);
    blueflame *= smoothstep(.2, -1.0, _uv.y);
    blueflame /= abs(uv.x * 2.0);
    blueflame = clamp(blueflame, 0.0, 1.0);
    
    // flame
    flame *= smoothstep(1., variationH * 0.5, _uv.y);
	flame = clamp(flame, 0.0, 1.0);
    flame = pow(flame, 3.);
    flame /= smoothstep(1.1, -0.1, _uv.y);    
	
    // colors
    float4 col = lerp(float4(1.0, 1., 0.0, 0.0), float4(1.0, 1.0, .6, 0.0), flame);
    col = lerp(float4(1.0, .0, 0.0, 0.0), col, smoothstep(0.0, 1.6, flame));
    float4 fragColor = col;
    
	// a bit blueness on the bottom
    float4 bluecolor = lerp(float4(0.0, 0.0, 1.0, 0.0), fragColor, 0.95);
    fragColor = lerp(fragColor, bluecolor, blueflame);
    
    // clear bg outside of the flame
    fragColor *= flame;
    fragColor.a = flame;
    
    // bg halo
    float haloSize = 0.5;
    float centerL = 1.0 - (length(centerUV + float2(0.0, 0.1)) / haloSize);
    float4 halo = float4(.8, .3, .3, 0.0) * 1.0 * fbm(float2(iTime * 0.035, iTime * 0.035)) * centerL + 0.02;
    float4 finalCol = lerp(halo, fragColor, fragColor.a);
    fragColor = finalCol;

    // just a hint of noise_
    fragColor *= lerp(rand(uv) + rand(uv * .45), 1.0, 0.9);
    fragColor = clamp(fragColor, 0.0, 1.0);
    
    return fragColor;
}

