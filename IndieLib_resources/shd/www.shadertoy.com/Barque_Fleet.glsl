// "Barque Fleet" by dr2 - 2023
// License: Creative Commons Attribution-NonCommercial-ShareAlike 4.0

/*
  (Bits from "Sailing" and "Franky and the Drakettes")

  No. 9 in "Sailing Ship" series
    "Ship in a Shell"          (XlBfRR)
    "Sailing"                  (MtBBRR)
    "Tempting the Mariner"     (ll2BzR)
    "Lighthouse with Ship"     (MtSBR1)
    "Ship of Mirrors"          (4ljfzw)
    "Refractable Ship"         (MtjBRm)
    "Ship in a Bottle"         (wlXXzf)
    "Not The Ship Stamp"       (ftsSWS)
*/

#define AA    0  // (= 0/1) optional antialiasing

#if 1
#define VAR_ZERO min (iFrame, 0)
#else
#define VAR_ZERO 0
#endif

float PrBoxDf (vec3 p, vec3 b);
float PrBox2Df (vec2 p, vec2 b);
float PrCylDf (vec3 p, float r, float h);
float PrCapsDf (vec3 p, float r, float h);
float PrEVCapsDf (vec3 p, vec4 u, float r);
float PrEECapsDf (vec3 p, vec3 v1, vec3 v2, float r);
float Minv2 (vec2 p);
float Maxv2 (vec2 p);
float SmoothBump (float lo, float hi, float w, float x);
vec2 Rot2D (vec2 q, float a);
vec3 HsvToRgb (vec3 c);
float Hashfv2 (vec2 p);
vec2 Noisev2v4 (vec4 p);
float Fbm2 (vec2 p);
float Fbm3 (vec3 p);

vec4 vum[4], vur[5];
vec3 qHit, sunDir, shipConf, bDeck, cloudDisp, vim[4], vir[5];
vec2 cId, bGrid, grLim, shipOff;
float tCur, dstFar, dstWat, szFac, tWav;
int idObj;
const int idHull = 1, idRud = 2, idStruc = 3, idMast = 4, idSparT = 5, idSparL = 6, idSailT = 7,
   idSailA = 8, idSailF = 9, idFlag = 10, idRig = 11;
const float pi = 3.1415927;

#define DMINQ(id) if (d < dMin) { dMin = d;  idObj = id;  qHit = q; }

float WaveHt (vec2 p)
{
  mat2 qRot = mat2 (1.6, -1.2, 1.2, 1.6);
  vec4 t4;
  vec2 q, t, tw;
  float wFreq, wAmp, h;
  q = 0.2 * p - vec2 (0., tCur);
  h = 0.59 + 0.25 * sin (0.5 * q.y + 0.5) + 0.12 * sin (dot (q, vec2 (0.1, 1.2))) +
     0.12 * sin (dot (q, vec2 (-0.2, 1.4)));
  q = p;
  wFreq = 0.25;
  wAmp = 0.05;
  tw = tWav * vec2 (1., -1.);
  for (int j = VAR_ZERO; j < 3; j ++) {
    t4 = wFreq * (q.xyxy + tw.xxyy);
    t4 = abs (sin (t4 + 2. * Noisev2v4 (t4).xxyy - 1.));
    t4 = (1. - t4) * (t4 + sqrt (1. - t4 * t4));
    t = 1. - sqrt (t4.xz * t4.yw);
    t *= t;
    h += wAmp * dot (t, t);
    q *= qRot;
    wFreq *= 2.;
    wAmp *= 0.25;
  }
  return h;
}

float WaveRay (vec3 ro, vec3 rd)
{
  vec3 p;
  float dHit, h, s, sLo, sHi;
  s = 0.;
  sLo = 0.;
  dHit = dstFar;
  for (int j = VAR_ZERO; j < 50; j ++) {
    p = ro + s * rd;
    h = p.y - WaveHt (p.xz);
    if (h < 0.) break;
    sLo = s;
    s += max (6., h);
    if (s > dstFar) break;
  }
  if (h < 0.) {
    sHi = s;
    for (int j = VAR_ZERO; j < 7; j ++) {
      s = 0.5 * (sLo + sHi);
      p = ro + s * rd;
      if (p.y > WaveHt (p.xz)) sLo = s;
      else sHi = s;
    }
    dHit = sHi;
  }
  return dHit;
}

