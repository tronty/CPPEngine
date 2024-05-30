// CC0 - Rendezvous with gasgiant
//  After an annoying week at work I needed to tinker a bit.

#define TIME        iTime
#define RESOLUTION  iResolution
#define PI          3.141592654
#define TAU         (2.0*PI)
#define ROT(a)      mat2(cos(a), sin(a), -sin(a), cos(a))

// License: WTFPL, author: sam hocevar, found: https://stackoverflow.com/a/17897228/418488
const vec4 hsv2rgb_K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
vec3 hsv2rgb(vec3 c) {
  vec3 p = abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www);
  return c.z * mix(hsv2rgb_K.xxx, clamp(p - hsv2rgb_K.xxx, 0.0, 1.0), c.y);
}
// License: WTFPL, author: sam hocevar, found: https://stackoverflow.com/a/17897228/418488
//  Macro version of above to enable compile-time constants
#define HSV2RGB(c)  (c.z * mix(hsv2rgb_K.xxx, clamp(abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www) - hsv2rgb_K.xxx, 0.0, 1.0), c.y))
#define ROT(a)      mat2(cos(a), sin(a), -sin(a), cos(a))


const vec3 luma         = sqrt(vec3(0.299, 0.587, 0.114));
const vec4 planetDim    = vec4(vec3(0.0,0.,0.), 1.);
const vec3 sunDir       = normalize(vec3(-0.4, 0.5, -1.));
const vec3 sunCol       = HSV2RGB(vec3(0.58, 0.6, 1.));
const vec3 starCol0     = HSV2RGB(vec3(0.05, 0.5, 0.5));
const vec3 starCol1     = HSV2RGB(vec3(0.55, 0.3, 0.75));
const vec3 coreCol      = HSV2RGB(vec3(0.05, 0.75, 8.));
const vec3 raylCol      = HSV2RGB(vec3(0.58, 0.85, 0.75));
const vec3 bottomBoxCol = HSV2RGB(vec3(0.5, 0.75, 0.5));
const vec3 topBoxCol    = HSV2RGB(vec3(0.57, 0.80, 1.));
const vec3 planetCol0   = HSV2RGB(vec3(0.04, 0.5, 1.));
const vec3 shipPos      = vec3(0.,0.075, 2.);
const vec3 shipDim      = vec3(0.9, 0.1, 0.3)*0.3;
const vec3 shipGlow     = HSV2RGB(vec3(0.05, 0.8, 1E-6));
const mat2 shipRot      = ROT(0.7);
const float halfPeriod  = 74.;

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
vec2 hash2(vec2 p) {
  p = vec2(dot (p, vec2 (127.1, 311.7)), dot (p, vec2 (269.5, 183.3)));
  return fract(sin(p)*43758.5453123);
}

// License: Unknown, author: Unknown, found: don't remember
float hash(vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,58.233))) * 13758.5453);
}

// License: MIT, author: Inigo Quilez, found: https://iquilezles.org/articles/distfunctions/
float box(vec2 p, vec2 b) {
  vec2 d = abs(p)-b;
  return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}
  
// License: MIT, author: Inigo Quilez, found: https://www.shadertoy.com/view/XslGRr
float vnoise(vec2 p) {
   vec2 i = floor( p );
   vec2 f = fract( p );

   vec2 u = f*f*(3.0-2.0*f);

   float a = hash( i + vec2(0.0,0.0) );
   float b = hash( i + vec2(1.0,0.0) );
   float c = hash( i + vec2(0.0,1.0) );
   float d = hash( i + vec2(1.0,1.0) );

   float m0 = mix(a, b, u.x);
   float m1 = mix(c, d, u.x);
   float m2 = mix(m0, m1, u.y);

   return m2;
}

// License: Unknown, author: Claude Brezinski, found: https://mathr.co.uk/blog/2017-09-06_approximating_hyperbolic_tangent.html
float tanh_approx(float x) {
  //  Found this somewhere on the interwebs
  //  return tanh(x);
  float x2 = x*x;
  return clamp(x*(27.0 + x2)/(27.0+9.0*x2), -1.0, 1.0);
}

// License: MIT, author: Inigo Quilez, found: https://iquilezles.org/articles/intersectors/
vec2 raySphere(vec3 ro, vec3 rd, vec4 sd) {
  vec3 oc = ro - sd.xyz;
  float b = dot( oc, rd );
  float c = dot( oc, oc ) - sd.w*sd.w;
  float h = b*b - c;
  // no intersection
  if (h<0.0) return vec2(-1.0);
  h = sqrt( h );
  return -b+vec2(-h, +h);
}

