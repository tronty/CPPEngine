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

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float sphere_size = 1.0; // Unit sphere
    // The number of facets can be anything from 8 to 2e6 (!) but for
    // very large N the facets won't show, making this demo pointless
    const float FibonacciN = 43.0;

    float3 bgcolor = float3(0.0, 0.0, 0.3);
    float3 spherecolor = float3(1.0, 0.5, 0.0);
    float3 dotcolor = float3(0.7,0.7,0.5);

    // Default view: fit unit circle in viewport
    float3 uvw;
    uvw.xy = (2.0*fragCoord- iResolution.xy)/min(iResolution.x, iResolution.y);

    // Adjust scale by mouse drags (down-drag zooms out, up-drag zooms in)
    float zoom = 1.1*pow(0.98,iMouse.y-abs(iMouse.w));
    uvw.xy = zoom*uvw.xy;
    // uv is scaled but not rotated
    float sphere_mask = 1.0 - aastep(sphere_size, length(uvw.xy));
    // Early exit outside object - big saving in distant views
    if(sphere_mask == 0.0) {
        return float4(bgcolor, 1.0);
        return;
    }

    // Fake sphere z in unit circle
    uvw.z = sqrt(max(0.0, sphere_size*sphere_size-dot(uvw.xy,uvw.xy)));

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

	// These feature sizes scale with N
	float edgewidth = 0.5/sqrt(FibonacciN); // Linear distances here
	float spotsize = 0.3/sqrt(FibonacciN); // Remember to use sqrt(d[i])

	// Find the three closest N-fib-sphere points to v
    float4 d;
#undef MAT4ARG
#ifdef MAT4ARG
    float4x4 p;
	inverseSF(v, FibonacciN, p, d);
    patchpoles(v, FibonacciN, p, d);
    sortp0p1(p, d);
    float d0 = length(v-p[0].xyz); // <-- Worley would call this "F1"
    float d1 = length(v-p[1].xyz); // These are "F2" to "F4",
    float d2 = length(v-p[2].xyz); // but we don't know which is
    float d3 = length(v-p[3].xyz); // which (p1 to p3 are not sorted)
#else
    float4 p0, p1, p2, p3; // Dodge a possible bug with float4x4 argument type
	inverseSF(v, FibonacciN, p0, p1, p2, p3, d);
    patchpoles(v, FibonacciN, p0, p1, p2, p3, d);
    sortp0p1(p0, p1, p2, p3, d);
    float d0 = length(v-p0.xyz); // <-- Worley would call this "F1"
	float d1 = length(v-p1.xyz); // These are "F2" to "F4",
	float d2 = length(v-p2.xyz); // but we don't know which is
    float d3 = length(v-p3.xyz); // which (p1 to p3 are not sorted)
#endif

#undef AA
#ifdef AA
	float dots = 1.0-aastep(spotsize, sqrt(d.x));
#else
	float dots = 1.0-step(spotsize, sqrt(d.x)); // AA with aastep disabled
#endif

    // Barycentric coordinates for chamfered normals
	float b0 = smoothstep(-edgewidth, edgewidth, (d1-d0));
	float b1 = smoothstep(-edgewidth, edgewidth, (d0-d1));
	float b2 = smoothstep(-edgewidth, edgewidth, (d0-d2));
	float b3 = smoothstep(-edgewidth, edgewidth, (d0-d3));
    // In most cases, b2 or b3 have no influence, but we don't
    // know which one. Let's be lazy and include both.
	// Note that we renormalize, so we don't need "/bn"
	// float bn = b0 + b1 + b2 + b3;
#ifdef MAT4ARG
	float3 facetN = normalize(p[0].xyz*b0 + p[1].xyz*b1
        + p[2].xyz*b2 + p[3].xyz*b3); // "Chamfered facets"
#else
	float3 facetN = normalize(p0.xyz*b0 + p1.xyz*b1
        + p2.xyz*b2 + p3.xyz*b3); // "Chamfered facets"
#endif

    // Simple lighting
	facetN = Ri*facetN;
	float3 Lpos = float3(3.0, 3.0, 3.0); // Local light source
	float3 L = normalize(Lpos-Ri*v);
	float kd = 0.2 + 0.5*max(dot(L, facetN), 0.0);
	float ks = (1.0-step(kd,0.2))*pow(max(reflect(-L, facetN).z, 0.0),15.0);
	float3 white = float3(1.0, 1.0, 1.0);
	float3 mixcolor = kd*spherecolor + ks*white;
	mixcolor = lerp(mixcolor, dotcolor, dots);
#ifdef AA
    mixcolor = lerp(bgcolor, mixcolor, sphere_mask); // AA of the outline
#endif

    return float4(mixcolor, 1.0);
}