vec4 WaveNf (vec3 p, float d)
{
  vec3 h;
  vec2 e, ee[3];
  e = vec2 (max (0.1, 1e-4 * d * d), 0.);
  ee[0] = e.yy;
  ee[1] = e.xy;
  ee[2] = e.yx;
  for (int j = VAR_ZERO; j < 3; j ++) h[j] = WaveHt (p.xz + ee[j]);
  return vec4 (normalize (vec3 (h.x - h.yz, e.x)).xzy, h.x);
}

float ShipHullDf (vec3 p, float dMin)
{
  vec3 q;
  float d, fy, fz, gz;
  q = p;
  d = abs (p.z) - 4.5;
  q.z = mod (q.z + 1.4, 2.8) - 1.4;
  q.yz -= vec2 (-3.4, -0.4);
  d = max (d, PrBoxDf (q, vec3 (0.3, 0.1, 0.5)));
  DMINQ (idStruc);
  q = p;
  q.x = abs (q.x);
  q.yz -= vec2 (-3.8, 0.5);
  fz = q.z / 5. + 0.3;
  fz *= fz;
  fy = 1. - smoothstep (-1.3, -0.1, q.y);
  gz = smoothstep (2., 5., q.z);
  bDeck = vec3 ((1. - 0.45 * fz * fz) * (1.1 - 0.5 * fy * fy) *
     (1. - 0.5 * smoothstep (-5., -2., q.y) * smoothstep (2., 5., q.z)),
     0.78 - 0.8 * gz * gz - 0.2 * (1. - smoothstep (-5.2, -4., q.z)), 5. * (1. + 0. * 0.02 * q.y));
  d = min (PrBoxDf (vec3 (q.x, q.y + bDeck.y - 0.6, q.z), bDeck),
     max (PrBoxDf (q - vec3 (0., 0.72, -4.6), vec3 (bDeck.x, 0.12, 0.4)),
     - PrBox2Df (vec2 (abs (q.x) - 0.4, q.y - 0.65), vec2 (0.2, 0.08))));
  d = max (d, - PrBoxDf (vec3 (q.x, q.y - 0.58 - 0.1 * fz, q.z),
     vec3 (bDeck.x - 0.07, 0.3, bDeck.z - 0.1)));
  q = p;
  d = max (d, - max (PrBox2Df (vec2 (q.y + 3.35, mod (q.z + 0.25, 0.5) - 0.25), vec2 (0.08, 0.1)),
     abs (q.z + 0.5) - 3.75));
  DMINQ (idHull);
  q = p;
  d = PrBoxDf (q + vec3 (0., 4.4, 4.05), vec3 (0.03, 0.35, 0.5));
  DMINQ (idRud);
  return dMin;
}

float ShipMastDf (vec3 p, float dMin)
{
  vec3 q, qq;
  float d, fy, fz, s, rSpar, yLim, zLim;
  rSpar = 0.05;
  fy = 1. - 0.07 * p.y;
  fz = 1. - 0.14 * step (1., abs (p.z));
  zLim = abs (p.z) - 4.5;
  q = p;
  d = zLim;
  q.z = mod (q.z + 1.4, 2.8) - 1.2;
  d = max (d, PrCapsDf ((q - vec3 (0., 3.7 * (fz - 1.), 0.)).xzy, 0.1 * fy, 3.7 * fz));
  DMINQ (idMast);
  q = p;
  yLim = abs (q.y - 0.2 * fz) - 3. * fz;
  qq = q;
  qq.y = mod (qq.y - 3.3 * (fz - 1.), 2. * fz) - fz;
  qq.z = mod (qq.z + 1.4, 2.8) - 1.4 + 0.1 * fz;
  d = max (max (min (d, PrCylDf (vec3 (qq - vec3 (0., 0.05 * fy * fz, 0.1 * fz - 0.23)).xzy,
     0.15 * fy, 0.11 * fy * fz)), yLim), zLim);
  DMINQ (idMast);
  d = max (max (PrCapsDf (qq.yzx, 0.05, 1.23 * fy * fz), yLim), zLim);
  DMINQ (idSparT);
  q = p;
  for (int k = 0; k <= 3; k ++) d = min (d, PrEVCapsDf (q - vim[k], vum[k], rSpar));
  DMINQ (idSparL);
  return dMin;
}

