/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
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
ROW_MAJOR 
float4x4 worldViewProj 
MVPSEMANTIC
;
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
float2      iResolution=float2(1,1);                  // viewport iResolution (in pixels)
float     iTime=0;                        // shader playback iTime (in seconds)
float2      iMouse=float2(1,1);                       // iMouse pixel coords

// https://www.shadertoy.com/view/NsdSW4

#define NUM_OCTAVES 5

float2 random2(float2 st) {
	st = float2(dot(st, float2(127.1, 311.7)),
		dot(st, float2(269.5, 183.3)));
	return -1.0 + 2.0 * frac(sin(st) * 43758.5453123 * 0.7897);
}

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
float noise(float2 st) {
	float2 i = floor(st);
	float2 f = frac(st);

	float2 u = f * f * (3.0 - 2.0 * f);

	return lerp(lerp(dot(random2(i + float2(0.0, 0.0)), f - float2(0.0, 0.0)),
		dot(random2(i + float2(1.0, 0.0)), f - float2(1.0, 0.0)), u.x),
		lerp(dot(random2(i + float2(0.0, 1.0)), f - float2(0.0, 1.0)),
			dot(random2(i + float2(1.0, 1.0)), f - float2(1.0, 1.0)), u.x), u.y);
}

float fbm(float2 x) {
	float v = 0.0;
	float a = 0.5;
	float2 shift = float2(100,100);
	// Rotate to reduce axial bias
	float2x2 rot = float2x2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = mul(rot , x) * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

float2x2 rotate(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return float2x2(c, -s, s, c);
}

float4 main(VsOut IN): COLOR
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    // Normalized pixel coordinates (from 0 to 1)
    float2 uv = (fragCoord-0.5*iResolution.xy)/iResolution.x * 4.0;

    float r = length(uv);
	float f = (1.0 - sqrt(1.0 - r)) / r;
    float light = dot(uv, float2(cos(iTime / 4.0), sin(iTime / 4.0)));
    float reflectivity = 1.0;

    float noiseVal = fbm((uv * f * 5.0) + 500.0) + 0.5;
    float cloudNoise = fbm((mul(rotate(iTime / 16.0), (uv * f * 2.0))) + 1000.0) + 0.5;
    //cloudNoise *= rotate(iTime);
    float3 col = float3(0,0,0);
    
    if (r < 1.0) {
        if (noiseVal < 0.5) {
            col = float3(0.0, 0.0, 1.0);
        }
        else if (noiseVal > 0.65) {
            col = float3(1.0, 1.0, 1.0) * noiseVal;
        }
        else {
            col = float3(0.5, noiseVal, 0.0);
        }
        reflectivity = noiseVal * 2.0;
        //if (cloudNoise > 0.55) col = float3(1.0);
        col += smoothstep(0.55, 0.6, cloudNoise);
        //col += float3(1.0) * smoothstep(0.75, 0.5, f) * (noiseVal + 0.75);
    }
    else {
        col = float3(0.5, 0.8, 1.0) * smoothstep(1.1, 1.0, r);
        reflectivity = 0.5;
    }
    //col += float3(1.0) * smoothstep(0.7, 0.5, f) * (noiseVal + 0.75);
    
    //if (f / (noiseVal + 0.5) < 0.6) {
    //    col += float3(1.0);
    //}
    
    col *= light;
    col += smoothstep(1.1, 1.0, r) * light * reflectivity;
    
    // Output to screen
    return float4(col,1.0);
}

