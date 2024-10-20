#define R iResolution.xy
#define T (iTime)
#define M_PI 3.14159265358979323846264338327950288
#define TAU (M_PI*2.0)
#define FUI floatBitsToUint
#define ZERO min(0, iFrame)
#define NEAR 0.003
#define FAR 60.0
#define STEPS 70
#define AMBIENT 0.0076


mat2 rot(float a) { float s = sin(a); float c = cos(a); return mat2(c, s, -s, c); }
vec3 aces(vec3 x) { return clamp((x*(2.51*x+0.03))/(x*(2.43*x+0.59)+0.14),0.0,1.0); }
float luma(vec3 color) { return dot(color, vec3(0.299, 0.587, 0.114)); }
float smin(float a, float b, float k) {float h = clamp(0.5 + 0.5*(a-b)/k, 0.0, 1.0); return mix(a, b, h) - k*h*(1.0-h); }
float smax(float a, float b, float k) {return smin(a, b, -k); }
float sabs(in float v, in float k) {return smax(-v, v, k);}
float sclamp(in float v, in float mi, in float ma, in float k) {return smin(smax(v, mi, k), ma, k);}
float sgt(in float a, in float b, in float s) {float h = clamp(0.5 + 0.5 * (a - b) / s, 0.0, 1.0); return mix(0.0, 1.0, h*h*(3.0-2.0*h));}
float slt(in float a, in float b, in float s) {float h = clamp(0.5 + 0.5 * (b - a) / s, 0.0, 1.0); return mix(0.0, 1.0, h*h*(3.0-2.0*h));}
float ssign(float x, float k) { float s = smoothstep(-k, k, x); return mix(s, sign(x), 0.5*clamp(max(0.0, 1.0-(abs(s)*2.5))*max(0.0, 1.0-k), 0.0, 1.0));}

vec3 pointRot(vec3 p, vec3 r) { p.yz *= rot(r.z); p.xz *= rot(r.y); p.xy *= rot(r.x); return p; }

vec3 thash(vec2 p, float seed) {
    return textureLod(iChannel0, ((p  + seed) / 256.0), 0.0).xyz;
}

vec3 tnoise(vec2 p, float s) {
  p += 3.3828714;
  vec2 id = floor(p); vec2 lv = fract(p);lv = lv*lv*(3.0-2.0*lv);
  return mix(mix(thash(id+vec2(0,0),s),thash(id+vec2(1,0),s),lv.x),
             mix(thash(id+vec2(0,1),s),thash(id+vec2(1,1),s),lv.x),lv.y);
}

vec3 tnoise(vec2 p, float s, float freq, float warp, const in int oct) {
  vec3 n = vec3(0.0);
  float div = 0.0;
  float amp = 1.0;
  vec2 tx = vec2(0.0);
  for (int i = ZERO; i < oct; i++) {
    n += amp*tnoise((p+tx)*freq,s);
    div += amp;
    amp *= 0.5;
    freq *= 2.0;
    tx += warp * vec2(sin(n.x*6.28), cos(n.y*6.28));

  }
  return n / div;
}

float hash21(vec2 ip, float seed) {
  uvec2 p = FUI(ip); uint s = FUI(seed); uvec2 k = ~p + ~s;
  p ^= p << 17U; p ^= p >> 13U; p ^= p << 5U;
  p += (p ^ k) + p * k; p *= 1013U; p ^= (p >> 4U);
  return float(p.x * k.y + p.y + p.y * k.x + k.y) / float(0xFFFFFFFFU);
}

float noise(vec2 p, float s) {
  p += 3.3828714;
  vec2 id = floor(p); vec2 lv = fract(p);lv = lv*lv*(3.0-2.0*lv);
  return mix(mix(hash21(id+vec2(0,0),s),hash21(id+vec2(1,0),s),lv.x),
             mix(hash21(id+vec2(0,1),s),hash21(id+vec2(1,1),s),lv.x),lv.y);
}