float ShipSailDf (vec3 p, float dMin)
{
  vec3 q, qq, w;
  float d, fy, fz;
  fy = 1. - 0.07 * p.y;
  fz = 1. - 0.14 * step (1., abs (p.z));
  q = p;
  qq = q;
  qq.y = mod (qq.y - 3.1 * (fz - 1.), 2. * fz) - fz;
  qq.z = mod (qq.z + 1.4, 2.8) - 1.4 + 0.2 * (fz - abs (qq.y)) * (fz - abs (qq.y)) - 0.1 * fz;
  d = max (max (max (PrBoxDf (qq, vec3 ((1.2 - 0.07 * q.y) * fz, fz, 0.01)),
     min (qq.y, 1.5 * fy * fz - length (vec2 (qq.x, qq.y + 0.9 * fy * fz)))),
     abs (q.y - 3. * (fz - 1.)) - 2.95 * fz), - PrBox2Df (qq.yz, vec2 (0.01 * fz)));
  d = max (d, abs (p.z) - 4.5);
  DMINQ (idSailT);
  q = p;
  q.z -= -3.8;
  q.y -= -1.75 - 0.2 * q.z;
  d = PrBoxDf (q, vec3 (0.01, 0.9 - 0.2 * q.z, 0.6));
  DMINQ (idSailA);
  q = p;
  q.yz -= vec2 (-1., 4.5);
  w = vec3 (1., q.yz);
  d = max (max (max (abs (q.x) - 0.01, - dot (w, vec3 (2.3, 1., -0.35))),
     - dot (w, vec3 (0.68, -0.74, -1.))), - dot (w, vec3 (0.41, 0.4, 1.)));
  DMINQ (idSailF);
  q = p;
  q.yz -= vec2 (3.4, 0.18);
  d = PrBoxDf (q, vec3 (0.01, 0.2, 0.3));
  DMINQ (idFlag);
  return dMin;
}

float ShipRigDf (vec3 p, float dMin)
{
  vec3 q;
  float rRig, d, fz, gz, s;
  rRig = 0.02;
  fz = 1. - 0.14 * step (1., abs (p.z));
  q = p;
  d = abs (p.z) - 4.5;
  gz = (q.z - 0.5) / 5. + 0.3;
  gz *= gz;
  gz = 1.05 * (1. - 0.45 * gz * gz);
  q.x = abs (q.x);
  q.z = mod (q.z + 1.4, 2.8) - 1.4;
  d = max (d, min (PrEECapsDf (q, vec3 (1.05 * gz, -3.25, -0.5),
     vec3 (1.4 * fz, -2.95, -0.05), 0.7 * rRig),
     PrEECapsDf (vec3 (q.xy, abs (q.z + 0.2) - 0.01 * (0.3 - 2. * q.y)), vec3 (gz, -3.2, 0.),
     vec3 (0.05, -0.9 + 2. * (fz - 1.), 0.), rRig)));
  q = p;
  d = min (d, PrEVCapsDf (q - vir[0], vur[0], 0.8 * rRig));
  for (int k = 1; k <= 3; k ++) d = min (d, PrEVCapsDf (q - vir[k], vur[k], rRig));
  q.x = abs (q.x);
  d = min (d, PrEVCapsDf (q - vir[4], vur[4], rRig));
  s = step (1.8, q.y) - step (q.y, -0.2);
  d = min (min (d, min (PrEECapsDf (q, vec3 (0.95, 0.4, 2.7) + vec3 (-0.1, 1.7, 0.) * s,
     vec3 (0.05, 1.1, -0.15) + vec3 (0., 2., 0.) * s, rRig),
     PrEECapsDf (q, vec3 (1.05, 1., -0.1) + vec3 (-0.1, 2., 0.) * s,
     vec3 (0.05, 0.5, -2.95) + vec3 (0., 1.7, 0.) * s, rRig))),
     PrEECapsDf (q, vec3 (0.95, 0.4, -2.9) + vec3 (-0.1, 1.7, 0.) * s,
     vec3 (0.05, 0.9, -0.25) + vec3 (0., 2., 0.) * s, rRig));
  DMINQ (idRig);
  return dMin;
}

