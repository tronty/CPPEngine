/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#if 0
#define WSIGN +
#define ROW_MAJOR
#define MVPSEMANTIC
#define fract frac
#define mix lerp
#define atan(x,y) atan2(y,x)
#endif

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
// https://www.shadertoy.com/view/XlXSzf
// explosion no. 12
const int ITERATIONS = 20;
const float STEP = 0.08;
const float DURATION = 5.0;


sampler2D iChannel0;
float2 iChannelResolution=float2(1,1);

//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}

float3 mod289(float3 x)
{
  return x - floor(x / 289.0) * 289.0;
}

float4 mod289(float4 x)
{
  return x - floor(x / 289.0) * 289.0;
}

float4 permute(float4 x){return mod289(((x*34.0)+1.0)*x);}
float4 taylorInvSqrt(float4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(float3 v){ 
  const float2  C = float2(1.0/6.0, 1.0/3.0) ;
  const float4  D = float4(0.0, 0.5, 1.0, 2.0);

// First corner
  float3 i  = floor(v + dot(v, C.yyy) );
  float3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  float3 g = step(x0.yzx, x0.xyz);
  float3 l = 1.0 - g;
  float3 i1 = min( g.xyz, l.zxy );
  float3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  float3 x1 = x0 - i1 + 1.0 * C.xxx;
  float3 x2 = x0 - i2 + 2.0 * C.xxx;
  float3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod289(i); 
  float4 p = permute( permute( permute( 
             i.z + float4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + float4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + float4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  float3  ns = n_ * D.wyz - D.xzx;

  float4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  float4 x_ = floor(j * ns.z);
  float4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  float4 x = x_ *ns.x + ns.yyyy;
  float4 y = y_ *ns.x + ns.yyyy;
  float4 h = 1.0 - abs(x) - abs(y);

  float4 b0 = float4( x.xy, y.xy );
  float4 b1 = float4( x.zw, y.zw );

  float4 s0 = floor(b0)*2.0 + 1.0;
  float4 s1 = floor(b1)*2.0 + 1.0;
  float4 sh = -step(h, float4(0, 0, 0, 0));

  float4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  float4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  float3 p0 = float3(a0.xy,h.x);
  float3 p1 = float3(a0.zw,h.y);
  float3 p2 = float3(a1.xy,h.z);
  float3 p3 = float3(a1.zw,h.w);

//Normalise gradients
  float4 norm = taylorInvSqrt(float4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise_ value
  float4 m = max(0.6 - float4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, float4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

#define ssnoise(x) ((2*snoise(x)*0.5+0.5)-1)

float fbm(float3 x)
{
    float r = 0.0;
    float w = 1.0, s = 1.0;
    for (int i=0; i<5; i++)
    {
        w *= 0.5;
        s *= 2.0;
        r += w * ssnoise(s * x);
    }
    return r;
}

float3 getColor(float r, float d)
{
    float3 c = mix(float3(0.9, 0.9, 0.9), float3(0.5, 0.4, 0.1), d);
    
	c *= mix(float3(1.0, 0.4, 0.0), float3(0.5, 0.5, 0.5), r );
    
	return c * 4.0;
}

float4 scene(float3 pos, float t)
{    
    float density = fbm(4.0*pos);
    
    float rad = length(pos);
    density -= 0.1 + rad * (1.0 - 0.2 * t);
   
    density = clamp(density, 0.0, 1.0);
    density *= (1.0 - smoothstep(0.5, 1.0, rad));
    
    return float4(getColor(rad, density), density);
}

float3 render(float3 eye, float3 dir, float2 fragCoord, float t)
{
    float4 color = float4(0, 0, 0, 0);
    
    float3 pos = eye + dir * STEP * tex2D(iChannel0, fragCoord.xy/iChannelResolution[0].x).r;
    
    for (int i=0; i<ITERATIONS; i++)
    {
        if (color.a > 0.99) continue;
        if (length(pos) > 1.1) continue;
        
        float4 d = scene(pos, t);
        d.rgb *= d.a;
        color += d * (1.0 - color.a);
        
        pos += dir * STEP;
    }
    
    return clamp(color.rgb, 0.0, 1.0);
}

float3 lookAtDir(float2 uv, float3 eye, float3 at, float3 up, float fov)
{
    float3 f = normalize(at - eye);
    float3 r = normalize(cross(up, f));
    float3 u = normalize(cross(f, r));
    return normalize(f + fov * (uv.x*r + uv.y*u));
}

float mod(float x, float y)
{
  return x - y * floor(x / y);
}

const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;
    float2 uv = fragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    
    float angle = iTime * 0.3;
    float3 eye = float3(sin(angle), 0, cos(angle));
    float3 dir = lookAtDir(uv, eye, float3(0, 0, 0), float3(0, 1, 0), 1.0);
    
    float time = mod(iTime, DURATION);
	
    float3 color = render(eye, dir, fragCoord, time);
    
    return float4(color, 1.0);
}
