#if 0
#extension GL_OES_standard_derivatives : enable

precision highp float;

//uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#endif

// License CC0: Planet Shader
//  I have been experimenting with space themed shaders
#define PI           3.141592654
#define TAU          (2.0*PI)

#define TIME         (time)
#define RESOLUTION   resolution
#define PERIOD       30.0
#define PERIODS      4.0
#define FADETIME     2.0
#define TPERIOD      (mod(TIME, PERIOD))
#define NPERIOD      (int(mod(TIME/PERIOD, PERIODS)))

vec3  sunDirection = normalize(vec3(0.0, 0.1, -10.0));
const vec3  sunColor1    = vec3(1.0, 0.8, 0.8);
const vec3  sunColor2    = vec3(1.0, 0.8, 0.9);
const vec3  planetPos    = vec3(0.0);
const float planetRadii  = 10.0;

vec3 tanh3(vec3 c) {
  return vec3(tan(c.x), tan(c.y), tan(c.z));
}

void rot(inout vec2 p, float a) {
  float c = cos(a);
  float s = sin(a);
  p = vec2(c*p.x + s*p.y, -s*p.x + c*p.y);
}

vec2 raySphere(vec3 ro, vec3 rd, vec3 center, float radius) {
  vec3 m = ro - center.xyz;
  float b = dot(m, rd);
  float c = dot(m, m) - radius*radius;
  if(c > 0.0 && b > 0.0) return vec2(-1.0, -1.0);
  float discr = b * b - c;
  if(discr < 0.0) return vec2(-1.0);
  float normalMultiplier = 1.0;
  float s = sqrt(discr);
  float t0 = -b - s;
  float t1 = -b + s;;
  return vec2(t0, t1);
}

vec3 scaleCol(vec3 c, float s, float r) {
  float o = sqrt(s/r);
  float i = s/o;
  return tanh3(c*i)*o;
}

vec3 skyColor(vec3 rd) {
  float diff = max(dot(rd, sunDirection), 0.0);
  vec3 col = vec3(0.0);
  col += scaleCol(pow(diff, 4.0*200.0)*sunColor1, 8.0, 0.25);
  col += scaleCol(pow(diff, 4.0*50.0)*sunColor2, 1.0, 0.25);
  return col;
}

float psin(float a) {
  return 0.5 + 0.5*sin(a);
}

float pcos(float a) {
  return 0.5 + 0.5*cos(a);
}

float spow(float v, float p) {
  return sign(v)*pow(abs(v), p);
}

vec4 rings(vec2 p) {
  float pl = length(p);
  float pdf = pl-22.0;
  pdf = abs(pdf) - 3.0;
  pdf = abs(pdf) - 2.0;
  pdf = abs(pdf) - 1.0;
  vec4 pcol = vec4(1.0);
  pcol.xyz = mix(vec3(1.0, 0.8, 0.75), vec3(1.0, 1.0, 1.0), 0.5 - 0.5*cos(15.0*pl))*sunColor1;
  pcol.xyz = pow(pcol.xyz, vec3(0.75));
  pcol.w = psin(20.0*pl)*psin(14.0*pl)*psin(21.0*pl);
  pcol.w *= 0.5*step(pdf, 0.0);
  return pcol;
}

vec4 planet(vec3 ro, vec3 rd, vec3 skyCol, inout float pd) {
  vec4 col = vec4(0.0);

  vec2 si = raySphere(ro, rd, planetPos, planetRadii);
  vec3 sp = ro + rd*si.x;
  vec3 sn = normalize(sp - planetPos);
  vec3 sr = reflect(rd, sn);
  vec3 sref = skyColor(sr);
  float sfres = pow(1.0-abs(dot(rd, sn)), 15.0);
  float sdiff = max(dot(sunDirection, sn), 0.0);
  float sf = (si.y - si.x)/(2.0*planetRadii);
  float sl = (1.0-smoothstep(0.2, 0.7, sf));
  vec3 sbeer = exp(-6.0*vec3(0.3, 0.25, 0.15)*sf)*sl;
  float srayl = pow(1.0 - abs(dot(rd, sn)), 5.0);

  float slo = atan(sp.z, sp.x);
  float slat = PI*sp.y/planetRadii;
  vec3 scol = vec3(0.8);

  scol = mix(scol, vec3(0.7, 0.7, 0.8), pow(pcos(20.0*slat), 5.0));
  scol = mix(scol, vec3(0.75, 0.7, 0.8), pow(pcos(14.0*slat), 20.0));
  scol = mix(scol, vec3(0.2), pow(pcos(21.0*slat)*pcos(13.0*slat+1.0), 15.0));  
  scol = tanh3(scol);
  scol *= pow(sunColor1, vec3(0.66));  
  scol += vec3(0.0, 0.0, srayl*0.5) ;

  if (si.x < si.y) {
    col.xyz = sfres*sref + pow(skyCol, vec3(0.85))*sbeer + scol*pow(sdiff, 0.75);
    col.w = 1.0;
    pd = si.x;
  }

  return col;
}

