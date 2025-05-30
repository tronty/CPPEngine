// https://www.shadertoy.com/view/NsdGDN
// "Urban Smog" by dr2 - 2021
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

#define AA  0   // (= 0/1) optional antialiasing

float PrBoxDf (vec3 p, vec3 b);
float PrCylDf (vec3 p, float r, float h);
float PrCapsDf (vec3 p, float r, float h);
float SmoothBump (float lo, float hi, float w, float x);
vec2 Rot2D (vec2 q, float a);
vec2 Rot2Cs (vec2 q, vec2 cs);
vec3 HsvToRgb (vec3 c);
float BumpFbm3 (vec3 p);
float Hashfv2 (vec2 p);
float Hashfv3 (vec3 p);
float Noisefv2 (vec2 p);
vec2 Noisev2v4 (vec4 p);
float Fbm1 (float p);
float IFbm1 (float p);
float Fbm2 (vec2 p);
vec3 VaryNf (vec3 p, vec3 n, float f);

mat3 flyerMat;
vec3 flyerPos, qHit, sunDir, noiseDisp;
vec2 cId, cIdT;
float dstFar, tCur, szFacFl, flrHt, bShape, bWid, bWidU, bHt, bHtU, hiMid,
   tWid, twHt, nFloor, fogAmp, fogTop;
int idObj;
const int idBldgF = 1, idBldgC = 2, idTwr = 3, idTwrTop = 4, idRoad = 5, idSWalk = 6,
   idFlyer = 7;
const float pi = 3.1415927;

#if 0
#define VAR_ZERO min (iFrame, 0)
#else
#define VAR_ZERO 0
#endif

#define DMINQ(id) if (d < dMin) { dMin = d;  idObj = id;  qHit = q; }

float FlyerDf (vec3 p)
{
  vec3 q, qq;
  float dMin, d, a;
  dMin = dstFar / szFacFl;
  q = flyerMat * (p - flyerPos) / szFacFl;
  q.x = abs (q.x);
  a = 0.22 * pi;
  q.z -= -0.25;
  qq = q;
  qq.xy = Rot2D (vec2 (abs (qq.x), qq.y), - a);
  d = abs (max (max (abs (dot (vec2 (qq.y, abs (dot (vec2 (qq.x, qq.z - 1.1),
     sin (0.012 * pi + vec2 (0.5 * pi, 0.))))), sin (a + vec2 (0.5 * pi, 0.)))) -
     0.002, 0.), max (dot (q.xz, sin (0.15 * pi + vec2 (0.5 * pi, 0.))) - 0.5, - q.z - 0.5))) - 0.02;
  DMINQ (idFlyer);
  return szFacFl * dMin;
}

float FlyerRay (vec3 ro, vec3 rd)
{
  float dHit, d;
  dHit = 0.;
  for (int j = VAR_ZERO; j < 120; j ++) {
    d = FlyerDf (ro + dHit * rd);
    dHit += d;
    if (d < 0.0002 || dHit > dstFar) break;
  }
  return dHit;
}

vec3 FlyerNf (vec3 p)
{
  vec4 v;
  vec2 e = vec2 (0.001, -0.001);
  for (int j = VAR_ZERO; j < 4; j ++) {
    v[j] = FlyerDf (p + ((j < 2) ? ((j == 0) ? e.xxx : e.xyy) : ((j == 2) ? e.yxy : e.yyx)));
  }
  v.x = - v.x;
  return normalize (2. * v.yzw - dot (v, vec4 (1.)));
}