#define VL(x) vec4 (normalize (x), length (x))

void SRigConsts ()
{
  vim[0] = vec3 (0., -3.5, 4.3);    vum[0] = VL (vec3 (0., -2.6, 6.7) - vim[0]);
  vim[1] = vec3 (0., -4., 4.1);     vum[1] = VL (vec3 (0., -2.9, 6.) - vim[1]);
  vim[2] = vec3 (0., -1.2, -3.);    vum[2] = VL (vec3 (0., -0.5, -4.5) - vim[2]);
  vim[3] = vec3 (0., -2.7, -3.);    vum[3] = VL (vec3 (0., -2.7, -4.5) - vim[3]);
  vir[0] = vec3 (0., -3., -4.45);   vur[0] = VL (vec3 (0., -2.7, -4.5) - vir[0]);
  vir[1] = vec3 (0., 2.45, 2.65);   vur[1] = VL (vec3 (0., -2.7, 6.5) - vir[1]);
  vir[2] = vec3 (0., 2.5, 2.65);    vur[2] = VL (vec3 (0., -3.2, 4.9) - vir[2]);
  vir[3] = vec3 (0., 2.6, -3.);     vur[3] = VL (vec3 (0., -0.5, -4.5) - vir[3]);
  vir[4] = vec3 (0.65, -3.5, 3.5);  vur[4] = VL (vec3 (0.05, -2.7, 6.4) - vir[4]);
}

void ShipState ()
{
  vec4 h;
  vec2 g, e, ee[4];
  szFac = (length (cId) == 0.) ? 1. : 0.7;
  shipOff = 2. * sin (0.01 * pi * (Hashfv2 (cId + 0.5) - 0.5) * (tCur + 100.) +
     vec2 (0.5 * pi, 0.));
  g = bGrid * (cId + 0.5) + shipOff;
  e = vec2 (2.5 * szFac, 0.);
  ee[0] = - e;
  ee[1] = e;
  ee[2] = - e.yx;
  ee[3] = e.yx;
  for (int j = VAR_ZERO; j < 4; j ++) h[j] = WaveHt (g + ee[j]);
  shipConf = vec3 (0.25 * dot (h, vec4 (1.)), vec2 (h.xz - h.yw) / (2. * e.x));
}

float ObjDf (vec3 p)
{
  vec3 q;
  float dMin;
  p.xz -= bGrid * (cId + 0.5) + shipOff;
  p.yz = Rot2D (p.yz, -0.4 * shipConf.z);
  p.xy = Rot2D (p.xy, 3. * shipConf.y);
  p.y -= shipConf.x + 4.2 * szFac;
  p.xz *= -1.;
  q = p;
  q /= szFac;
  dMin = dstFar / szFac;
  dMin = ShipHullDf (q, dMin);
  dMin = ShipMastDf (q, dMin);
  dMin = ShipSailDf (q, dMin);
  dMin = ShipRigDf (q, dMin);
  return 0.7 * szFac * dMin;
}

float ObjRay (vec3 ro, vec3 rd)
{
  vec3 p, rdi;
  vec2 cIdP, s;
  float dHit, d, eps;
  eps = 0.0005;
  if (rd.x == 0.) rd.x = 0.001;
  if (rd.z == 0.) rd.z = 0.001;
  rdi.xz = 1. / rd.xz;
  cIdP = vec2 (-999.);
  dHit = eps;
  for (int j = VAR_ZERO; j < 160; j ++) {
    p = ro + dHit * rd;
    cId = floor (p.xz / bGrid);
    if (cId != cIdP) {
      ShipState ();
      cIdP = cId;
    }
    d = (Maxv2 (abs (cId) - grLim) <= 0.) ? ObjDf (p) : dstFar;
    s = (bGrid * (cId + step (0., rd.xz)) - p.xz) * rdi.xz;
    d = min (d, abs (Minv2 (s)) + eps);
    dHit += d;
    if (d < eps || dHit > dstFar || p.y < 0.) break;
  }
  if (d >= eps) dHit = dstFar;
  return dHit;
}

