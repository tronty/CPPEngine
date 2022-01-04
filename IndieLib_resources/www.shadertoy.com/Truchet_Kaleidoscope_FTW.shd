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
float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;

// https://www.shadertoy.com/view/7lKSWW
// CC0: Truchet + Kaleidoscope FTW
//  Bit of experimenting with kaleidoscopes and truchet turned out nice
//  Quite similar to an earlier shader I did but I utilized a different truchet pattern this time
#define PI              3.141592654
#define TAU             (2.0*PI)
#define ROT(a)          float2x2(cos(a), sin(a), -sin(a), cos(a))
#define PCOS(x)         (0.5+0.5*cos(x))

// License: Unknown, author: Unknown, found: don't remember
float4 alphaBlend(float4 back, float4 front) {
  float w = front.w + back.w*(1.0-front.w);
  float3 xyz = (front.xyz*front.w + back.xyz*back.w*(1.0-front.w))/w;
  return w > 0.0 ? float4(xyz, w) : float4(0.0);
}

// License: Unknown, author: Unknown, found: don't remember
float3 alphaBlend(float3 back, float4 front) {
  return mix(back, front.xyz, front.w);
}

// License: Unknown, author: Unknown, found: don't remember
float hash(float co) {
  return fract(sin(co*12.9898) * 13758.5453);
}

// License: Unknown, author: Unknown, found: don't remember
float hash(float2 p) {
  float a = dot(p, float2 (127.1, 311.7));
  return fract(sin (a)*43758.5453123);
}

// License: Unknown, author: Unknown, found: don't remember
float tanh_approx(float x) {
  //  Found this somewhere on the interwebs
  //  return tanh(x);
  float x2 = x*x;
  return clamp(x*(27.0 + x2)/(27.0+9.0*x2), -1.0, 1.0);
}

// License: MIT, author: Inigo Quilez, found: https://www.iquilezles.org/www/articles/smin/smin.htm
float pmin(float a, float b, float k) {
  float h = clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);
  return mix(b, a, h) - k*h*(1.0-h);
}

// License: MIT, author: Inigo Quilez, found: https://www.iquilezles.org/www/index.htm
float3 postProcess(float3 col, float2 q) {
  col = clamp(col, 0.0, 1.0);
  col = pow(col, float3(1.0/2.2));
  col = col*0.6+0.4*col*col*(3.0-2.0*col);
  col = mix(col, float3(dot(col, float3(0.33))), -0.4);
  col *=0.5+0.5*pow(19.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.7);
  return col;
}

float pmax(float a, float b, float k) {
  return -pmin(-a, -b, k);
}

float pabs(float a, float k) {
  return pmax(a, -a, k);
}

float2 toPolar(float2 p) {
  return float2(length(p), atan(p.y, p.x));
}

float2 toRect(float2 p) {
  return float2(p.x*cos(p.y), p.x*sin(p.y));
}

// License: MIT OR CC-BY-NC-4.0, author: mercury, found: https://mercury.sexy/hg_sdf/
float modMirror1(inout float p, float size) {
  float halfsize = size*0.5;
  float c = floor((p + halfsize)/size);
  p = mod(p + halfsize,size) - halfsize;
  p *= mod(c, 2.0)*2.0 - 1.0;
  return c;
}

float smoothKaleidoscope(inout float2 p, float sm, float rep) {
  float2 hp = p;

  float2 hpp = toPolar(hp);
  float rn = modMirror1(hpp.y, TAU/rep);

  float sa = PI/rep - pabs(PI/rep - abs(hpp.y), sm);
  hpp.y = sign(hpp.y)*(sa);

  hp = toRect(hpp);

  p = hp;

  return rn;
}

// The path function
float3 offset(float z) {
  float a = z;
  float2 p = -0.075*(float2(cos(a), sin(a*sqrt(2.0))) + float2(cos(a*sqrt(0.75)), sin(a*sqrt(0.5))));
  return float3(p, z);
}

// The derivate of the path function
//  Used to generate where we are looking
float3 doffset(float z) {
  float eps = 0.1;
  return 0.5*(offset(z + eps) - offset(z - eps))/eps;
}

// The second derivate of the path function
//  Used to generate tilt
float3 ddoffset(float z) {
  float eps = 0.1;
  return 0.125*(doffset(z + eps) - doffset(z - eps))/eps;
}

float2 cell_df(float r, float2 np, float2 mp, float2 off) {
  const float2 n0 = normalize(float2(1.0, 1.0));
  const float2 n1 = normalize(float2(1.0, -1.0));

  np += off;
  mp -= off;
  
  float hh = hash(np);
  float h0 = hh;

  float2  p0 = mp;  
  p0 = abs(p0);
  p0 -= 0.5;
  float d0 = length(p0);
  float d1 = abs(d0-r); 

  float dot0 = dot(n0, mp);
  float dot1 = dot(n1, mp);

  float d2 = abs(dot0);
  float t2 = dot1;
  d2 = abs(t2) > sqrt(0.5) ? d0 : d2;

  float d3 = abs(dot1);
  float t3 = dot0;
  d3 = abs(t3) > sqrt(0.5) ? d0 : d3;


  float d = d0;
  d = min(d, d1);
  if (h0 > .85)
  {
    d = min(d, d2);
    d = min(d, d3);
  }
  else if(h0 > 0.5)
  {
    d = min(d, d2);
  }
  else if(h0 > 0.15)
  {
    d = min(d, d3);
  }
  
  return float2(d, d0-r);
}