void SetBldgParms ()
{  //  (based on "Urban Growth")
  bHt = (0.25 * Hashfv2 (13. * cId) + 0.05) * (1. + 0.15 / flrHt) + 0.1;
  nFloor = floor (bHt * (0.6 + 0.4 * length (sin (0.125 * pi * cId))) / flrHt);
  bHt = (nFloor + 0.3) * flrHt;
  bWid = floor ((0.17 /*0.2*/ + Hashfv2 (11. * cId) * 0.1) / flrHt + 0.5) * flrHt;
  bShape = Hashfv2 (17. * cId);
  cIdT = mod (cId + 8., vec2 (16.)) - 8.;
  hiMid = 0.75 * clamp (4. / max (length (cIdT), 1.), 0., 1.);
  bWidU = floor (bWid * (0.5 + 0.3 * Hashfv2 (12. * cId)) / flrHt + 0.5) * flrHt;
  bHt = (0.5 * Hashfv2 (13. * cId) + 0.05) * hiMid *
     (1.5 + (bWid - 0.15) / flrHt) + 0.1;
  bHtU = 0.25 * bHt + 0.75 * max (0., Hashfv2 (15. * cId) - 0.5) * hiMid + 0.05;
  bHt = (floor (bHt / flrHt) + 0.2) * flrHt;
  bHtU = floor (bHtU / flrHt) * flrHt;
  if (bHtU > 0.) bHtU += 0.2 * flrHt;
  if (cIdT == vec2 (0.)) {
    bHt = max (bHt, 40.2 * flrHt);
    bHtU = max (bHtU, 20.2 * flrHt);
  }
  tWid = ((bHtU > 0.) ? bWidU : bWid) - 0.0125;
}

float BldgDf (vec3 p, float dMin)
{
  vec3 pp, q;
  vec2 s;
  float d;
  q = p;
  d = q.y;
  DMINQ (idRoad);
  pp = p;
  pp.xz = fract (pp.xz) - vec2 (0.5);
  q = pp;
  q.y -= 0.0015;
  d = PrBoxDf (q, vec3 (0.35, 0.0015, 0.35));
  DMINQ (idSWalk);
  if (bHt > 0.) {
    q.y -= bHt - 0.2 * flrHt + 0.0005;
    if (bShape > 0.25) {
      d = PrBoxDf (q, vec3 (bWid, bHt, bWid));
      DMINQ (idBldgF);
    } else {
      d = PrCylDf (q.xzy, bWid, bHt);
      DMINQ (idBldgC);
    }
    q.y -= bHt + bHtU - 0.2 * flrHt - 0.001;
    if (bHtU > 0.) {
      if (bShape > 0.5) {
        d = max (PrBoxDf (q, vec3 (bWidU, bHtU, bWidU)),
           - PrBoxDf (q - vec3 (0., bHtU, 0.), vec3 (tWid, 0.1 * flrHt, tWid)));
        DMINQ (idBldgF);
      } else {
        d = max (PrCylDf (q.xzy, bWidU, bHtU),
           - PrCylDf ((q - vec3 (0., bHtU, 0.)).xzy, tWid, 0.1 * flrHt));
        DMINQ (idBldgC);
      }
    }
    q.y -= bHtU - 0.2 * flrHt - 0.001;
    if (bShape < 0.1) {
      d = PrCapsDf (q.xzy, 0.4 * bWidU, 1.25 * flrHt);
      DMINQ (idBldgC);
    } else if (bShape > 0.7) {
      d = PrBoxDf (q, vec3 (0.25 * bWidU, 1.25 * flrHt, 0.25 * bWidU));
      DMINQ (idBldgF);
    }
    if (bHt + bHtU > 30. * flrHt) {
      twHt = 0.1 * (bHt + bHtU);
      q.y -= twHt;
      d = PrCapsDf (q.xzy, 0.3 * flrHt, twHt);
      DMINQ (((q.y > 0.9 * twHt) ? idTwrTop : idTwr));
    }
    if (cIdT.x == 0. && cIdT.y == 0.) {
      q = pp;
      q.y -= 2. * (bHt + bHtU) + 0.2 * flrHt;
      d = PrCylDf (q.xzy, 0.3, flrHt);
      DMINQ (idBldgC);
    }
  }
  return dMin;
}

float ObjDf (vec3 p)
{
  return BldgDf (p, dstFar);
}