vec3 ObjNf (vec3 p)
{
  vec4 v;
  vec2 e;
  e = vec2 (0.0005, -0.0005);
  for (int j = VAR_ZERO; j < 4; j ++) {
    v[j] = ObjDf (p + ((j < 2) ? ((j == 0) ? e.xxx : e.xyy) : ((j == 2) ? e.yxy : e.yyx)));
  }
  v.x = - v.x;
  return normalize (2. * v.yzw - dot (v, vec4 (1.)));
}

float MSymCol (vec2 s)
{
  return min (abs (length (s) - 0.1) - 0.02, PrBox2Df (Rot2D (s, 2. * pi * floor (3. *
     atan (s.y, - s.x) / (2. * pi) + 0.5) / 3.) + vec2 (0.05, 0.), vec2 (0.05, 0.02)));
}

vec4 ShipCol ()
{
  vec4 col4;
  vec3 cSym;
  vec2 cg;
  cSym = (length (cId) > 0.) ? HsvToRgb (vec3 (Hashfv2 (17.11 * cId), 0.8, 1.)) : vec3 (0.1);
  if (idObj == idHull) {
    if (abs (qHit.x) < bDeck.x - 0.08 && qHit.y > -3.6 && qHit.z > - bDeck.z + 0.62) {
      col4 = vec4 (0.5, 0.3, 0., 0.1) * (0.5 +
         0.5 * SmoothBump (0.05, 0.95, 0.02, fract (8. * qHit.x)));
    } else if (qHit.y > -4.) {
      col4 = vec4 (0.7, 0.5, 0.1, 0.1);
      if (abs (qHit.z - 4.) < 0.25 && abs (qHit.y + 3.55) < 0.05) col4 *= 1.2;
      else if (qHit.z < -4. && abs (qHit.x) < 0.84 && abs (qHit.y + 3.62) < 0.125) {
        cg = step (0.1, abs (fract (vec2 (6. * qHit.x, 8. * (qHit.y + 3.62)) + 0.5) - 0.5));
        if (cg.x * cg.y == 1.) col4 = vec4 (0.8, 0.8, 0.2, -1.);
        else col4 *= 0.8;
      } else {
        col4 *= 0.7 + 0.3 * SmoothBump (0.05, 0.95, 0.02, fract (8. * qHit.y));
      } 
    } else if (qHit.y > -4.05) {
      col4 = vec4 (0.8, 0.8, 0.8, 0.1);
    } else if (qHit.y < -4.7) {
      col4 = vec4 (0.8, 0., 0., 0.1);
    } else {
      col4 = vec4 (0.3, 0.2, 0.1, 0.);
    }
  } else if (idObj == idRud) {
    col4 = vec4 (0.5, 0.3, 0., 0.);
  } else if (idObj == idStruc) {
    col4 = vec4 (0.4, 0.3, 0.1, 0.1);
    if (max (abs (qHit.x), abs (qHit.z + 0.22)) < 0.2) {
      cg = step (0.1, abs (fract (vec2 (10. * vec2 (qHit.x, qHit.z + 0.22)) + 0.5) - 0.5));
      if (cg.x * cg.y == 1.) col4 = vec4 (0.8, 0.8, 0.2, -1.);
    }
  } else if (idObj == idSailT) {
    qHit.x *= (1. + 0.07 * qHit.y) * (1. + 0.14 * step (1., abs (qHit.z)));
    col4 = vec4 (1., 1., 1., 0.) * (0.8 + 0.2 * SmoothBump (0.08, 0.92, 0.02, fract (4. * qHit.x)));
    if (abs (qHit.z) < 0.2) col4.rgb = mix (cSym, col4.rgb,
       smoothstep (-0.01, 0., MSymCol (0.3 * vec2 (0.25 - qHit.y, qHit.x))));
  } else if (idObj == idSailA) {
    col4 = vec4 (1., 1., 1., 0.) * (0.8 + 0.2 * SmoothBump (0.08, 0.92, 0.02, fract (5. * qHit.z)));
  } else if (idObj == idSailF) {
    col4 = vec4 (1., 1., 1., 0.) * (0.8 + 0.2 * SmoothBump (0.08, 0.92, 0.02,
       fract (2.95 * qHit.y + 4. * qHit.z - 0.5)));
  } else if (idObj == idFlag) {
    col4 = vec4 (1., 1., 0.5, 0.1);
    col4.rgb = mix (cSym, col4.rgb, smoothstep (-0.01, 0., MSymCol (-0.7 * qHit.yz)));
  } else if (idObj == idMast) {
    col4 = vec4 (0.7, 0.4, 0., 0.1);
    if (length (qHit.xz) < 0.16 * (1. - 0.07 * qHit.y))
       col4 *= 0.6 + 0.4 * SmoothBump (0.03, 0.97, 0.01, fract (2. * qHit.y /
       (1. + 0.14 * step (1., abs (qHit.z)))));
    if (qHit.y > 3.65) col4 = vec4 (1., 0., 0., -1.);
  } else if (idObj == idSparT) {
    qHit.x *= (1. + 0.07 * qHit.y) * (1. + 0.14 * step (1., abs (qHit.z)));
    col4 = vec4 (0.7, 0.4, 0., 0.1) *  (0.6 + 0.4 * SmoothBump (0.08, 0.92, 0.01,
       fract (4. * qHit.x)));
  } else if (idObj == idSparL) {
    col4 = vec4 (0.7, 0.4, 0., 0.1);
    if (qHit.z > 6.65) col4 = vec4 (1., 1., 0.3, -1.);
  } else if (idObj == idRig) {
    col4 = vec4 (0.2, 0.15, 0.1, 0.);
  }
  return col4;
}