// License: MIT, author: Inigo Quilez, found: https://iquilezles.org/articles/intersectors/
vec2 rayEllipse(vec3 ro, vec3 rd, vec3 ra) {
    vec3 ocn = ro/ra;
    vec3 rdn = rd/ra;
    float a = dot( rdn, rdn );
    float b = dot( ocn, rdn );
    float c = dot( ocn, ocn );
    float h = b*b - a*(c-1.0);
    if( h<0.0 ) return vec2(-1.0); //no intersection
    h = sqrt(h);
    return vec2(-b-h,-b+h)/a;
}

// License: MIT, author: Inigo Quilez, found: https://iquilezles.org/articles/intersectors/
vec3 norEllipse(vec3 pos, vec3 ra) {
  return normalize(pos/(ra*ra) );
}


vec3 toSpherical(vec3 p) {
  float r   = length(p);
  float t   = acos(p.z/r);
  float ph  = atan(p.y, p.x);
  return vec3(r, t, ph);
}

vec2 rayShip(vec3 ro, vec3 rd) {
  ro.xz *= shipRot;
  ro -= shipPos;
  rd.xz *= shipRot;
  return rayEllipse(ro, rd, shipDim);
}

vec3 norShip(vec3 pos) {
  pos.xz *= shipRot;
  pos -= shipPos;
  return norEllipse(pos, shipDim);
}

float height(vec2 p, float magic) {
  float h = 0.;
  float ip = 7.*length(p)+1.5;
  float ia = 0.25;
  for (float i = 0.; i < 7.; ++i) {
    h += ia*sin(ip);
    ia *= 0.5*magic;
    ip = 1.99*ip+1.;
  }
  return h;
}

vec3 ringCol(vec2 p, float magic) {
  vec3 h = vec3(0.);
  float ip = 7.*length(p)+2.;
  float ia = 0.5;
  for (float i = 0.; i < 7.; ++i) {
    h += ia*sin(0.15*vec3(2,1,0)+ip);
    ia *= 0.5*magic;
    ip = 1.99*ip+1.;
  }
  h = abs(h);
  return h;
}

vec3 planetCol(vec3 pp) {
  float h = 0.;
  float p = pp.y;
  float ip = 4.*(p)-2.;
  float ia = 0.6;
  for (float i = 0.; i < 7.; ++i) {
    h += ia*(1.+sin(ip));
    ia *= 0.5;
    ip = 1.99*ip+1.;
  }
  h *= h*0.25;
  return h*planetCol0;
}

float hf(vec2 p, float magic) {
  return 1E-2*height(p, 0.9*magic);
}

vec3 nf(vec2 p, float aa, float magic) {
  vec2 e = vec2(aa, 0);
  
  vec3 n;
  n.x = hf(p + e.xy, magic) - hf(p - e.xy, magic);
  n.y = 2.0*e.x;
  n.z = hf(p + e.yx, magic) - hf(p - e.yx, magic);
  
  return normalize(n);
}

vec3 skyCol0(vec3 ro, vec3 rd) {
  float gf0 = 1.001 -dot(rd, sunDir);
  vec3 col = sunCol*1E-5/(gf0*gf0);
  return col; 
}

vec3 stars0(vec2 cp, float sd) {
  vec3 scol0 = vec3(0.);
  const float ii = 3.;
  for (float i = 0.; i < ii; ++i) {
    float fi = (i+1.)/ii;
    vec2 sp2 = cp;
    float sc = PI/(100.-20.*fi*fi); 
    sp2 /= sc;
    vec2 np2 = round(sp2);
    vec2 cp2 = sp2-np2;
    vec2 sh2 = -1.+2.*hash2(np2+123.4+i);
    float sh1= fract((sh2.x+sh2.y)*8667.);
    cp2 += sh2*0.5;
    cp2.y *=sd;
    float dd = length(cp2)*sc;
    
    if (sh1+0. < sd) {
      scol0 += sh1*fi*smoothstep(sd*15E-4, 0., dd)*sqrt(mix(starCol0, starCol1, sh1*sh1));
    }
  }
  return scol0;
}

vec3 stars1(vec2 cp, float sd) {
  vec3 scol1 = vec3(0.);
  const float ii =3.;
  for (float i = 0.; i < ii; ++i) {
    float fi = (i+1.)/ii;
    vec2 sp2 = cp;
    float sc = PI/(20.-10.*fi*fi); 
    sp2 /= sc;
    vec2 np2 = round(sp2);
    vec2 cp2 = sp2-np2;
    vec2 sh2 = -1.+2.*hash2(np2+123.4+i);
    float sh1= fract((sh2.x+sh2.y)*8667.);
    cp2 += sh2*0.33;
    cp2.y *=sd;
    float dd = length(cp2)*sc;
    float sh3 = fract(3667.0*sh1);
    
    if (sh3+0.5 < sd) {
      vec3 scol = mix(starCol0, starCol1, sh1*sh1);
      scol1 += sh3*fi*2E-10*scol*scol/max(dd*dd*dd*dd, 1E-10);
      scol1 += 2.*fi*smoothstep(sd*15E-4, 0., mix(0.75, 0.5, fi)*dd)*scol;
    }
  }
  return scol1;
}

