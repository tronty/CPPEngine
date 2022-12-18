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

// https://www.shadertoy.com/view/fsXczN
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

// https://www.shadertoy.com/view/NdjyDV
// Swirl planet
uint seed;
uint col_seed;
float pix;
#define uvec3 float3
#define uvec4 float4

// 3D hash function to simulate seeding:
// https://www.shadertoy.com/view/XlGcRh
uvec3 pcg3d(uvec3 v)
{
  v = v * 1664525u + 1013904223u;

  v.x += v.y * v.z;
  v.y += v.z * v.x;
  v.z += v.x * v.y;

  v ^= v >> 16u;

  v.x += v.y * v.z;
  v.y += v.z * v.x;
  v.z += v.x * v.y;

  return v;
}

// https://www.shadertoy.com/view/XlGcRh
float hash(float2 s, uint hash_seed)
{
  float res;
  uvec4 u = uvec4(s, uint(s.x) ^ uint(s.y), uint(s.x) + uint(s.y));
  uvec3 p = pcg3d(uvec3(u.x, u.y, hash_seed));
  res = float(p) * (1.0 / float(0xffffffffu));
  return res;
}

float noise(float2 coord, uint noise_seed)
{
  float2 i = floor(coord);
  float2 f = frac(coord);
  f = f * f * (3.0 - 2.0 * f);

  float a = hash(i, noise_seed);
  float b = hash(i + float2(1.0, 0.0), noise_seed);
  float c = hash(i + float2(0.0, 1.0), noise_seed);
  float d = hash(i + float2(1.0, 1.0), noise_seed);

  return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}

// https://iquilezles.org/www/articles/smin/smin.htm
float smin(float a, float b, float k)
{
  float h = max(k - abs(a - b), 0.0);
  return min(a, b) - h * h * 0.25 / k;
}

float2 rotation(float2 p, float a)
{
  return p * float2x2(cos(a), -sin(a),
                  sin(a),  cos(a));
}

float2 spherify(float2 uv, float2 center, float radius)
{
  float2 centered = (uv - center) * 2.;
  float z = sqrt(radius * radius * 4. - dot(centered.xy, centered.xy));
  float2 sphere = centered / (z + 1.0);
  return sphere * 0.5 + 0.5;
}

// https://www.shadertoy.com/view/NsfyDs
float circles( float2 p, float r, uint s)
{
  float2 i = float2(floor(p)), f = frac(p), h, a;

  float d = 1e9, c, rad;
  for(int k = 0; k < 9; k++)
  {
    p = float2(mod(float(k) , 3.0), k / 3) - 1.;
    rad = hash(i + p, s + 2u) * r;
    h = float2(hash(i + p, s + 89u), hash(i + p, s + 52u));

    // movement
    a = float2(hash(i + p, s + 25u), hash(i + p, s + 215u));
    a = .3 * cos(5. * (a.x - .5) * iTime * 0.5 + 6.3 * a.y + float2(0., 11.));
    p += .1 + .8 * h - f + a;

    c = length(p) - rad;
    d = smin(d, c, 0.3);
  }
  return d;
}

// https://iquilezles.org/www/articles/fbmsdf/fbmsdf.htm
float fbmCircles(float2 p, uint se)
{
  float s = 1., d = 1.;
  int o = 2;
  for(int i = 0; i < o; i++)
  {
    float n = s*(circles(p, .5, se));

    d = smin(d, n, 0.3 * s);

    p *= 2.;
    s = 0.5 * s;
  }
  return d;
}

// original author warned against discontinuities that I did not fix:
// https://www.shadertoy.com/view/fsKSWD
float2 swirls(float2 p, out float4 O, uint se, float sz, float ro)
{
  float r;
  float s = .012 * sz;

  p = p * s * R.y;
  float2 F = abs(frac(p + .5) - .5);
  float y = min(F.x, F.y);
  O += smoothstep(12. / R.y, 0., y);
  float2 I = floor(p);
  F = frac(p);

  float d = 3.*ro;
  y = d * cos(d * y);
  p -= p;
  float2 D;
  int k;
  for (k = 0; k < 9; k++)
  {
    D = float2(mod(float(k) , 3.0), k / 3) - 1.;
    D += hash(I + D, se + 222u);
    r = length(F - D) * (1. + hash(I + D, se + 72u));
    F = rotation(F - D, y * smoothstep(.5, 0., r)) + D;
    p = F + I;
  }
  return p / (s * R.y);
}