vec3 CloudCol (vec3 ro, vec3 rd)
{
  vec4 cSum, c;
  vec3 s1, s2;
  float sd, sd2, sd4, sd16, t;
  s1 = 0.003 * (ro + cloudDisp + rd * (300. - ro.y) / rd.y);
  s2 = 0.003 * (rd * 10. / rd.y);
  cSum = vec4 (0.);
  for (float ns = float (VAR_ZERO); ns < 100.; ns ++) {
    c.a = 0.8 * smoothstep (0.5, 1., Fbm3 (s1 + s2 * ns));
    c = vec4 (1., 0.95, 0.9, c.a) - vec4 (0.7, 0.65, 0.7, cSum.a) * c.a;
    cSum += vec4 (c.rgb * c.a, c.a);
    if (cSum.a > 0.99) break;
  }
  cSum.rgb /= cSum.a + 0.001;
  sd = max (dot (rd, sunDir), 0.);
  sd2 = sd * sd;
  sd4 = sd2 * sd2;
  sd16 = sd4 * sd4;
  sd16 *= sd16;
  cSum.rgb += vec3 (0.5) * mix (sd4, - sd16, smoothstep (0.8, 1., cSum.a));
  t = pow (1. - 0.7 * rd.y, 16.);
  return mix (vec3 (0.05, 0.2, 0.5) + t * vec3 (0.45, 0.4, 0.3) +
     0.3 * vec3 (1., 0.9, 0.8) * (sd2 + 4. * pow (sd16, 16.)), cSum.rgb, cSum.a * (1. - t));
}

float CloudDens (vec3 ro, vec3 rd)
{
  vec3 s1, s2;
  float attSum;
  s1 = 0.003 * (ro + cloudDisp + rd * (300. - ro.y) / rd.y);
  s2 = 0.003 * (rd * 10. / rd.y);
  attSum = 0.;
  for (float ns = float (VAR_ZERO); ns < 100.; ns += 10.) {
    attSum += 0.8 * (1. - attSum) * smoothstep (0.5, 1., Fbm3 (s1 + s2 * ns));
    if (attSum > 0.99) break;
  }
  return clamp (1. - attSum, 0., 1.);
}