float noise(vec2 p, float s, float freq, float dec, float warp, const in int oct) {
  float n = 0.0;
  float div = 0.0;
  float amp = 1.0;
  vec2 tx = vec2(0.0);
  for (int i = ZERO; i < oct; i++) {
    n += amp*noise((p+tx)*freq,s);
    div += amp;
    amp *= dec;
    freq *= exp(dec*1.386294401);
    tx += warp * vec2(sin(n*6.28), cos(n*6.28));
    
  }
  return n / div;
}

struct Light {
  vec3 p;
  vec3 dir;
  vec3 color;
  float strength;
  int type;
};

#define LIGHT_AMBIENT 0
#define LIGHT_POINT 1
#define LIGHT_DIRECTIONAL 2

vec3 getLightDir(Light light, vec3 p) {
  switch (light.type) {
    case LIGHT_AMBIENT: return normalize(light.p); break;
    case LIGHT_POINT: return normalize(light.p - p); break;
    case LIGHT_DIRECTIONAL: return normalize(-light.dir); break;
    default: return normalize(vec3(1, 2, 3)); break;
  }
}

vec3 getLightAtt(Light light, vec3 p, vec3 N) {
  vec3 L = getLightDir(light, p);
  float NdotL = max(AMBIENT, dot(N, L));
  switch (light.type) {
    case LIGHT_AMBIENT: return NdotL * light.strength * light.color; break;
    case LIGHT_POINT: return pow(NdotL * light.strength * light.color, vec3(2.0)) / max(0.0000353, pow(distance(light.p, p), 2.0));
    case LIGHT_DIRECTIONAL: {
      return light.color * light.strength * NdotL * smoothstep(0.8, 1.0, dot(
        normalize(light.p - p),
        L
      )); break;
    }
  }

  return vec3(0.5);
}

struct Material {
  float rough;
  float spec;
  float z;
};

#define NEW_MATERIAL Material(0.0, 0.0, 0.0)

struct Object {
  int id;
  vec3 p;
  vec3 q;
};

#define NEW_OBJECT Object(-1, vec3(0.0), vec3(0.0))

struct Data {
  vec3 p;
  vec3 n;
  float d;
  Object o;
  Material m;
};

#define NEW_DATA Data(vec3(0.0), vec3(0.0), FAR, NEW_OBJECT, NEW_MATERIAL)


float boxSDF(vec3 p, vec3 s) {
  return length(max(abs(p)-s, 0.0));
}

float lineSDF(vec3 p, vec3 a, vec3 b, float r) {
  vec3 pa = p - a;
  vec3 ba = b - a;
  float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
  return length(pa - h * ba)-r;
}

float sphereSDF(vec3 p, float r) {
    return length(p) - r;
}


#define SAMPLE(OBJ, DIST) if (DIST < dist) { dist = DIST; data.o = OBJ; }

#define ID_BOX 0
#define ID_GROUND 1
#define ID_PEDICEL 2
#define ID_FLOWER 3
float getGroundHeight(vec3 p) {
  vec3 n1 = tnoise(p.xz, 0.039123, 0.05, 0.5, 2);
  
  return n1.x * 10.;
}

float groundSDF(vec3 p, inout float h) {
  h = getGroundHeight(p);
  
  
  
  return p.y - h;
}
vec3 repeatX(in vec3 p, in float size) {
    return vec3(p.x - size*round(p.x/size), p.yz);
}
vec3 repeatY(in vec3 p, in float size) {
    return vec3(p.x, p.y - size*round(p.y/size), p.z);
}
vec3 repeatZ(in vec3 p, in float size) {
    return vec3(p.xy, p.z- size*round(p.z/size));
}