float ObjRay (vec3 ro, vec3 rd)
{
  vec3 p;
  vec2 cIdP, rdi, s;
  float dHit, d;
  if (rd.x == 0.) rd.x = 0.0001;
  if (rd.z == 0.) rd.z = 0.0001;
  rdi = 1. / rd.xz;
  cIdP = vec2 (-999.);
  dHit = 0.01;
  for (int j = VAR_ZERO; j < 160; j ++) {
    p = ro + dHit * rd;
    cId = floor (p.xz);
    if (cId != cIdP) {
      cIdP = cId;
      SetBldgParms ();
    }
    d = ObjDf (p);
    s = (cId + step (0., rd.xz) - p.xz) * rdi.xy;
    dHit += min (d, 0.001 + max (0., min (s.x, s.y)));
    if (d < 0.0005 || dHit > dstFar) break;
  }
  return dHit;
}

vec3 ObjNf (vec3 p)
{
  vec4 v;
  vec2 e;
  e = vec2 (0.0001, -0.0001);
  for (int j = VAR_ZERO; j < 4; j ++) {
    v[j] = ObjDf (p + ((j < 2) ? ((j == 0) ? e.xxx : e.xyy) : ((j == 2) ? e.yxy : e.yyx)));
  }
  v.x = - v.x;
  return normalize (2. * v.yzw - dot (v, vec4 (1.)));
}

vec4 BldgCol (vec3 vn)
{
  vec4 col4;
  vec2 g;
  float wFac, f;
  wFac = 1.;
  col4 = vec4 (HsvToRgb (vec3 (0.7 * Hashfv2 (19. * cId), 0.2,
     0.4 + 0.2 * Hashfv2 (21. * cId))), 0.3);
  if (abs (vn.y) < 0.05) {
    f = mod (qHit.y / flrHt - 0.2, 1.) - 0.5;
    wFac = 1. - (step (0., f) - 0.5) * step (abs (abs (f) - 0.24), 0.02) -
       0.801 * step (abs (f), 0.22);
    if (wFac < 0.2) {
      f = (idObj == idBldgF) ? 1.5 * dot (qHit.xz, normalize (vn.zx)) :
         length (qHit.xz) * (atan (qHit.z, qHit.x) + 0.5 * pi);
      wFac = min (0.2 + 0.8 * floor (fract (f / flrHt + 0.25) *
         (1. + Hashfv2 (51. * cId))), 1.);
    }
    col4.rgb *= wFac;
  } else if (vn.y > 0.95) {
    g = step (0.05, fract (qHit.xz * 70.));
    col4.rgb *= mix (0.8, 1., g.x * g.y);
  }
  if (vn.y < 0.95 && wFac > 0.5)
     col4.rgb *= (0.8 + 0.2 * Noisefv2 (512. * vec2 (qHit.x + qHit.z, qHit.y)));
  if (wFac < 0.5) col4.a = -2.;
  return col4;
}

vec4 ObjCol (vec3 ro, vec3 rd, vec3 vn)
{
  vec4 col4;
  col4 = vec4 (0.);
  if (idObj == idBldgF || idObj == idBldgC) {
    col4 = BldgCol (vn);
  } else if (idObj == idTwr) {
    col4 = vec4 (0.3);
  } else if (idObj == idTwrTop) {
    col4 = vec4 (1., 0., 0., -2.);
  } else if (idObj == idRoad) {
    col4.rgb = vec3 (0.15, 0.15, 0.17);
  } else if (idObj == idSWalk) {
    col4.rgb = vec3 (0.2);
  }
  return col4;
}

float FogHt (vec2 p)
{  // (based on "Misty Terraces")
  mat2 qRot;
  vec4 t4;
  vec2 q, t, tw;
  float wAmp, h;
  qRot = mat2 (0.8, -0.6, 0.6, 0.8);
  q = p + vec2 (0., 0.05 * tCur);
  wAmp = 1.;
  h = 0.;
  tw = 0.05 * tCur * vec2 (1., -1.);
  for (int j = 0; j < 5; j ++) {
    q *= 2. * qRot;
    t4 = abs (sin (2. * (q.xyxy + tw.xxyy) + 2. * Noisev2v4 (t4).xxyy - 1.));
    t4 = (1. - t4) * (t4 + sqrt (1. - t4 * t4));
    t = 1. - sqrt (t4.xz * t4.yw);
    t *= t;
    h += wAmp * dot (t, t);
    wAmp *= 0.5;
  }
  return fogTop - 0.5 * h;
}