float GRayDens (vec3 ro, vec3 rd)
{
  vec3 p, s1, s2;
  float d, sd, gDens;
  sd = 0.08 * max (dot (rd, sunDir), 0.);
  gDens = 0.;
  if (sd > 0.01) {
    s1 = ro + 200. * rd;
    s2 = 40. * rd;
    for (float ns = float (VAR_ZERO); ns < 32.; ns ++) {
      p = s1 + s2 * ns;
      gDens += sd * (1. - gDens) * smoothstep (0.8, 1., CloudDens (p, sunDir));
      if (p.y > 400. || gDens > 0.95) break;
    }
  }
  return gDens;
}

vec3 WaveCol (vec3 ro, vec3 rd)
{
  vec3 col;
  float sd, sd2, sd4;
  sd = max (dot (rd, sunDir), 0.);
  sd2 = sd * sd;
  sd4 = sd2 * sd2;
  col = vec3 (0.15, 0.2, 0.2) * (0.7 * ro.y + 1.) +
     CloudDens (ro + 300. * rd, rd) * (mix (vec3 (0., 0.1, 0.2), vec3 (0.2, 0.2, 0.3), rd.y) +
     (1. - rd.y) * vec3 (0.6, 0.6, 0.5) * sd4);
  col = mix (col * (0.7 + 0.3 * sd2), vec3 (0.6, 0.8, 0.9) + vec3 (0.2, 0.2, 0.1) * sd4,
     1. - exp (- pow (0.005 * dstWat, 1.5)));
  return col;
}

vec3 ShowScene (vec3 ro, vec3 rd)
{
  vec4 col4;
  vec3 col, roo, row, rdo, vn, vnw;
  float dstObj, reflFac;
  tWav = 0.5 * tCur;
  cloudDisp = vec3 (0., 0., -20. * tCur);
  SRigConsts ();
  roo = ro;
  rdo = rd;
  reflFac = 1.;
  col = vec3 (0.);
  dstWat = (rd.y < 0.) ? WaveRay (ro, rd) : dstFar;
  for (int k = VAR_ZERO; k < 2; k ++) {
    dstObj = ObjRay (ro, rd);
    if (k == 0 && dstWat < min (dstObj, dstFar)) {
      ro += dstWat * rd;
      row = ro;
      vnw = WaveNf (ro, dstWat).xyz;
      rd = reflect (rd, vnw);
      reflFac = 0.25;
    } else break;
  }
  if (dstObj < min (dstWat, dstFar)) {
    ro += dstObj * rd;
    vn = ObjNf (ro);
    col4 = ShipCol ();
    if (col4.a >= 0.) col = col4.rgb * (0.3 + 
       0.7 * max (dot (vn, vec3 (sunDir * vec3 (-1., 1., -1.))), 0.) +
       0.7 * max (dot (vn, sunDir), 0.)) +
       col4.a * pow (max (dot (sunDir, reflect (rd, vn)), 0.), 32.);
    else col = col4.rgb * (0.5 - 0.5 * dot (vn, rd));
  } else if (dstWat < dstFar) {
    reflFac = 0.;
  } else {
    col = CloudCol (roo, rdo) + 0.9 * vec3 (1., 0.8, 0.6) * GRayDens (roo, rdo);
  }
  if (reflFac < 1.) col = mix (WaveCol (row, reflect (rdo, vnw)), col, reflFac);
  return clamp (col, 0., 1.);
}