float2 fbmSwirls(float2 p, out float4 O, uint se)
{
  uint o = 3u;
  O -= O;
  p += float2(iTime * 0.02, 0.);
  float sz = 1., ro = 1.;
  for(uint i = 0u; i < o; i++)
  {
    p = swirls(p, O, se + i, sz, ro);
    sz *= 2.;
  }
  return p;
}

float3 hsv2rgb(in float3 c)
{
  float3 rgb = clamp(abs(mod(c.x * 6.0
    + float3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0 );
  return c.z * lerp( float3(1.0), rgb, c.y);
}

// https://www.slynyrd.com/blog/2018/1/10/pixelblog-1-color-palettes
float3 planet_color(float sm)
{
  float hu, sa, br;
  if (col_seed < 1u)
  {
    hu = radians(3.1415926 *2. *(5.5 + sm * 0.5));
  } else {
    hu = radians(3.1415926 * 2. * (9. * hash(float2(1.), col_seed)
      + sm * (hash(float2(10.), col_seed + 5u) * 2. - 1.)));
  }
  if (sm < 2.5)
  {
    sa = 0.2 + 0.075 * sm;
  } else if (sm < 3.5) {
    sa = 0.35;
  } else if (sm < 4.5) {
    sa = 0.4;
  } else {
    sa = 0.425 - 0.07 * (sm - 4.);
  }
  if (sm < 3.5)
  {
    br = 0.15 + 0.15 * sm;
  } else if (sm < 6.5) {
    br = 0.6 + 0.1 * (sm - 3.);
  } else {
    br = .9 + 0.05 * (sm - 6.);
  }
  return hsv2rgb(float3(hu, sa, br));
}

// https://www.shadertoy.com/view/MslGWN
float3 nrand3(float2 co)
{
  float a = hash(co, 98u);
  float b = hash(co, 99u);
  float c = lerp(a, b, 0.5);
  return float3(c);
}

// https://www.shadertoy.com/view/MslGWN
float4 stars(float2 uv)
{
  float3 rnd = nrand3(uv);
  float r = rnd.y;
  float4 starcolor = float4(r * r * r * r * r);

  if (starcolor.x > 0.3)
  {
    float brighness_variance = max(0.15, hash(uv, 94u) / 2.0f);
    return starcolor + float4(abs(sin((iTime*10. + hash(uv, 94u)) *
      (hash(uv, 95u) + 1.))) * brighness_variance
      - (brighness_variance / 2.));
  } else {
    return float4(0.);
  }
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 u=IN.uv;
  seed = uint(round(abs(tex2D(iChannel0, int2(u)).x)));
  col_seed = uint(round(abs(tex2D(iChannel0, int2(u)).y)));
  pix = round(tex2D(iChannel0, int2(u)).z);

  float2 bU = u / R.y;
  float2 U = floor(bU * pix) / pix;

  if (distance(U, R / (2. * R.y)) > 0.425)
  {
    U *= R.y;
    float sta = stars(U).x*noise(U * 0.1, 182u);
    O = float4(2. * sta * noise(U * 0.025, 47u),
      sta * 1.5 * noise(U * 0.025, 52u),
      sta * 3., 1.);
    return;
  }
  bool dith = mod(bU.x + U.y, 2. / pix) < 1. / pix;

  U = spherify(U, R / (2. * R.y), 0.425);
  float2 aU = fbmSwirls(U, O, seed + 151u);
  float g = fbmCircles(aU * 10. + float2(iTime * 0.2, 0.), seed + 151u);
  g = -smin(1., g, 3.2);

  float sm = sqrt(sqrt(max(g, 0.025))) * 12.;

  // light and dithering
  float d_light = distance(U, iMouse.xy / R);
  float light_b =
    max(0.8 - d_light + (noise(U * 15., seed + 15u) - 0.5) * 0.2, 0.05);
  light_b = sqrt(light_b);
  sm = dith ? 0.95 * sm * light_b : sm * light_b;

  sm = sm - mod(sm, sqrt(sqrt(light_b))) + 1.;

  return float4(planet_color(sm), 1.);
}