float dust(vec2 cp, float sd, float di) {
  const float jj = 9.;
  vec2 jp2 = cp;
  jp2.y *= sd;
  jp2 *= 1.-0.2;
  jp2 *= ROT(0.6);
  jp2 -= vec2(-2., -0.8);
  float jh = 0.;
  float ja = 1.; 
  float jagg = 0.;
  for (float j = 0.; j < jj; ++j) {
    jh += ja*vnoise(jp2);
//    jh += ja*sin(jp2.x)*sin(jp2.y);
    jagg += ja;
    ja *= -0.55;
    jp2 *= 1.99*ROT(1.);
  }
  jh /= jagg;
  jh -= 0.4;
  jh = max(jh, 0.);
  jh *= sd;
  jh *= di;
  return exp(-40.*(jh));
}

vec2 core(vec2 cp, float sd) {
  vec2 gp = cp;
  gp.y *= sd;
  gp.y -= 0.6;
  gp *= ROT(PI*0.33);
  gp = abs(gp);
  float glum = smoothstep(1.0,0.25, gp.x)*tanh_approx(1E-3/((pow(10.*gp.x,4.)+1.)*pow(gp.y, 2.)));
  
  return vec2(glum, tanh_approx(5E-2/gp.y));
}

vec3 skyCol1(vec3 ro, vec3 rd) {
  vec3 col = skyCol0(ro, rd);

  vec3 sp = toSpherical(rd.xzy);
  float sd = sin(sp.y);
  
  vec2 cp = sp.yz-.5*PI*vec2(1.,-1.);
  
  vec3 scol0 = stars0(cp, sd);
  vec3 scol1 = stars1(cp, sd);

  vec2 core = core(cp, sd);
  float dust = dust(cp, sd, core.y);
  
  col += coreCol*core.x*dust;
  float lum = dot(luma, col);
  float sfo = 1./(1E3*lum+1.);
  col += scol0*dust*sfo;
  col += scol1*sfo;

  return col; 
}

vec3 skyCol2(vec3 ro, vec3 rd) {
  vec3 col = skyCol0(ro, rd);
  
  float tp0  = -(ro.y+5.)/rd.y;
  float tp1 = -(ro.y-6.)/rd.y;

  if (tp0 > 0.0) {
    col += bottomBoxCol*exp(-0.75*(length((ro + tp0*rd).xz)));
  }

  if (tp1 > 0.0) {
    vec3 pos  = ro + tp1*rd;
    vec2 pp = pos.xz;
    float db = box(pp, vec2(9.0, 2.0))-3.;
    
    col += topBoxCol*rd.y*rd.y*smoothstep(0.25, 0.0, db);
    col += 0.2*topBoxCol*exp(-0.5*max(db, 0.0));
    col += 0.05*sqrt(topBoxCol)*max(-db, 0.0);
  }

  return clamp(col, 0., 10.); 
}


float ringShadow(vec3 ro, vec3 rd) {
  float ri = -(ro.y-planetDim.y)/rd.y; 
  vec3 rp = ro+rd*ri; 
  vec3 pp  = rp-planetDim.xyz;

  vec2 rp2 = pp.xz;
  float rl = length(rp2);
  float sp = 4.5*rl+1.;
  float shade = 0.6+0.3*sin(sp)*sin(2.9*sp);
  return ri > 0. ? 1.-shade*smoothstep(0.4, 0.2, abs(rl-2.)) : 1.;
}

vec4 rings(vec3 ro, vec3 rd, vec3 nrd, out float hit) {
  float ri = -(ro.y-planetDim.y)/rd.y; 
  float nri = -(ro.y-planetDim.y)/nrd.y; 
  vec3 rp = ro+rd*ri; 
  vec3 nrp = ro+nrd*nri; 
  vec2 si = raySphere(rp, sunDir, planetDim);
  vec2 ssi = rayShip(rp, sunDir);
  vec3 pp  = rp-planetDim.xyz;
  vec2 rp2 = pp.xz;
  vec2 nrp2 = nrp.xz-planetDim.xz;
  float aa = length(nrp2-rp2);
  float rl = length(rp2);
  float rfre = 1.+rd.y;
  float magic = smoothstep(1., .6, rfre*sqrt(abs(dot(normalize(pp), normalize(rd)))));
  magic = mix(1., magic, tanh_approx(.1*ri));
  vec3 ringCol = ringCol(rp2, magic);
  vec3 n = nf(rp2, aa, magic);
  vec3 r = reflect(rd, n);
  float dif = max(dot(sunDir, n), 0.0);
  vec3 ref = skyCol0(rp, r);
  vec3 rcol = vec3(0.);
  rcol += ringCol;
  rcol += sqrt(ref)*rfre;
  rcol *= smoothstep(0.4, 0.2, abs(rl-2.));
  float rt = dot(luma, rcol);
  if (si.x > 0.)
    rcol *= 1.-0.95*tanh_approx(4.*pow(si.y-si.x, 2.));
  if (ssi.x > 0.)
    rcol *= 1.-0.9*tanh_approx(2E4*pow(ssi.y-ssi.x, 4.));
  hit = ri > 0. ? ri : -1.;
  return vec4(rcol, rt*step(0., ri));
}

