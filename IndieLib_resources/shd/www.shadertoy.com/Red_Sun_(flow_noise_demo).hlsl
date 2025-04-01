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

//[Vertex shader]
//ROW_MAJOR 
float4x4 worldViewProj;
// MVPSEMANTIC;
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

//[Fragment shader]
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
#define int2 float2
sampler2D iChannel0;
float     iTime=0;                        // shader playback time (in seconds)
float2      iResolution=float2(1,1);                  // viewport resolution (in pixels)
float2 iMouse=float2(1,1);

// psrdnoise ???
float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float sphere_size = 1.0; // Unit sphere
    // The number of facets can be anything from 8 to 2e6
    const float FibN = 471.0;

    float3 bgcolor = float3(0.0, 0.0, 0.0);

    // Default view: fit unit circle in viewport
    float3 uvw;
    uvw.xy = (2.0*fragCoord- iResolution.xy)/min(iResolution.x, iResolution.y);

    // Adjust scale by mouse drags (down-drag zooms out, up-drag zooms in)
    float zoom = 1.1*pow(0.98,iMouse.y-abs(iMouse.w));
    uvw.xy = zoom*uvw.xy;
    float sphere_mask = 1.0 - aastep(sphere_size, length(uvw.xy)); // uv is scaled but not rotated
    if(sphere_mask == 0.0) { // Early exit outside object - big saving in distant views
        return float4(bgcolor, 1.0);
        return;
    }

    uvw.z = sqrt(max(0.0, sphere_size*sphere_size-dot(uvw.xy,uvw.xy))); // Fake sphere z in unit circle

    // Spin vertically (rotate only the texcoords)
    float el = 3.5*sin(iTime*0.02*sqrt(3.0));
    float Se = sin(el);
    float Ce = cos(el);
    // Spin horizontally
    float az = 7.0*sin(iTime*0.02*sqrt(2.0));
    float Sa = sin(az);
    float Ca = cos(az);
    float3x3 R = float3x3(1.0,0.0,0.0, 0.0,Ce,-Se, 0.0,Se,Ce);
    R *= float3x3(Ca,0.0,Sa, 0.0,1.0,0.0, -Sa,0.0,Ca);

    float3 v = normalize(mul(R, uvw)); // Rescale coords to unit sphere

	float n;
    float t = iTime;
	float3 g, g1;
    float3 p = float3(0.0, 0.0, 0.0);
    float w = 0.025;
    
	n = psrdnoise(v, p, t, g);
	n += 0.5*psrdnoise(v*2.0+g*w, p, t*1.0, g1);
	g += g1;
	n += 0.25*psrdnoise(v*4.0+g*w, p, t*2.0, g1);
	g += g1;
	n += 0.125*psrdnoise(v*8.0-g*w, p, t*3.0, g1);
	g += g1;
	n += 0.0625*psrdnoise(v*16.0-g*w, p, t*4.0, g1);
	g += g1;
	float m = 0.1*n+0.1*length(g);

    const float3 darkred = float3(0.5, 0.0, 0.0);
	const float3 brightred = float3(1.0, 0.3, 0.0);
	float3 mixcolor;
    mixcolor = lerp(darkred, brightred, m);
    mixcolor = lerp(bgcolor, mixcolor, sphere_mask);

    return float4(mixcolor, 1.0);
}