void mainImage_ (out vec4 fragColor, in vec2 fragCoord)
{
  mat3 vuMat;
  vec4 mPtr;
  vec3 ro, rd, vd, col, u;
  vec2 canvas, uv;
  float el, az, zmFac, sr, f;
  canvas = iResolution.xy;
  uv = 2. * fragCoord.xy / canvas - 1.;
  uv.x *= canvas.x / canvas.y;
  tCur = iTime;
  mPtr = iMouse;
  mPtr.xy = mPtr.xy / canvas - 0.5;
  bGrid = vec2 (12., 16.);
  grLim = vec2 (1., 1.);
  ro = vec3 (20., 8.6, -40.);
  zmFac = 4.;
  if (mPtr.z > 0.) {
    ro.x += 30. * mPtr.x;
    zmFac += 2. * (mPtr.y + 0.5);
  } else {
    ro.x += 8. * sin (0.015 * pi * tCur);
  }
  vd = normalize (vec3 (6., 3., 0.) - ro);
  u = - vd.y * vd;
  f = 1. / sqrt (1. - vd.y * vd.y);
  vuMat = mat3 (f * vec3 (vd.z, 0., - vd.x), f * vec3 (u.x, 1. + u.y, u.z), vd);
  rd = vuMat * normalize (vec3 (uv, zmFac));
  sunDir = normalize (vec3 (0.5, 0.25, 1.));
  dstFar = 300.;
#if ! AA
  const float naa = 1.;
#else
  const float naa = 3.;
#endif  
  col = vec3 (0.);
  sr = 2. * mod (dot (mod (floor (0.5 * (uv + 1.) * canvas), 2.), vec2 (1.)), 2.) - 1.;
  for (float a = float (VAR_ZERO); a < naa; a ++) {
    rd = vuMat * normalize (vec3 (uv + step (1.5, naa) * Rot2D (vec2 (0.5 / canvas.y, 0.),
       sr * (0.667 * a + 0.5) * pi), zmFac));
    col += (1. / naa) * ShowScene (ro, rd);
  }
  fragColor = vec4 (col, 1.);
}

float PrBoxDf (vec3 p, vec3 b)
{
  vec3 d;
  d = abs (p) - b;
  return min (max (d.x, max (d.y, d.z)), 0.) + length (max (d, 0.));
}

float PrBox2Df (vec2 p, vec2 b)
{
  vec2 d;
  d = abs (p) - b;
  return min (max (d.x, d.y), 0.) + length (max (d, 0.));
}

float PrCylDf (vec3 p, float r, float h)
{
  return max (length (p.xy) - r, abs (p.z) - h);
}

float PrCapsDf (vec3 p, float r, float h)
{
  return length (p - vec3 (0., 0., clamp (p.z, - h, h))) - r;
}

float PrEVCapsDf (vec3 p, vec4 u, float r)
{
  return length (p - clamp (dot (p, u.xyz), 0., u.w) * u.xyz) - r;
}

float PrEECapsDf (vec3 p, vec3 v1, vec3 v2, float r)
{
  return PrEVCapsDf (p - v1, vec4 (normalize (v2 - v1), length (v2 - v1)), r);
}

float Minv2 (vec2 p)
{
  return min (p.x, p.y);
}

float Maxv2 (vec2 p)
{
  return max (p.x, p.y);
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

vec3 HsvToRgb (vec3 c)
{
  return c.z * mix (vec3 (1.), clamp (abs (fract (c.xxx + vec3 (1., 2./3., 1./3.)) * 6. - 3.) - 1.,
     0., 1.), c.y);
}

const float cHashM = 43758.54;

float Hashfv2 (vec2 p)
{
  return fract (sin (dot (p, vec2 (37., 39.))) * cHashM);
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

float Noisefv2 (vec2 p)
{
  vec2 t, ip, fp;
  ip = floor (p);  
  fp = fract (p);
  fp = fp * fp * (3. - 2. * fp);
  t = mix (Hashv2v2 (ip), Hashv2v2 (ip + vec2 (0., 1.)), fp.y);
  return mix (t.x, t.y, fp.x);
}

float Noisefv3 (vec3 p)
{
  vec4 t;
  vec3 ip, fp;
  ip = floor (p);
  fp = fract (p);
  fp *= fp * (3. - 2. * fp);
  t = mix (Hashv4v3 (ip), Hashv4v3 (ip + vec3 (0., 0., 1.)), fp.z);
  return mix (mix (t.x, t.y, fp.x), mix (t.z, t.w, fp.x), fp.y);
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

float Fbm2 (vec2 p)
{
  float f, a;
  f = 0.;
  a = 1.;
  for (int i = 0; i < 5; i ++) {
    f += a * Noisefv2 (p);
    a *= 0.5;
    p *= 2.;
  }
  return f * (1. / 1.9375);
}

float Fbm3 (vec3 p)
{
  float f, a;
  f = 0.;
  a = 1.;
  for (int i = 0; i < 5; i ++) {
    f += a * Noisefv3 (p);
    a *= 0.5;
    p *= 2.;
  }
  return f * (1. / 1.9375);
}
void main()
{
    mainImage_(gl_FragColor, xlv_TEXCOORD0.xy);
}