vec4 planet(vec3 ro, vec3 rd, vec3 nrd, out float hit) {
  vec2 pi = raySphere(ro, rd, planetDim);
  vec3 rp = ro+rd*pi.x;
  vec3 pp = rp-planetDim.xyz;
  vec3 pn = normalize(pp);
  vec3 pr = reflect(rd, pn);
  float pfre = 1.+dot(rd, pn);
  pfre *= pfre;
  pfre *= pfre;
  float pt = tanh_approx(1E3*pow(pi.y-pi.x, 4.)/pi.x);
  vec3 pcol = vec3(0.);
    
  float diff = max(dot(pn, sunDir), 0.);
  pcol += sqrt(diff)*sqrt(sunCol)*planetCol(pp);
  pcol += skyCol0(rp, pr)*pfre;
  pcol += raylCol*diff*pfre;
  hit = pi.x;
  float ringShadow = ringShadow(rp, sunDir);
  pcol.xyz *= ringShadow;
  return vec4(pcol, pt*step(0., pi.x));
}

vec4 ship(vec3 ro, vec3 rd, vec3 nrd, out float hit) {

  vec2 si = rayShip(ro, rd);
  vec3 sp = ro+rd*si.x;
  vec3 sn = norShip(sp);
  vec3 sr = reflect(rd, sn);
  vec3 srr= refract(rd, sn, 0.8);
  float gd = abs(sp.y-shipPos.y);
  float fre= 1.0+dot(sn,rd);

  vec3 scol = skyCol2(sp, sr)*mix(2./3., 1., fre*fre)/(1.+1E-2*si.x*si.x);
  scol += shipGlow/max(gd*gd, 1E-7+1E-7*si.x*si.x);
  float st = tanh_approx(1E3*pow(si.y-si.x, 2.)/si.x);
  hit = si.x;
  return vec4(scol, st*step(0., si.x));
}

vec3 pos(float tm) {
  float t = -(tm-halfPeriod)/halfPeriod;
  return vec3(8.*t, 3.*t*t*t, -14.0*t*t)+vec3(0.0, 0.5, 3.);
}

vec3 effect(vec2 p) {
  const float fov = 2.;
  const vec3 up = normalize(vec3(0.0, 1., 0.5));
  
  float tm  = mod(TIME, 2.*halfPeriod); 
  vec3 ro   = pos(tm);
  vec3 dro  = normalize(ro-pos(tm-1E-1));
  vec2 np   = p+sqrt(8.)/RESOLUTION.y;

  vec3 ww0 = dro;
  vec3 ww1 = normalize(-ro);
  vec3 ww = normalize(1.*ww0+3.*ww1);
  vec3 uu = normalize(cross(up, ww));
  vec3 vv = cross(ww, uu);
  
  vec3 rd = normalize(-p.x*uu + p.y*vv + fov*ww);
  vec3 nrd = normalize(-np.x*uu + np.y*vv + fov*ww);

  float phit, rhit, shit;
  
  vec4 pcol = planet(ro, rd, nrd, phit);
  vec4 rcol = rings(ro, rd, nrd, rhit);
  vec4 scol = ship(ro, rd, nrd, shit);

  float pt = pcol.w;
  float rt = rcol.w*(rhit < phit?1.:1.-pt);
  float st = scol.w*(shit < phit?1.:1.-pt);
  st *= shit < rhit?1.:1.-rt;

  vec3 col = skyCol1(ro, rd);

  col = mix(col, pcol.xyz, pt);
  col = mix(col, rcol.xyz, rt);  
  col = mix(col, scol.xyz, st);

  col = aces_approx(col);  
  col *= smoothstep(halfPeriod, halfPeriod-2., abs(tm-halfPeriod));
  col = sqrt(col);

  return col;
}


void main( )
{
  vec2 fragCoord=xlv_TEXCOORD0.xy;
  vec2 p= (-RESOLUTION.xy+2.*fragCoord)/RESOLUTION.yy;

  vec3 col = effect(p);
  gl_FragColor = vec4(col,1.0);
}

