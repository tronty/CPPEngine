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

float2 mouse=float2(1, 1);
float2 resolution=float2(1, 1);
float time=0.0;

// random/hash function              
float hash( float n )
{
  return frac(cos(n)*41415.92653);
}

// 3d noise function
float noise( in float3 x )
{
  float3 p  = floor(x);
  float3 f  = smoothstep(0.0, 1.0, frac(x));
  float n = p.x + p.y*57.0 + 113.0*p.z;

  return lerp(lerp(lerp( hash(n+  0.0), hash(n+  1.0),f.x),
    lerp( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
    lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
    lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

float sdf_wave(float3 p)
{
    float2x2 m2 = float2x2(1.6,-1.2,1.2,1.6); // 2 + 
    float speed = 3.141592 / 2.0;
    float waveHeight = .5;

    float wave;
    for (int i = 0; i < 4; i++)
    {
      p.xz = mul(m2, p.xz);
      wave += p.y - waveHeight * sin(p.x * 0.07 + p.z * 0.21  - time * speed);
      waveHeight *= 0.6;
    }
    // wave += 12.0;
    wave += noise(p) * 0.03;
    return wave;
}

float3 getNormal(float3 p)
{
    float diff = 0.01;
    return normalize(float3(
        sdf_wave(p + float3(diff, 0, 0)) - sdf_wave(p - float3(diff, 0, 0)),
        sdf_wave(p + float3(0, diff, 0)) - sdf_wave(p - float3(0, diff, 0)),
        sdf_wave(p + float3(0, 0, diff)) - sdf_wave(p - float3(0, 0, diff))
    ));    
}

#define SKY_COLOR float3(19, 27, 44) / 255.0
#define SUN_COLOR_1 float3(246, 254, 207) / 255.0
#define SUN_COLOR_2 float3(182, 149, 95) / 255.0
// #define SUN_COLOR_3 float3(96, 57, 28) / 255.0
#define SUN_COLOR_3 float3(105, 54, 25) / 255.0
#define SUN_COLOR_SCALE 4.0
#define SKY_COLOR_HORIZON_1 float3(96, 57, 28) / 255.0
#define SKY_COLOR_HORIZON_2 float3(105, 54, 25) / 255.0

#define WATER_BASE_COLOR float3(15, 21, 21)

// 
void raymarch(float3 cPos, float3 rd, out float isHit, out float3 rp)
{
    if (rd.y > 0.0) // RayRaymarch
    {
        isHit = 0.0;
        return;
    }

    float t = 0.0;
    // float isHit = 0.0;
    float st = 0.06;
    for (int i = 0; i < 500; i++)
    {
        if (i > 400) st = 0.7;

        rp = cPos + t * rd; // 
        if (sdf_wave(rp) < 0.001)
        {
            isHit = 1.0;
            break;
        }
        t += st;
    }
}

float3 renderSky(float2 p, float3 rd, float3 l)
{
    float sundot = clamp(dot(rd, l), 0.0, 1.0);

    float3 sunGrad1 = pow(sundot, 350.0) * SUN_COLOR_1;
    float3 sunGrad2 = pow(sundot, 15.0) * SUN_COLOR_2 * 0.7;
    float3 sunGrad3 = pow(sundot, 4.0) * SUN_COLOR_3 * 0.7;

    return sunGrad1 + sunGrad2 + sunGrad3 + SKY_COLOR;
}


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 p = (fragCoord.xy * 2.0 - resolution.xy) / max(resolution.x, resolution.y);
    float3 cPos = float3(0,1.2,0); // camera position
    float3 cDir = normalize(float3(0,0.15,1)); // camera direction
    float3 cSide = normalize(cross(cDir, float3(0,1,0))); // camera side
    float3 cUp = normalize(cross(cSide, cDir)); // camera up

    float3 rd = normalize(cSide * p.x + cUp * p.y + cDir); // ray direction
    float3 rp; // ray position
    float isHit; // Ray
    raymarch(cPos, rd, isHit, rp);

    // Light vector
    float3 l = normalize(float3(0, 0.25, 1.0)); // 

    // Sky
    float3 skyColor = renderSky(p, rd, l);

    // Bling-Phonn Shading
    float3 n = getNormal(rp); // Ray
    float3 r = reflect(-l, n);  // 
    float d = 0.5; // 
    float s = 2.0; // 
    float a = 0.1; // 
    float3 h = normalize(r + rd); // Bling-Phonn
    float nDotL = clamp(dot(n, l), 0.0, 1.0);
    float nDotH = clamp(dot(n, h), 0.0, 1.0);
    float nDotV = clamp(dot(rd, n), 0.0, 1.0);
    float3 waterColor = 
        + SKY_COLOR * a  + SKY_COLOR * d * nDotL + SUN_COLOR_2 * s * pow(nDotH, 50.0) // Bling-Phonn
        + SKY_COLOR_HORIZON_1 * nDotV * 4.0; // Specular (Phonn)

    waterColor = lerp(waterColor, skyColor, 0.45);

    fragColor = float4(lerp(skyColor, waterColor, isHit), 1.0);
	return fragColor;

}
