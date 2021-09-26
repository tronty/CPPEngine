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
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords

// https://www.shadertoy.com/view/4dlGW2
// Tileable Noise

// Tileable noise, for creating useful textures. By David Hoskins, Sept. 2013.
// It can be extrapolated to other types of randomised texture.

#define SHOW_TILING
#define TILES 2.0 // Use 1.0 for normal tiling across whole texture.

//----------------------------------------------------------------------------------------
float Hash(in float2 p, in float scale)
{
	// This is tiling part, adjusts with the scale...
	p = mod(p, scale);
	return frac(sin(dot(p, float2(27.16898, 38.90563))) * 5151.5473453);
}

//----------------------------------------------------------------------------------------
float Noise(in float2 p, in float scale )
{
	float2 f;
	
	p *= scale;

	
	f = frac(p);		// Separate integer from fractional
    p = floor(p);
	
    f = f*f*(3.0-2.0*f);	// Cosine interpolation approximation
	
    float res = lerp(lerp(Hash(p, 				 scale),
						Hash(p + float2(1.0, 0.0), scale), f.x),
					lerp(Hash(p + float2(0.0, 1.0), scale),
						Hash(p + float2(1.0, 1.0), scale), f.x), f.y);
    return res;
}

//----------------------------------------------------------------------------------------
float fBm(in float2 p)
{
    p += float2(sin(iTime * .7), cos(iTime * .45))*(.1) + iMouse.xy*.1/iResolution.xy;
	float f = 0.0;
	// Change starting scale to any integer value...
	float scale = 10.;
    p = mod(p, scale);
	float amp   = 0.6;
	
	for (int i = 0; i < 5; i++)
	{
		f += Noise(p, scale) * amp;
		amp *= .5;
		// Scale must be multiplied by an integer value...
		scale *= 2.;
	}
	// Clamp it just in case....
	return min(f, 1.0);
}

//----------------------------------------------------------------------------------------
float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
	float2 uv = fragCoord.xy / iResolution.xy;

	#ifdef SHOW_TILING
	uv *= TILES;
	#endif
	
	// Do the noise cloud (fractal Brownian motion)
	float bri = fBm(uv);
	
	bri = pow(bri, 1.2); // ...cranked up the contrast for demo.
	float3 col = float3(bri);
	
	#ifdef SHOW_TILING
	// Flash tile borders...
	float2 pixel = TILES / iResolution.xy;
	if (mod(iTime-2.0, 4.0) < 2.0)
	{
		float2 first 		= step(pixel, uv);
		uv  = step(frac(uv), pixel);	// Only add one line of pixels per tile.
		col = lerp(col, float3(1.0, 1.0, 0.0), (uv.x + uv.y) * first.x * first.y);
	}
	#endif

	fragColor = float4(col,1.0);
	return fragColor;
}