float FogDens (vec3 p)
{
  return fogAmp * (0.2 + 0.8 * smoothstep (0., 1., 1. - p.y / fogTop)) *
     BumpFbm3 (0.03 * (p + noiseDisp)) * smoothstep (0., 1., FogHt (0.1 * p.xz) - p.y);
}

vec3 FogCol (vec3 col, vec3 ro, vec3 rd, float dHit)
{
  float s, ds, f, fn;
  s = 1.;
  ds = 1.;
  fn = FogDens (ro + s * rd);
  for (int j = VAR_ZERO; j < 40; j ++) {
    s += ds;
    f = fn;
    fn = FogDens (ro + (s + 0.5 * ds * Hashfv3 (16. * rd)) * rd);
    col = mix (col, vec3 (0.95, 0.95, 1.) * (1. - clamp (f - fn, 0., 1.)),
       min (f * (1. - smoothstep (0.5 * dHit, dHit, s)), 1.));
    if (s > dHit) break;
  }
  return col;
}

vec3 BgCol (vec3 ro, vec3 rd)
{
  vec3 p, col, clCol;
  float fd, f, ff;
  rd.y = (rd.y + 0.1) / 1.1;
  rd = normalize (rd);
  col = vec3 (0.7);
  if (rd.y > 0.) {
    p = rd * (200. - ro.y) / max (rd.y, 0.0001);
    p += ro + 4. * tCur;
    p *= 0.01;
    ff = Fbm2 (p.xz);
    f = smoothstep (0.2, 0.8, ff);
    fd = smoothstep (0.2, 0.8, Fbm2 (p.xz + 0.01 * sunDir.xz)) - f;
    clCol = (0.7 + 0.5 * ff) * (vec3 (0.7) - 0.7 * vec3 (0.3, 0.3, 0.2) * sign (fd) *
       smoothstep (0., 0.05, abs (fd)));
    fd = smoothstep (0.01, 0.1, rd.y);
    col = mix (mix (vec3 (0.7, 0.7, 0.75), vec3 (0.4, 0.5, 0.8), 0.3 + 0.7 * fd),
       clCol, 0.1 + 0.9 * f * fd);
  }
  return col;
}

vec3 ShowScene (vec3 ro, vec3 rd)
{
  vec4 col4;
  vec3 col, vn, roo, rdr;
  vec2 g, vf;
  float dstObj, dstFlyer, s, f;
  noiseDisp = 0.02 * tCur * vec3 (-1., 0., 1.) + 0.5 * sin (vec3 (0.2, 0.1, 0.3) * pi * tCur);
  fogAmp = 1.;
  fogTop = 4.;
  roo = ro;
  vf = vec2 (0.);
  dstFlyer = FlyerRay (ro, rd);
  dstObj = ObjRay (ro, rd);
  if (dstObj < min (dstFlyer, dstFar)) {
    ro += dstObj * rd;
    cId = floor (ro.xz);
    SetBldgParms ();
    vn = ObjNf (ro);
    col4 = ObjCol (ro, rd, vn);
    col = col4.rgb;
    if (col4.a >= 0.) {
      if (idObj == idRoad) vf = vec2 (512., 1.);
      else if (idObj == idBldgF || idObj == idBldgC) vf = vec2 (512., 0.5);
      if (vf.x > 0.) vn = VaryNf (vf.x * qHit, vn, vf.y);
      col = col * (0.3 + 0.3 * max (dot (vn.xz, - normalize (sunDir.xz)), 0.) +
         0.2 * max (vn.y, 0.) + 0.7 * max (dot (vn, sunDir), 0.)) +
         col4.a * pow (max (dot (normalize (sunDir - rd), vn), 0.), 32.);
    } else if (col4.a == -2.) {
      rdr = reflect (rd, vn);
      g = Rot2D (rdr.xz, 5.1 * atan (20. + cId.y, 20. + cId.x));
      f = step (1., 0.3 * ro.y + 4. * rdr.y -
         0.1 * floor (5. * IFbm1 (0.3 * atan (g.y, g.x) + pi) + 0.05));
      if (idObj == idBldgF || idObj == idBldgC) {
        if (f == 1.) col = mix (0.7 * col, BgCol (ro, rdr), 0.5);
        else col += 0.05;
      }
    }
    if (col4.a == -2.) col *= 1. - 0.95 * smoothstep (0.4, 0.7, dstObj / dstFar);
    col = mix (col, vec3 (0.3), smoothstep (0.6, 1., dstObj / dstFar));
  } else if (dstFlyer < dstFar) {
    ro += dstFlyer * rd;
    vn = FlyerNf (ro);
    col = vec3 (0.3, 1., 0.3);
    col = mix (col, 1. - col, smoothstep (0.02, 0.04, abs (length (vec2 (qHit.xz -
       vec2 (0.3, -0.1))) - 0.17)) * (1. - smoothstep (0.95, 0.97, qHit.z)));
    col = col * (0.2 + 0.2 * max (- dot (vn, sunDir), 0.) + 0.8 * max (dot (vn, sunDir), 0.)) +
       0.2 *  pow (max (dot (normalize (sunDir - rd), vn), 0.), 32.);
  } else col = BgCol (ro, rd);
  col = FogCol (col, roo, rd, min (dstFlyer, dstObj));
  return clamp (col, 0., 1.);
}

