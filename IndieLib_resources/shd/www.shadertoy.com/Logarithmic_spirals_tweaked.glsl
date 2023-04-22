// CC0: Logarithmic spirals tweaked
//  Been travelling and was messing around with this before travel
//  Looked better than I remembered so publishing it now.

#define TIME        iTime*10.0
#define RESOLUTION  iResolution
#define PI          3.141592654
#define TAU         (2.0*PI)
#define ROT(a)      mat2(cos(a), sin(a), -sin(a), cos(a))

const float ExpBy = log2(1.2);

float modPolar(inout vec2 p, float repetitions) {
  float angle = TAU/repetitions;
  float a = atan(p.y, p.x) + angle/2.;
  float r = length(p);
  float c = floor(a/angle);
  a = mod(a,angle) - angle/2.;
  p = vec2(cos(a), sin(a))*r;
  // For an odd number of repetitions, fix cell index of the cell in -x direction
  // (cell index would be e.g. -5 and 5 in the two halves of the cell):
  if (abs(c) >= (repetitions/2.0)) c = abs(c);
  return c;
}

float forward(float l) {
  return exp2(ExpBy*l);
}

float reverse(float l) {
  return log2(l)/ExpBy;
}

vec3 sphere(vec3 col, mat2 rot, vec3 bcol, vec2 p, float r, float aa) {
  vec3 lightDir = normalize(vec3(1.0, 1.5, 2.0));
  lightDir.xy *= rot;
  float z2 = (r*r-dot(p, p));
  vec3 rd = -normalize(vec3(p, 0.1));
  if (z2 > 0.0) {
    float z = sqrt(z2);
    vec3 cp = vec3(p, z);
    vec3 cn = normalize(cp);
    vec3 cr = reflect(rd, cn);
    float cd= max(dot(lightDir, cn), 0.0);
    vec3 cspe = pow(max(dot(lightDir, cr), 0.0), 10.0)*tanh(8.0*(bcol))*0.5;
    vec3 ccol = mix(0.2, 1.0, cd*cd)*bcol;
    ccol += cspe;
    float d = length(p)-r;
    col = mix(col, ccol, smoothstep(0.0, -aa, d));
  }
  return col;
}

vec2 toSmith(vec2 p)  {
  // z = (p + 1)/(-p + 1)
  // (x,y) = ((1+x)*(1-x)-y*y,2y)/((1-x)*(1-x) + y*y)
  float d = (1.0 - p.x)*(1.0 - p.x) + p.y*p.y;
  float x = (1.0 + p.x)*(1.0 - p.x) - p.y*p.y;
  float y = 2.0*p.y;
  return vec2(x,y)/d;
}

vec2 fromSmith(vec2 p)  {
  // z = (p - 1)/(p + 1)
  // (x,y) = ((x+1)*(x-1)+y*y,2y)/((x+1)*(x+1) + y*y)
  float d = (p.x + 1.0)*(p.x + 1.0) + p.y*p.y;
  float x = (p.x + 1.0)*(p.x - 1.0) + p.y*p.y;
  float y = 2.0*p.y;
  return vec2(x,y)/d;
}


vec2 transform(vec2 p) {
  vec2 sp0 = toSmith(p-0.);
  vec2 sp1 = toSmith(p+vec2(1.0)*ROT(0.12*TIME));
  vec2 sp2 = toSmith(p-vec2(1.0)*ROT(0.23*TIME));
  p = fromSmith(sp0+sp1-sp2);
  return p;
}

// License: Unknown, author: nmz (twitter: @stormoid), found: https://www.shadertoy.com/view/NdfyRM
vec3 sRGB(vec3 t) {
  return mix(1.055*pow(t, vec3(1./2.4)) - 0.055, 12.92*t, step(t, vec3(0.0031308)));
}

// License: Unknown, author: Matt Taylor (https://github.com/64), found: https://64.github.io/tonemapping/
vec3 aces_approx(vec3 v) {
  v = max(v, 0.0);
  v *= 0.6f;
  float a = 2.51f;
  float b = 0.03f;
  float c = 2.43f;
  float d = 0.59f;
  float e = 0.14f;
  return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

vec3 effect(vec2 p, vec2 pp) {
//  float aa = 4.0/RESOLUTION.y;
  vec2 np = p + 1.0/RESOLUTION.y;
  vec2 tp = transform(p);
  vec2 ntp = transform(np);
  float aa = 2.0*distance(tp, ntp);
  p = tp;

  float ltm = 0.75*TIME;
  mat2 rot0 = ROT(-0.125*ltm); 
  p *= rot0;
  float mtm = fract(ltm);
  float ntm = floor(ltm);
  float gd = dot(p, p);
  float zz = forward(mtm);

  vec2 p0 = p;
  p0 /= zz;

  float l0 = length(p0);
  
  float n0 = ceil(reverse(l0));
  float r0 = forward(n0);
  float r1 = forward(n0-1.0);
  float r = (r0+r1)/2.0;
  float w = r0-r1;
  float nn = n0;
  n0 -= ntm;

  vec2 p1 = p0;
  float reps = floor(TAU*r/(w));
  mat2 rot1 = ROT(0.66*n0); 
  p1 *= rot1;
  float m1 = modPolar(p1, reps)/reps;
  p1.x -= r;
  
  vec3 ccol = (1.0+cos(0.85*vec3(0.0, 1.0, 2.0)+TAU*(m1)+0.5*n0))*0.5;
  vec3 gcol = (1.+cos(vec3(0.0, 1.0, 2.0) + 0.125*ltm))*0.01;
  mat2 rot2 = ROT(TAU*m1);

  vec3 col = vec3(0.0);
  float fade = 0.5+0.5*cos(TAU*m1+0.33*ltm);
  //fade *= 0.001;
  col = sphere(col, rot0*rot1*rot2, ccol*mix(0.25, 1.0, sqrt(fade)), p1, mix(0.125, 0.5, fade)*w, aa/zz);
  col *= 1.5;
  col += gcol/max(gd, 0.001);
  col += gcol*aa*10.0;
//  col -= 0.05*vec3(0.0, 1.0, 2.0).zyx*(length(pp)+0.25);
  col = aces_approx(col);
  col = sRGB(col);

  return col;
}

void main( ) {
  vec2 q = xlv_TEXCOORD0.xy/RESOLUTION.xy;
  vec2 p = -1. + 2. * q;
  vec2 pp = p;
  p.x *= RESOLUTION.x/RESOLUTION.y;
  vec3 col = effect(p, p);
  gl_FragColor = vec4(col, 1.0);
}