vec3 render(vec3 ro, vec3 rd) {
  vec3 skyCol = skyColor(rd);  
  
  vec4 col = vec4(skyCol, 1.0);
  float cold = 10000.0;

  vec4 pcol = planet(ro, rd, skyCol, cold);

  col.xyz = mix(col.xyz, pcol.xyz, pcol.w);

  float rsd = (0.0 - ro.y)/rd.y;
  vec3 rsp = ro + rd*rsd;
  vec4 rscol = rings(rsp.xz); 
  vec3 rsn = vec3(0.0, 1.0, 0.0);
  
  rscol.w = pow(rscol.w, 1.0*pow(abs(dot(rd, rsn)), 0.4));

  vec2 psi = raySphere(rsp, sunDirection, planetPos, planetRadii);
  rscol.xyz *= (1.0  - smoothstep(0.0, 1.0, (psi.y - psi.x)/(2.0*planetRadii)));
  
  col.xyz = mix(col.xyz, rscol.xyz, rscol.w*step(rsd, cold));
  
  return col.xyz;
}


vec3 postProcess(vec3 col, vec2 q)  {
  col=pow(clamp(col,0.0,1.0),vec3(0.75)); 
  col=col*0.6+0.4*col*col*(3.0-2.0*col);
  col=mix(col, vec3(dot(col, vec3(0.33))), -0.4);
  col*=0.5+0.5*pow(19.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.7);
  return col;
}

vec3 fragment(vec3 ro, vec3 uu, vec3 vv, vec3 ww, vec2 p) {
  vec3 rd = normalize(p.x*uu + p.y*vv + 2.5*ww);
  vec3 col = render(ro, rd);
  return col;
}

void main() {
  vec2 q = xlv_TEXCOORD0.xy/RESOLUTION.xy;
  vec2 p = -1. + 2. * q;
  p.x *= RESOLUTION.x/RESOLUTION.y;

  float tperiod = TPERIOD;
  int period = NPERIOD;
  vec3 ro = vec3(0.0, 0.0, 52.0);
  vec3 la = vec3(0.0, 0.0,0.0);
  vec3 up = vec3(-0.0, 1.0, 0.0);

  if (period == 0) {
    ro = vec3(0.0, 0.0, 52.0);
    ro.y += tperiod*15.0/PERIOD+2.0;
    la = vec3(0.0, 0.0, -1000000.0);
  } else if (period == 1) {
    ro = vec3(00.0, 15.0, -50.0);
    ro.y -= 10.0*tperiod/PERIOD;
    rot(ro.xz,-2.0*tperiod/PERIOD-TAU/10.0);
  } else if (period == 2) {
    ro = vec3(0.0, 5.0, 30.0);
    la = vec3(0.0, 0.0, -1000000.0);
    ro.x += -10.0*tperiod/PERIOD;
    ro.y += 17.0*tperiod/PERIOD;
    ro.z += 90.0*tperiod/PERIOD;
    up = vec3(-1.0, 1.0, 0.0);
  } else if (period == 3) {
    ro = vec3(00.0, 0.0, -25.0);
    rot(ro.yz, 1.0-0.25*tperiod/PERIOD);
    la = vec3(0.0, 0.0, 0.0);
    up = vec3(-0.25, 1.0, 0.0);
  }
  
  
  vec3 ww = normalize(la - ro);
  vec3 uu = normalize(cross(up, ww));
  vec3 vv = (cross(ww,uu));

  float s = 2.0/RESOLUTION.y;

  vec2 o1 = vec2(1.0/8.0, 3.0/8.0)*s;
  vec2 o2 = vec2(-3.0/8.0, 1.0/8.0)*s;

  vec3 col = vec3(0.0);

  // https://blog.demofox.org/2015/04/23/4-rook-antialiasing-rgss/  
  col += fragment(ro, uu, vv, ww, p+o1);
  col += fragment(ro, uu, vv, ww, p-o1);
  col += fragment(ro, uu, vv, ww, p+o2);
  col += fragment(ro, uu, vv, ww, p-o2);
  
  col*=0.25;

  col = postProcess(col, q);
 
  float fadeIn = smoothstep(0.0, FADETIME, tperiod);
  float fadeOut = 1.0 - smoothstep(PERIOD - FADETIME, PERIOD, tperiod);
  col *= fadeIn;
  col *= fadeOut;

  gl_FragColor = vec4(col, 1.0);
}