float sdf(vec3 p, vec3 rd, inout Data data) {
  float dist = FAR;
  
  float h = 0.0;
  Object oGround = Object(ID_GROUND, vec3(0.0), vec3(0.0));
  float ground = groundSDF(p - oGround.p, h);
  
  vec3 bp = p;
  float freq = 2.0;
  vec2 lid = floor((p.xz/freq)+0.15);//-(freq*0.5);
  p.x = (mod(p.x, freq)-(freq*0.5));
  p.z = (mod(p.z, freq)-(freq*0.5));

  
  float r1 = hash21(lid*16.9238123, 3.32124);
  float r2 = fract(r1*10.4984923);
  float r4 = fract(r1*30.2918121+r2*5.602919);
  float r5 = fract((r1+r2+r4)*10.0583823);
  
  float lineH = mix(0.05, 0.3, r1);
  float lineR = mix(0.01, 0.03, r2)*0.5;
  float flowerR = mix(0.02, 0.04, r4)*0.5;
  
  
  if (ceil(r1-0.4) > 0.9) {
      Object oPedicel = Object(ID_PEDICEL, vec3(0.0), vec3(0.0));
      float pedicel = lineSDF(p, vec3(-r5, 0.0, r5), vec3(0, h+lineH, 0), lineR)*0.6;
      SAMPLE(oPedicel, pedicel);
      
      Object oFlower = Object(ID_FLOWER, vec3(0, h+lineH, 0), vec3(0.0));
      vec3 fdir1 = normalize(vec3(r1, 1.0, sin(p.x)));
      vec3 fdir2 = normalize(vec3(cos(p.z), 1.0, r2));
      float flower1 = lineSDF(vec3(abs(p.x), p.y, p.z), oFlower.p, vec3(0, h+0.5*lineH, 0) + fdir1*lineH, flowerR);
      float flower2 = lineSDF(vec3(p.x, p.y, abs(p.z)), oFlower.p, vec3(0, h+0.5*lineH, 0) + fdir2*lineH, flowerR);
    
      float flower = min(flower1, flower2);
      
      flower *= 0.9;
      
      SAMPLE(oFlower, flower);
      
  }
  SAMPLE(oGround, ground);

  return dist;
}


bool march(vec3 ro, vec3 rd, inout Data data) {
  float d = 0.0;

  for (int i = ZERO; i < STEPS; i++) {
    vec3 p = ro+rd*d;
    float next = sdf(p, rd, data);
    if (next >= FAR) return false;
    if (next <= (NEAR*(1.0+0.5*abs(d)))) break;
    d += next;
  }

  vec3 p = ro+rd*d;
  vec2 e = vec2(NEAR, 0.0);
  vec3 n = normalize(sdf(p, rd, data) - vec3(
    sdf(p - e.xyy, rd, data),
    sdf(p - e.yxy, rd, data),
    sdf(p - e.yyx, rd, data)
  ));

  data.p = p;
  data.n = n;
  data.d = d;

  return true;
}

float getShadow(vec3 ro, vec3 rd, vec3 n) {
  Data data = NEW_DATA;
  float NdotL = dot(n, rd);
  if (NdotL <= 0.00003) return AMBIENT;
  const int steps = 30;
  float d = 0.0;
  float t = NEAR * (1.0 + 2.0*max(0.0, NdotL));
  float s = 0.0;
  float k = 0.0;

  for (int i = ZERO; i < steps; i++) {
    vec3 p = ro+rd*t;//0.999*((t + 00.25*NdotL)*rd);
    float next = sdf(p, rd, data);
    //if (next >= FAR) return 1.0;
    if (next < -0.05) break;
    d += next;
    t += clamp(next, 0.005, 0.5);
    k += 1.0;
  }
  
  d /= max(1.0, k);

  s = smoothstep(0.5, 1.0, clamp(1.0-(d / (1.0+d)), 0.0, 1.0));
  s = 1.0-s;
  s = exp(s-1.0);
  return clamp(s, AMBIENT, 1.0);
}

vec3 getAlbedoBox(inout Data data) {
  vec3 p = data.p;
  vec3 n = data.n;
  vec2 uv = mix(mix(p.xy, p.yz, round(abs(dot(n, vec3(1, 0, 0))))),
                p.xz,round(abs(dot(n, vec3(0, 1, 0)))));

  vec2 id = floor(uv*2.0);
  return vec3(1.0)*mod(id.x-id.y, 2.0);
}

