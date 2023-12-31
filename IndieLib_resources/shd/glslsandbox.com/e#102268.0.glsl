#if 0
#extension GL_OES_standard_derivatives : enable

precision highp float;

//uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#endif

// CC0: Colorful star travelling
//  Inspired by Xor's colorful tweet:
//  https://twitter.com/XorDev/status/1621603585996775430?s=20&t=omyxMpuu2nELIw6_AEi72A

#define RESOLUTION  resolution
#define TIME        10.0*time
#define PI          3.141592654
#define TAU         (2.0*PI)
#define ROT(a)      mat2(cos(a), sin(a), -sin(a), cos(a))
#define BPM         (145.0*0.5)

const float planeDist = 1.0-0.82;


// License: WTFPL, author: sam hocevar, found: https://stackoverflow.com/a/17897228/418488
const vec4 hsv2rgb_K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
vec3 hsv2rgb(vec3 c) {
  vec3 p = abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www);
  return c.z * mix(hsv2rgb_K.xxx, clamp(p - hsv2rgb_K.xxx, 0.0, 1.0), c.y);
}
// License: WTFPL, author: sam hocevar, found: https://stackoverflow.com/a/17897228/418488
//  Macro version of above to enable compile-time constants
#define HSV2RGB(c)  (c.z * mix(hsv2rgb_K.xxx, clamp(abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www) - hsv2rgb_K.xxx, 0.0, 1.0), c.y))

// License: Unknown, author: nmz (twitter: @stormoid), found: https://www.shadertoy.com/view/NdfyRM
vec3 sRGB(vec3 t) {
  return mix(1.055*pow(t, vec3(1./2.4)) - 0.055, 12.92*t, step(t, vec3(0.0031308)));
}
// License: Unknown, author: Matt Taylor (https://github.com/64), found: https://64.github.io/tonemapping/
vec3 aces_approx(vec3 v) {
  v = max(v, 0.0);
  v *= 0.6;
  float a = 2.51;
  float b = 0.03;
  float c = 2.43;
  float d = 0.59;
  float e = 0.14;
  return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0, 1.0);
}

// License: Unknown, author: Unknown, found: don't remember
float hash3(vec3 r)  {
  return fract(sin(dot(r.xy,vec2(1.38984*sin(r.z),1.13233*cos(r.z))))*653758.5453);
}

// License: MIT OR CC-BY-NC-4.0, author: mercury, found: https://mercury.sexy/hg_sdf/
vec2 mod2(inout vec2 p, vec2 size) {
  vec2 c = floor((p + size*0.5)/size);
  p = mod(p + size*0.5,size) - size*0.5;
  return c;
}

vec2 soff(float h0) {
  float h1 = fract(h0*8677.0);
  return -1.0+2.0*vec2(h0, h1);
}

vec3 offset(float z) {
  float a = z;
  vec2 p = -mix(0.0, 0.1, smoothstep(40.0, 50.0, TIME))*(vec2(cos(a), sin(a*sqrt(2.0))) + vec2(cos(a*sqrt(0.75)), sin(a*sqrt(0.5))));
  return vec3(p, z);
}

vec3 doffset(float z) {
  float eps = 0.05;
  return (offset(z + eps) - offset(z - eps))/(2.0*eps);
}

vec3 ddoffset(float z) {
  float eps = 0.05;
  return (doffset(z + eps) - doffset(z - eps))/(2.0*eps);
}

vec3 skyColor(vec3 ro, vec3 rd) {
  const vec3 gcol = HSV2RGB(vec3(0.55, 0.9, 0.035));
  vec2 pp = rd.xy;
  return gcol/dot(pp, pp);
}

vec3 plane(vec3 ro, vec3 rd, vec3 pp, vec3 off, float aa, float n) {
  float l = distance(ro, pp);
  vec3 hn;
  vec2 p = (pp-off*vec3(1.0, 1.0, 0.0)).xy;

  p *= 1.0+l*l;
  
  const float csz = 0.15;
  const float co  = 0.33*csz;

  vec3 col = vec3(0.0);
  
  const float cnt = 5.0;
  const float icnt = 1.0/cnt;
  for (float i = 0.0; i < 1.0; i += icnt) {
    vec2 cp = p;
    vec2 cn = mod2(cp, vec2(csz));
    
    float h0 = hash3(vec3(cn, n)+123.4+i);
    float h1 = fract(3677.0*h0);
    
    cp += soff(h0)*co;
    
    float cl = length(cp);
    float d = (cl-0.0005);    
    d = max(d, 0.0001);
  
    vec3 bcol = 0.000005*(1.0+sin(vec3(0.0, 1.0, 2.0)+ TAU*h1));
    bcol *= smoothstep(0.17*csz, 0.05*csz, cl)*0.15/(d*d*(l*l+0.01));
    
    col += bcol;
    p += icnt;
    p *= ROT(1.0);
  }
  

  return col;
}

vec3 color(vec3 ww, vec3 uu, vec3 vv, vec3 ro, vec2 p) {
  float lp = length(p);
  vec2 np = p + 1.0/RESOLUTION.xy;
  float rdd = 3.0;
  
  vec3 rd = normalize(-p.x*uu + p.y*vv + rdd*ww);
  vec3 nrd = normalize(-np.x*uu + np.y*vv + rdd*ww);

  const int furthest = 9;
  const int fadeFrom = 7;

  const float fadeDist = planeDist*float(furthest - fadeFrom);
  float nz = floor(ro.z / planeDist);

  vec3 col = skyColor(ro, rd);

  // Steps from nearest to furthest plane and accumulates the color 
  for (int i = 1; i <= furthest; ++i) {
    float pz = planeDist*nz + planeDist*float(i);

    float pd = (pz - ro.z)/rd.z;

    if (pd > 0.0) {
      vec3 pp = ro + rd*pd;
      vec3 npp = ro + nrd*pd;

      float aa = 3.0*length(pp - npp);

      vec3 off = offset(pp.z);

      vec3 pcol = plane(ro, rd, pp, off, aa, nz+float(i));

      float nz = pp.z-ro.z;
      float fadeIn = smoothstep(planeDist*float(furthest), planeDist*float(fadeFrom), nz);
      float fadeOut = smoothstep(0.0, planeDist*0.1, nz);
      pcol *= fadeOut*fadeIn;

      col += pcol; 
    }
  }

  return col;
}

vec3 effect(vec2 p, vec2 pp) {
  float tm  = planeDist*TIME*BPM/60.0;
  vec3 ro   = offset(tm);
  vec3 dro  = doffset(tm);
  vec3 ddro = ddoffset(tm);

  vec3 ww = normalize(dro);
  vec3 uu = normalize(cross(normalize(vec3(0.0,1.0,0.0)+ddro), ww));
  vec3 vv = cross(ww, uu);

  vec3 col = color(ww, uu, vv, ro, p);
  col -= .1*vec3(0.0, 1.0, 2.0).zyx*length(pp);
  col *= smoothstep(1.5, 0.5, length(pp));
  col *= smoothstep(1.0, 15.0, TIME);
  col = aces_approx(col);
  col = sRGB(col);

  return col;
}

void main() {
  vec2 q = xlv_TEXCOORD0.xy/RESOLUTION.xy;
  vec2 p = -1. + 2. * q;
  vec2 pp = p;
  p.x *= RESOLUTION.x/RESOLUTION.y;

  vec3 col = effect(p, pp);
  
  gl_FragColor = vec4(col, 1.0);
}