vec3 TrackPath (float t)
{ 
  vec3 p;
  float pLen, s, cCount;
  pLen = 2.;
  p.y = 2.5;
  s = mod (t, 11.);
  if (s < 7.) p.xz = (s < 4.) ? vec2 (0., s) : vec2 (s - 4., 4.);
  else p.xz = (s < 9.) ? vec2 (3., 11. - s) : vec2 (12. - s, 2.);
  cCount = floor (t / 11.);
  if (mod (cCount, 2.) == 0.) p.x *= -1.;
  else p.x -= 1.;
  p.z += 2. * cCount;
  p.xz *= pLen;
  return p;
}

void FlyerPM (float s, float rl, float vu, out vec3 flPos, out mat3 flMat)
{
  vec3 fpF, fpB, vel, acc, va, ort, ca, sa;
  float ds, rlFac, elFac, el, az;
  ds = 0.12;
  elFac = 0.7;
  rlFac = 0.02;
  fpF = 0.5 * (TrackPath (s + 2. * ds) + TrackPath (s + ds));
  fpB = 0.5 * (TrackPath (s - 2. * ds) + TrackPath (s - ds));
  vel = normalize (fpF - fpB);
  acc = (fpF - 2. * TrackPath (s) + fpB) / (2.25 * ds * ds);
  flPos = 0.5 * (fpF + fpB);
  va = cross (acc, vel);
  el = - elFac * asin (vel.y);
  az = atan (vel.z, vel.x) - 0.5 * pi;
  rl += rlFac * length (va) * sign (va.y);
  ort = (vu >= 0.) ? vec3 (el, az, rl) : vec3 (- el, az + pi, - rl);
  ca = cos (ort);
  sa = sin (ort);
  flMat = mat3 (ca.z, - sa.z, 0., sa.z, ca.z, 0., 0., 0., 1.) *
          mat3 (1., 0., 0., 0., ca.x, - sa.x, 0., sa.x, ca.x) *
          mat3 (ca.y, 0., - sa.y, 0., 1., 0., sa.y, 0., ca.y);
}