vec3 getAlbedoFlower(inout Data data) {
    vec3 col = vec3(0.0);
    vec3 n1 = tnoise(data.p.xz*24., 3.209123);
    col += vec3(n1.x, n1.y*0.89, n1.z*0.1);
    return col;
}

vec3 getAlbedoPedicel(inout Data data) {
    vec3 col = vec3(0.0);
     vec3 n1 = tnoise(data.p.xz*24., 10.209123);

    col += vec3(n1.x*0.36, max(0.5, n1.y), n1.z*0.15);
    col = col*col;
    return col;
}

//#define DEBUG_TEXTURE getAlbedoGround

float line(vec2 p, vec2 a, vec2 b, float t) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return 1.0-clamp(length((pa - ba * h)/t), 0.0, 1.0);
}

vec3 getAlbedoGround(inout Data data) {
    vec3 col = vec3(0.0);
    vec3 p = data.p;
    vec2 uv = p.xz*1.;
    
    vec3 n1 = tnoise(uv, 2.44312, 9.2, 0.01, 4);
    vec3 n2 = tnoise(uv, 10.00444, 0.9, 0.005, 2);
    vec3 n3 = clamp(abs(cross(n1*2.0-1.0, n2*2.0-1.0)), 0.0, 1.0);
    vec3 n4 = tnoise(uv*0.1, 16.693825+2.0*n1.x);
    
    vec3 g1 = vec3(n1.x*0.6, max(0.39, n1.z), n1.y*0.15);
    vec3 g2 = vec3(n1.y*0.6, max(0.39, n1.x), n1.x*0.15);
    vec3 g3 = vec3(n1.z*0.6, max(0.39, n1.y), n1.y*0.15);
    
   
    
    float pat = 0.0;
    float seed = 33.3212;
    
    const int iterations = 4;
    const float invIter = 1.0 / float(iterations);
    
    
    for (int i = ZERO; i < iterations; i++)
    {
        float k = float(i)*invIter*6.28;
        uv *= rot(k);
        vec2 id = floor(uv);
        vec2 lv = fract(uv);
        vec2 cv = lv*2.0-1.0;
        vec2 slv = abs(cv);
//        float grid = 
  //          max(smoothstep(0.05, 0.0, abs(cv.x+1.)),
    //            smoothstep(0.05, 0.0, abs(cv.y+1.)));

        vec2 angid = id+floor(cv);
        
        float ang = mix(
            mix(hash21(angid+vec2(0, 0), seed), hash21(angid+vec2(1, 0), seed), slv.x),
            mix(hash21(angid+vec2(0, 1), seed), hash21(angid+vec2(1, 1), seed), slv.x),
            slv.y
        );
        pat += line(vec2(abs(cv.x), abs(cv.y)), vec2(0.0), 2.0*vec2(sin(ang), cos(ang)), mix(0.05, 3., n3.y*n3.y));
        uv *= 1.5;
        seed += 0.15489338;
    }
    col = mix(g1,  g1, n1.x);
    col = mix(col, g2, n1.y);
    col = mix(col, g3, n1.z);
    
    col = mix(col, col*col, smoothstep(0.0, 0.05+n3.y+0.25*(n2.y*n2.y), pat*n3.x));
    col = mix(col, col*col, smoothstep(0.4, 0.7, n4.x));
    
    return col;
}

vec3 getAlbedo(inout Data data) {
  switch (data.o.id) {
    case ID_BOX: return getAlbedoBox(data); break;
    case ID_GROUND: return getAlbedoGround(data); break;
    case ID_FLOWER: return getAlbedoFlower(data); break;
    case ID_PEDICEL: return getAlbedoPedicel(data); break;
  }

  return vec3(0.93);
}