float2 truchet_df(float r, float2 p) {
  float2 np = floor(p+0.5);
  float2 mp = fract(p+0.5) - 0.5;
  return cell_df(r, np, mp, float2(0.0));
}

float4 plane(float3 ro, float3 rd, float3 pp, float3 off, float aa, float n) {
  float h_ = hash(n);
  float h0 = fract(1777.0*h_);
  float h1 = fract(2087.0*h_);
  float h2 = fract(2687.0*h_);
  float h3 = fract(3167.0*h_);
  float h4 = fract(3499.0*h_);

  float l = length(pp - ro);

  float3 hn;
  float2 p = (pp-off*float3(1.0, 1.0, 0.0)).xy;
  p *= ROT(0.5*(h4 - 0.5)*iTime);
  float rep = 2.0*round(mix(5.0, 30.0, h2));
  float sm = 0.05*20.0/rep;
  float sn = smoothKaleidoscope(p, sm, rep);
  p *= ROT(TAU*h0+0.025*iTime);
  float z = mix(0.2, 0.4, h3);
  p /= z;
  p+=0.5+floor(h1*1000.0);
  float tl = tanh_approx(0.33*l);
  float r = mix(0.30, 0.45, PCOS(0.1*n));
  float2 d2 = truchet_df(r, p);
  d2 *= z;
  float d = d2.x;
  float lw =0.025*z; 
  d -= lw;
  
  float3 col = mix(float3(1.0), float3(0.0), smoothstep(aa, -aa, d));
  col = mix(col, float3(0.0), smoothstep(mix(1.0, -0.5, tl), 1.0, sin(PI*100.0*d)));
//  float t0 = smoothstep(aa, -aa, -d2.y-lw);
  col = mix(col, float3(0.0), step(d2.y, 0.0));
  //float t = smoothstep(3.0*lw, 0.0, -d2.y);
//  float t = smoothstep(aa, -aa, -d2.y-lw);
  float t = smoothstep(aa, -aa, -d2.y-3.0*lw)*mix(0.5, 1.0, smoothstep(aa, -aa, -d2.y-lw));
  return float4(col, t);
}

float3 skyColor(float3 ro, float3 rd) {
  float d = pow(max(dot(rd, float3(0.0, 0.0, 1.0)), 0.0), 20.0);
  return float3(d);
}

float3 color(float3 ww, float3 uu, float3 vv, float3 ro, float2 p) {
  float lp = length(p);
  float2 np = p + 1.0/iResolution.xy;
  float rdd = (2.0+1.0*tanh_approx(lp));
//  float rdd = 2.0;
  float3 rd = normalize(p.x*uu + p.y*vv + rdd*ww);
  float3 nrd = normalize(np.x*uu + np.y*vv + rdd*ww);

  const float planeDist = 1.0-0.25;
  const int furthest = 6;
  const int fadeFrom = max(furthest-5, 0);

  const float fadeDist = planeDist*float(furthest - fadeFrom);
  float nz = floor(ro.z / planeDist);

  float3 skyCol = skyColor(ro, rd);


  float4 acol = float4(0.0);
  const float cutOff = 0.95;
  bool cutOut = false;

  // Steps from nearest to furthest plane and accumulates the color 
  for (int i = 1; i <= furthest; ++i) {
    float pz = planeDist*nz + planeDist*float(i);

    float pd = (pz - ro.z)/rd.z;

    if (pd > 0.0 && acol.w < cutOff) {
      float3 pp = ro + rd*pd;
      float3 npp = ro + nrd*pd;

      float aa = 3.0*length(pp - npp);

      float3 off = offset(pp.z);

      float4 pcol = plane(ro, rd, pp, off, aa, nz+float(i));

      float nz = pp.z-ro.z;
      float fadeIn = smoothstep(planeDist*float(furthest), planeDist*float(fadeFrom), nz);
      float fadeOut = smoothstep(0.0, planeDist*0.1, nz);
      pcol.xyz = mix(skyCol, pcol.xyz, fadeIn);
      pcol.w *= fadeOut;
      pcol = clamp(pcol, 0.0, 1.0);

      acol = alphaBlend(pcol, acol);
    } else {
      cutOut = true;
      break;
    }

  }

  float3 col = alphaBlend(skyCol, acol);
// To debug cutouts due to transparency  
//  col += cutOut ? float3(1.0, -1.0, 0.0) : float3(0.0);
  return col;
}

float3 effect(float2 p, float2 q) {
  float tm  = iTime*0.25;
  float3 ro   = offset(tm);
  float3 dro  = doffset(tm);
  float3 ddro = ddoffset(tm);

  float3 ww = normalize(dro);
  float3 uu = normalize(cross(normalize(float3(0.0,1.0,0.0)+ddro), ww));
  float3 vv = normalize(cross(ww, uu));

  float3 col = color(ww, uu, vv, ro, p);
  
  return col;
}

float4 main(float2 fragCoord : TEXCOORD0) : COLOR0
{
  float2 q = fragCoord/iResolution.xy;
  float2 p = -1. + 2. * q;
  p.x *= iResolution.x/iResolution.y;
  
  float3 col = effect(p, q);
  col *= smoothstep(0.0, 4.0, iTime);
  col = postProcess(col, q);
 
  return float4(col, 1.0);
}