void main( )
{
  mat3 vuMat, flMat;
  vec4 mPtr, dateCur;
  vec3 ro, rd, col, ori, ca, sa, vd;
  vec2 canvas, uv, uvv;
  float spd, el, az, zmFac, asp, sr, tPos;
  canvas = iResolution.xy;
  uv = 2. * xlv_TEXCOORD0.xy / canvas - 1.;
  uv.x *= canvas.x / canvas.y;
  tCur = iTime;
  dateCur = iDate;
  mPtr = iMouse;
  mPtr.xy = mPtr.xy / canvas - 0.5;
  tCur = mod (tCur, 2400.) + 30. * floor (dateCur.w / 7200.);
  el = 0.;
  az = 0.;
  spd = 0.3;
  tPos = spd * tCur;
  if (mPtr.z > 0.) {
    az = 2. * pi * mPtr.x;
    el = clamp (0.45 * pi * mPtr.y, -0.18 * pi, 0.18 * pi);
  }
  zmFac = 2.5;
  asp = canvas.x / canvas.y;
  szFacFl = 0.04;
  FlyerPM (tPos, 0.2 * pi * (Fbm1 (3.5 * tPos) - 0.5), 1., flyerPos, flyerMat);
  flyerPos.y -= 0.2;
  FlyerPM (tPos - 0.3, 0., 0., ro, flMat);
  ro.xz += 0.01;
  ori = vec3 (-0.02 * pi, 0.001 * pi, 0.1 * pi * (Fbm1 (1.5 * tPos) - 0.5));
  ori.xy += vec2 (el, az);
  ca = cos (ori);
  sa = sin (ori);
  vuMat = mat3 (ca.y, 0., - sa.y, 0., 1., 0., sa.y, 0., ca.y) *
          mat3 (1., 0., 0., 0., ca.x, - sa.x, 0., sa.x, ca.x) *
          mat3 (ca.z, - sa.z, 0., sa.z, ca.z, 0., 0., 0., 1.);
  dstFar = 50.;
  flrHt = 0.07;
  sunDir = normalize (vec3 (1., 2., -1.));
#if ! AA
  const float naa = 1.;
#else
  const float naa = 3.;
#endif  
  col = vec3 (0.);
  sr = 2. * mod (dot (mod (floor (0.5 * (uv + 1.) * canvas), 2.), vec2 (1.)), 2.) - 1.;
  for (float a = float (VAR_ZERO); a < naa; a ++) {
    uvv = uv + step (1.5, naa) * Rot2D (vec2 (0.5 / canvas.y, 0.), sr * (0.667 * a + 0.5) * pi);
    rd = normalize (vec3 (2. * tan (0.5 * atan (uvv.x / (asp * zmFac))) * asp, uvv.y / zmFac, 1.));
    rd = (vuMat * rd) * flMat;
    col += (1. / naa) * ShowScene (ro, rd);
  }
  gl_FragColor = vec4 (col, 1.);
}

float PrBoxDf (vec3 p, vec3 b)
{
  vec3 d;
  d = abs (p) - b;
  return min (max (d.x, max (d.y, d.z)), 0.) + length (max (d, 0.));
}

float PrCylDf (vec3 p, float r, float h)
{
  return max (length (p.xy) - r, abs (p.z) - h);
}

float PrCapsDf (vec3 p, float r, float h)
{
  return length (p - vec3 (0., 0., clamp (p.z, - h, h))) - r;
}

float SmoothBump (float lo, float hi, float w, float x)
{
  return (1. - smoothstep (hi - w, hi + w, x)) * smoothstep (lo - w, lo + w, x);
}

vec2 Rot2D (vec2 q, float a)
{
  vec2 cs;
  cs = sin (a + vec2 (0.5 * pi, 0.));
  return vec2 (dot (q, vec2 (cs.x, - cs.y)), dot (q.yx, cs));
}

vec2 Rot2Cs (vec2 q, vec2 cs)
{
  return vec2 (dot (q, vec2 (cs.x, - cs.y)), dot (q.yx, cs));
}

vec3 HsvToRgb (vec3 c)
{
  return c.z * mix (vec3 (1.), clamp (abs (fract (c.xxx + vec3 (1., 2./3., 1./3.)) * 6. - 3.) - 1., 0., 1.), c.y);
}

float PerBumpf (float p)
{
  return 0.5 * smoothstep (0., 0.5, abs (fract (p) - 0.5));
}

vec3 PerBumpv3 (vec3 p)
{
  return 0.5 * smoothstep (0., 0.5, abs (fract (p) - 0.5));
}

float BumpFbm3 (vec3 p)
{
  vec3 q;
  float a, f;
  a = 1.;
  f = 0.;
  q = p;
  for (int j = 0; j < 4; j ++) {
    p += PerBumpv3 (q + PerBumpv3 (q).yzx);
    p *= 1.5;
    f += a * (PerBumpf (p.z + PerBumpf (p.x + PerBumpf (p.y))));
    q = 2. * q + 0.5;
    a *= 0.75;
  }
  return f;
}

const float cHashM = 43758.54;

