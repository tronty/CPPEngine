/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
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

// fibnoise ???
float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float sphere_size = 1.0; // Unit sphere
    // The number of facets can be anything from 8 to 2e6
    const float FibN = 471.0;

    float3 bgcolor = float3(0.0, 0.0, 0.3);

    // Default view: fit unit circle in viewport
    float3 uvw;
    uvw.xy = (2.0*fragCoord- iResolution.xy)/min(iResolution.x, iResolution.y);

    // Adjust scale by mouse drags (down-drag zooms out, up-drag zooms in)
    float zoom = pow(0.98,iMouse.y-abs(iMouse.w));
    uvw.xy = zoom*uvw.xy;
    float sphere_mask = 1.0 - aastep(sphere_size, length(uvw.xy)); // uv is scaled but not rotated
    if(sphere_mask == 0.0) { // Early exit outside object - big saving in distant views
        return float4(bgcolor, 1.0);
        return;
    }

    uvw.z = sqrt(max(0.0, sphere_size*sphere_size-dot(uvw.xy,uvw.xy))); // Fake sphere z in unit circle

    // Spin vertically (rotate only the texcoords)
    float el = 3.5*sin(iTime*0.1*sqrt(3.0));
    float Se = sin(el);
    float Ce = cos(el);
    // Spin horizontally
    float az = 7.0*sin(iTime*0.1*sqrt(2.0));
    float Sa = sin(az);
    float Ca = cos(az);
    float3x3 R = float3x3(1.0,0.0,0.0, 0.0,Ce,-Se, 0.0,Se,Ce);
    R *= float3x3(Ca,0.0,Sa, 0.0,1.0,0.0, -Sa,0.0,Ca);
    float3x3 Ri = transpose(R); // orthonormal, hence inverse = tranpose

    float3 v = normalize(mul(R, uvw)); // Rescale coords to unit sphere

    float4x4 p;
    float4 d;
	// Find the three closest N-fib-sphere points to v
	inverseSF(v, FibN, p, d);
    patchpoles(v, FibN, p, d);
    
	float n;
	float3 vw, g, gtemp;

	float warp = 1.0;
	n = fibnoise(v, FibN, iTime, g);
    // Display one component of noise on the left half,
    // and a more interesting fractal sum on the right.
    if(uvw.x > 0.0) {
        vw = normalize(v - warp*g);
        n += 0.5*fibnoise(vw, FibN*4.0, -iTime*1.9, gtemp);
        g += 0.5*gtemp;
        vw = normalize(v - warp*g);
        n += 0.25*fibnoise(vw, FibN*16.0, iTime*3.9, gtemp);
    }

	float3 mixcolor;
	mixcolor = lerp(bgcolor, float3(0.5 - 0.35*n), sphere_mask);

    return float4(mixcolor, 1.0);
}