vec3 forEachLight(Light light, vec3 ro, vec3 rd, Data data, vec3 diffuse) {
  vec3 p = data.p;
  vec3 N = data.n;
  vec3 L = getLightDir(light, p);
  vec3 att = getLightAtt(light, p, N);
  vec3 ref = reflect(N, L);
  float VdotR = max(0.0, dot(rd, ref));
  float spec = pow(VdotR, 24.0) * data.m.spec;
  float shadow = data.d > (FAR*0.5) ? 1.0 : getShadow(data.p, getLightDir(light, data.p), data.n);
  return max(((diffuse + spec) * att) * shadow, vec3(0.0));
}

vec3 getSky(in vec3 ro, in vec3 rd) {
    vec3 col = vec3(0.0);
    float dotup = max(0.0, dot(rd, vec3(0, 1, 0)));
    vec3 blue = vec3(0.04, 0.4, 0.8);
    
    col += pow(blue, vec3(1.0 + 9.*dotup));
    
    if (rd.y <= 0.0) return col;
    
    float t = T*0.25;
    
    vec2 uv = rd.xz / rd.y;
    
    vec3 n1 = tnoise(uv-vec2(sin(t), cos(t)), 4.49218, 2.2, 0.03, 4);
    vec3 n2 = tnoise(uv+vec2(sin(t+n1.y), cos(t+n1.z)), 10.593, 1.6, 0.03, 3);
    col += ((n1.x * smoothstep(0.4, 0.7, n1.z)) + (n2.x * smoothstep(0.4, 0.7, n2.z)))*0.5;
    
    return col;
}

vec3 scene(vec3 ro, vec3 rd) {
  #define NUM_LIGHTS 1

  Light lights[NUM_LIGHTS] = Light[](
    Light(vec3(1, 2, -3), vec3(0.0, 1, 1), vec3(0.97, 0.79, 0.69), 4.0, LIGHT_AMBIENT)
  );
  
  vec3 col = vec3(0.0);
  Data data = NEW_DATA;

  if (march(ro, rd, data)) {
    vec3 albedo = getAlbedo(data);
    vec3 diffuse = albedo / M_PI;
    for (int i = ZERO; i < NUM_LIGHTS; i++) {
      Light light = lights[i];
      col += forEachLight(light, ro, rd, data, diffuse);
    }
  } else {
      col += getSky(ro, rd);
  }
  
  
  vec3 L = getLightDir(lights[0], ro);
  float VdotL = max(0.0, dot(rd, L));
  
  col += lights[0].color * lights[0].strength * pow(VdotL, 16.0);
  
  return max(col, vec3(0.0));
}

void getRay(vec2 uv, vec4 m, inout vec3 ro, inout vec3 rd) {
  float Y = 3.5;
  ro = vec3(0.0, 0.0, -6.0);
  rd = normalize(vec3(uv, 1.0));

  if (m.z > 0.01) {
    ro.yz *= rot(m.y*TAU);
    ro.xz *= rot(m.x*TAU);
    rd.yz *= rot(m.y*TAU);
    rd.xz *= rot(m.x*TAU);
  }
  
  ro.z += T;
  
  Y += getGroundHeight(ro);
  ro.y += Y;
  
  ro.y = max(ro.y, Y);
}

////////////////////////////////////////////////////////

void mainImage(out vec4 o, in vec2 fc) {
  vec3 col = vec3(0.0);
  vec2 uv = (fc-0.5*R.xy)/R.y;
  vec4 m = vec4((iMouse.xy-0.5*R.xy)/R.y,iMouse.zw);

  vec3 ro = vec3(0.0);
  vec3 rd = vec3(0.0);
  getRay(uv, m, ro, rd);

  #ifdef DEBUG_TEXTURE
  Data data = NEW_DATA;
  data.p = (fc.xy / R.y).xyy;
  col += DEBUG_TEXTURE(data);
  #else
  col += scene(ro, rd);
  #endif
  col = aces(col);
  col = pow(col, vec3(1.0 / 2.2));
  
  o = vec4(col, 1.0);
}