float Hashff (float p)
{
  return fract (sin (p) * cHashM);
}

float Hashfv2 (vec2 p)
{
  return fract (sin (dot (p, vec2 (37., 39.))) * cHashM);
}

float Hashfv3 (vec3 p)
{
  return fract (sin (dot (p, vec3 (37., 39., 41.))) * cHashM);
}

vec2 Hashv2f (float p)
{
  return fract (sin (p + vec2 (0., 1.)) * cHashM);
}

vec2 Hashv2v2 (vec2 p)
{
  vec2 cHashVA2 = vec2 (37., 39.);
  return fract (sin (dot (p, cHashVA2) + vec2 (0., cHashVA2.x)) * cHashM);
}

vec4 Hashv4f (float p)
{
  return fract (sin (p + vec4 (0., 1., 57., 58.)) * cHashM);
}

vec4 Hashv4v3 (vec3 p)
{
  vec3 cHashVA3 = vec3 (37., 39., 41.);
  return fract (sin (dot (p, cHashVA3) + vec4 (0., cHashVA3.xy, cHashVA3.x + cHashVA3.y)) * cHashM);
}

float Noiseff (float p)
{
  vec2 t;
  float ip, fp;
  ip = floor (p);
  fp = fract (p);
  fp = fp * fp * (3. - 2. * fp);
  t = Hashv2f (ip);
  return mix (t.x, t.y, fp);
}

float Noisefv2 (vec2 p)
{
  vec2 t, ip, fp;
  ip = floor (p);  
  fp = fract (p);
  fp = fp * fp * (3. - 2. * fp);
  t = mix (Hashv2v2 (ip), Hashv2v2 (ip + vec2 (0., 1.)), fp.y);
  return mix (t.x, t.y, fp.x);
}

vec2 Noisev2v4 (vec4 p)
{
  vec4 ip, fp, t1, t2;
  ip = floor (p);
  fp = fract (p);
  fp = fp * fp * (3. - 2. * fp);
  t1 = Hashv4f (dot (ip.xy, vec2 (1., 57.)));
  t2 = Hashv4f (dot (ip.zw, vec2 (1., 57.)));
  return vec2 (mix (mix (t1.x, t1.y, fp.x), mix (t1.z, t1.w, fp.x), fp.y),
               mix (mix (t2.x, t2.y, fp.z), mix (t2.z, t2.w, fp.z), fp.w));
}

float Fbm1 (float p)
{
  float f, a;
  f = 0.;
  a = 1.;
  for (int j = 0; j < 5; j ++) {
    f += a * Noiseff (p);
    a *= 0.5;
    p *= 2.;
  }
  return f * (1. / 1.9375);
}

float IFbm1 (float p)
{
  float s, a;
  p *= 5.;
  s = 0.;
  a = 10.;
  for (int j = 0; j < 4; j ++) {
    s += floor (a * Noiseff (p));
    a *= 0.5;
    p *= 2.;
  }
  return 0.1 * s;
}

float Fbm2 (vec2 p)
{
  float f, a;
  f = 0.;
  a = 1.;
  for (int j = 0; j < 5; j ++) {
    f += a * Noisefv2 (p);
    a *= 0.5;
    p *= 2.;
  }
  return f * (1. / 1.9375);
}

float Fbmn (vec3 p, vec3 n)
{
  vec3 s;
  float a;
  s = vec3 (0.);
  a = 1.;
  for (int j = 0; j < 5; j ++) {
    s += a * vec3 (Noisefv2 (p.yz), Noisefv2 (p.zx), Noisefv2 (p.xy));
    a *= 0.5;
    p *= 2.;
  }
  return dot (s, abs (n));
}

vec3 VaryNf (vec3 p, vec3 n, float f)
{
  vec4 v;
  vec3 g;
  vec2 e = vec2 (0.1, 0.);
  for (int j = VAR_ZERO; j < 4; j ++)
     v[j] = Fbmn (p + ((j < 2) ? ((j == 0) ? e.xyy : e.yxy) : ((j == 2) ? e.yyx : e.yyy)), n);
  g = v.xyz - v.w;
  return normalize (n + f * (g - n * dot (n, g)));
}

