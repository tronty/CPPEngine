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
float iTime=0.0;
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
sampler2D iChannel1;
sampler2D iChannel2;

// https://www.shadertoy.com/view/4tfBRf
// The One Ring
// ----------------------------------------------------
//  "The One Ring" by Krzysztof Kondrak @k_kondrak
// ----------------------------------------------------

// enable/disable AA
#define ANTIALIASING 1
#define AA_SAMPLES 2
#define AA_WIDTH .8

#define FOV 5.5
#define RING_RADIUS 1.5

// raymarching constants
#define MIN_DIST  .001
#define MAX_DIST  30.
#define NUM_STEPS 100
#define BACKGROUND_ID 0
#define RING_ID       1

// Gold color: https://www.shadertoy.com/view/XdVSRV
float3 GOLD1 = float3(1.1,  0.91, 0.52);
float3 GOLD2 = float3(1.1,  1.07, 0.88);
float3 GOLD3 = float3(1.02, 0.82, 0.55);

// initial eye/camera position
float3 EYE = float3(7.5, 0., 0.);

// helper struct to collect raymarching data
struct RMInfo
{
  	float3 pos;
  	float3 normal;
  	int  objId;
};

// ------------------
//  1D hash function
// ------------------
float hash(float n)
{
    return frac(sin(n)*753.5453123);
}

// ----------------------------------------------
//  noise: https://www.shadertoy.com/view/4sfGzS
// ----------------------------------------------
float noise(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);
	
    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return lerp(lerp(lerp(hash(n +   0.0), hash(n +   1.0), f.x),
                   lerp(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
               lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
                   lerp(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

// -----------------
//  vector rotation
// -----------------
float2 rotate(float2 v, float a)
{
    return float2(v.x * cos(a) - v.y * sin(a), v.x * sin(a) + v.y * cos(a));
}

// --------------------
//  "noisy" gold color
// --------------------
float3 Gold(float3 p)
{
    p += .4 * noise(p * 24.);
    float t = noise(p * 30.);
    float fade = max(0., sin(iTime * .3));

    float3 gold = lerp(GOLD1, GOLD2, smoothstep(.55, .95, t));
    gold = lerp(gold, GOLD3, smoothstep(.45, .25, t));

	// Glowing "black speech" inscription on the ring.
    // Flicker depends on the current audio value.
    if(p.y > .18 && p.y < .23)
    {
    	gold +=  8. * fade * float3(1., .3, 0.) * (1. + 10. * tex2D(iChannel1, float2(50., 0.)).r);
    }

    // darker gold tint if the inscription is visible
    gold *= (1. - 0.666 * fade);

    return gold;
}

// ----------------------------------------
//  calculate ray direction for eye/camera
// ----------------------------------------
float3 EyeRay(float2 fragCoord, float3 eyeDir)
{
  	float2 uv = fragCoord.xy / iResolution.xy; 
  	uv = uv * 2.0 - 1.0;
  	uv.x *= iResolution.x / iResolution.y;

    float3 forward = normalize(eyeDir);
	float3 right   = normalize(cross(float3(.0, 1., .0), forward));
	float3 up      = normalize(cross(forward, right));    

	return normalize(uv.x * right + uv.y * up + forward * FOV);
}

// ----------------------------------------------
//  SDF for the ring - a slightly deformed torus
// ----------------------------------------------
float Ring(float3 pos)
{
    float2 t = float2(RING_RADIUS, RING_RADIUS * .2);
    float2 q = float2(clamp(2. * (length(pos.xz) - t.x), -5., 5.),pos.y);

    return length(q) - t.y;
}

// -------------------------------
//  flickering hellish background
// -------------------------------
float3 Background(float3 ray)
{ 
    return tex2D(iChannel2, ray).rgb * float3(.7, .15, .0) * (1. + tex2D(iChannel1, float2(0., 0.)).r);
}

// ----------------
//  surface normal
// ----------------
float3 SurfaceNormal(in float3 pos)
{
    float3 eps = float3( MIN_DIST, 0., 0. );
    return normalize(-float3(Ring(pos + eps.xyy) - Ring(pos - eps.xyy),
                           Ring(pos + eps.yxy) - Ring(pos - eps.yxy),
                           Ring(pos + eps.yyx) - Ring(pos - eps.yyx)));
}

// ------------------
//  scene raymarcher
// ------------------
RMInfo Raymarch(float3 from, float3 to)
{
    float t = 0.;
    int objId = BACKGROUND_ID;
    float3 pos;
    float3 normal;
    float dist;
    
  	for (int i = 0; i < NUM_STEPS; ++i)
    {
    	pos = from + to * t;
        dist = Ring(pos);

        if (dist > MAX_DIST || abs(dist) < MIN_DIST)
            break;

        t += dist * 0.43;
        objId = RING_ID;
  	}
    
    if (t < MAX_DIST)
    {
        normal = SurfaceNormal(pos);
    }
    else
    {
        objId = BACKGROUND_ID;
    }

    return RMInfo(pos, normal, objId);
}


// -------------------------
//  here be scene rendering
// -------------------------
float4 Draw(float2 fragCoord)
{   
    float3   col = float3(0.);
  	float3   ray = EyeRay(fragCoord, -EYE);
  	RMInfo rmi = Raymarch(EYE, ray);

    if (rmi.objId == RING_ID)
    {
        col = lerp(col, Gold(rmi.pos) * tex2D(iChannel0, reflect(ray, rmi.normal)).rgb, .99);            
        rmi = Raymarch(rmi.pos, reflect(ray, rmi.normal));
    }
    else if(rmi.objId == BACKGROUND_ID)
    {
        col += Background(ray);
    }

  	return float4(col, 1.0);
}

float4 main(float2 fragCoord : TEXCOORD0) : COLOR0
{
	float4 fragColor;
    EYE.xy = rotate(EYE.xy, iTime * .03 + .015);
    EYE.yz = rotate(EYE.yz, iTime * .03 + .015);

    // Antialiasing: https://www.shadertoy.com/view/XdVSRV
#if ANTIALIASING
    float4 vs = float4(0.);
    for (int j = 0; j < AA_SAMPLES ;j++)
    {
        float oy = float(j) * AA_WIDTH / max(float(AA_SAMPLES - 1), 1.);
        for (int i = 0; i < AA_SAMPLES; i++)
        {
            float ox = float(i) * AA_WIDTH / max(float(AA_SAMPLES - 1), 1.);
            vs += Draw(fragCoord + float2(ox, oy));
        }
    }

    fragColor = vs/float4(AA_SAMPLES * AA_SAMPLES);
#else
    fragColor = Draw(fragCoord);
#endif
    return fragColor;
}
