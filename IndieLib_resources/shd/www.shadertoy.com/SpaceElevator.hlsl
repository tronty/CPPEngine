/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#if 0
#define WSIGN +
#define ROW_MAJOR
#define MVPSEMANTIC
#define frac frac
#define lerp lerp
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
// https://www.shadertoy.com/view/XlcGRn
// Space Elevator
float     iTime=0;                        // shader playback time (in seconds)
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
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
// "Space Elevator" by dr2 - 2016
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

float Noisefv3 (float3 p);
float Fbm3 (float3 p);float3 VaryNf (float3 p, float3 n, float f);
float SmoothBump (float lo, float hi, float w, float x);
float2 SSBump (float w, float s, float x);
float2 Rot2D (float2 q, float a);
float PrBox2Df (float2 p, float2 b);
float PrSphDf (float3 p, float s);
float PrCylDf (float3 p, float r, float h);
float PrCylAnDf (float3 p, float r, float w, float h);
float PrRCylDf (float3 p, float r, float rt, float h);
float PrTorusDf (float3 p, float ri, float rc);
float PrHexp2Df (float2 p, float r);

int idObj;
float3 qHit, sunDir, wPos, cgPos;
float dstFar, tCur, cbLen, cbRad, cgRad, cgLen, wRad, wGap, wtRad, wcRad;
bool showElev;
int idHub = 1, idSph = 2, idTor = 3, idCbl = 4, idCage = 5;
float pi = 3.14159;
float sqrt3 = 1.73205;

float CageDf (float3 p, float pAng, float dMin)
{
  float3 q;
  float2 qs;
  float d;
  q = p;
  qs = float2 (mod (float2 (16. * pAng, 0.25 * q.z) + 0.5, 1.) - 0.5);
  d = max (PrCylAnDf (p, cgRad, 0.1, cgLen), - PrBox2Df (qs, float2 (0.45, 0.45)));
  if (d < dMin) { dMin = d;  idObj = idCage;  qHit = q; } // ???
  q.z = abs (q.z) - cgLen;
  d = max (max (PrSphDf (q, cgRad + 0.1), cbRad + 0.1 - length (q.xy)), - q.z);
  if (d < dMin) { dMin = d;  idObj = idCage;  qHit = q; }
  return dMin;
}

float CablDf (float3 p, float pAng, float dMin)
{
  float3 q;
  float2 vg, qs;
  float d, wd;
  q = p;
  wd = 1./3.;
  vg = 1.15 * wd * float2 (sqrt3, 1.);
  q.z += mod (0.8 * tCur, 4.);
  qs = float2 ((32. / 3.) * pAng, sqrt3 * q.z);
  d = min (PrHexp2Df (mod (qs + vg, 2. * vg) - vg, wd),
           PrHexp2Df (mod (qs, 2. * vg) - vg, wd));
  d = 0.7 * max (PrCylAnDf (q, cbRad, 0.02 * cbRad, cbLen), - d);
  if (d < dMin) { dMin = d;  idObj = idCbl;  qHit = p; }
  return dMin;
}

float WhlDf (float3 p, float dMin)
{
  float3 q, qq;
  float d, a, s;
  p.xy = Rot2D (p.xy, 0.19 * tCur);
  q = p;
  d = PrCylDf (q, 2. * wcRad, 2. * wGap + wcRad);
  if (d < dMin) { dMin = d;  idObj = idHub;  qHit = q; }
  q.z = abs (q.z) - wGap;
  d = PrRCylDf (q, 4. * wcRad, 2. * wcRad, 1.5 * wtRad);
  if (d < dMin) { dMin = d;  idObj = idHub;  qHit = q; }
  q.z -= wGap + 4. * wcRad;
  d = PrSphDf (q, 4. * wcRad);
  qq = q;
  qq.z -= 4. * wcRad;
  d = max (d, - PrCylDf (qq, 1.5 * wcRad, wcRad));
  if (d < dMin) { dMin = d;  idObj = idSph;  qHit = q; }
  q.z += wGap + 4. * wcRad;
  d = PrTorusDf (q, wtRad, wRad);
  if (d < dMin) { dMin = d;  idObj = idTor;  qHit = q; }
  q = p;
  a = atan (q.y, - q.x) / (2. * pi);
  q.xy = Rot2D (q.xy, (0.5 + floor (7. * a)) * (2. * pi / 7.));
  q.x -= - wRad;
  d = PrCylDf (q, wcRad, wGap);
  if (d < dMin) { dMin = d;  idObj = idHub;  qHit = q; }
  q.z = abs (q.z) - wGap;
  d = PrSphDf (q, 1.3 * wtRad);
  if (d < dMin) { dMin = d;  idObj = idHub;  qHit = q; }
  s = 1. + 0.2 * SmoothBump (0.45, 0.55, 0.02, mod (4. * q.x / wRad + 0.5, 1.));
  q.x -= 0.5 * wRad;
  d = PrCylDf (q.yzx, wcRad * s, 0.5 * wRad);
  if (d < dMin) { dMin = d;  idObj = idHub;  qHit = q; }
  return dMin;
}

float ObjDf (float3 p)
{
  float dMin, pAng;
  pAng = 0.5 * (atan (p.y, - p.x) / pi + 1.);
  dMin = dstFar;
  dMin = WhlDf (p - wPos, dMin);
  if (showElev) {
    dMin = CablDf (p - cgPos, pAng, dMin);
    dMin = CageDf (p - cgPos, pAng, dMin);
  }
  return dMin;
}

float ObjRay (float3 ro, float3 rd)
{
  float dHit, d;
  dHit = 0.;
  for (int j = 0; j < 200; j ++) {
    d = ObjDf (ro + dHit * rd);
    dHit += d;
    if (d < 0.001 || dHit > dstFar) break;
  }
  if (d >= 0.001) dHit = dstFar;
  return dHit;
}

float TransObjDf (float3 p)
{
  float3 q;
  float2 qs;
  float a, d;
  q = p;
  a = 0.5 * (atan (q.y, - q.x) / pi + 1.);
  qs = float2 (mod (float2 (16. * a, 0.25 * q.z) + 0.5, 1.) - 0.5);
  d = max (PrCylAnDf (q, cgRad - 0.05, 0.05, cgLen), PrBox2Df (qs, float2 (0.45)));
  return d;
}

float TransObjRay (float3 ro, float3 rd)
{
  float dHit, d;
  dHit = 0.;
  for (int j = 0; j < 100; j ++) {
    d = TransObjDf (ro + dHit * rd);
    dHit += d;
    if (d < 0.001 || dHit > dstFar) break;
  }
  if (d >= 0.001) dHit = dstFar;
  return dHit;
}

float3 ObjNf (float3 p)
{
  float4 v;
  float3 e = float3 (0.001, -0.001, 0.);
  v = float4 (ObjDf (p + e.xxx), ObjDf (p + e.xyy),
     ObjDf (p + e.yxy), ObjDf (p + e.yyx));
  return normalize (float3 (v.x - v.y - v.z - v.w) + 2. * v.yzw);
}

float ObjSShadow (float3 ro, float3 rd)
{
  float sh, d, h;
  sh = 1.;
  d = 0.1;
  for (int j = 0; j < 30; j ++) {
    h = ObjDf (ro + rd * d);
    sh = min (sh, smoothstep (0., 0.05 * d, h));
    d += max (0.2, 0.1 * d);
    if (sh < 0.05) break;
  }
  return 0.5 + 0.5 * sh;
}

float4 ObjCol ()
{
  float4 col;
  float a, aa, f;
  if (idObj == idTor) {
    col = float4 (0.9, 0.9, 1., 0.3);
    a = (atan (qHit.y, - qHit.x) / (2. * pi));
    aa = atan (qHit.z, length (qHit.xy) - wRad) / (2. * pi);
    f = max (SmoothBump (0.3, 0.7, 0.02, mod (0.5 + 63. * a, 1.)),
       SmoothBump (0.3, 0.7, 0.02, mod (4. * aa, 1.)));
    col = lerp (float4 (0.7, 0.7, 0.2, 1.), col, f);
  } else if (idObj == idSph) {
    col = float4 (0.9, 0.9, 1., 0.3);
    a = (atan (qHit.y, - qHit.x) / (2. * pi));
    aa = atan (qHit.z, length (qHit.xy)) / (2. * pi);
    f = max (SmoothBump (0.3, 0.7, 0.02, mod (0.5 + 14. * a, 1.)),
       1. - SmoothBump (-0.05, 0.05, 0.01, aa));
    col = lerp (float4 (0.7, 0.7, 0.2, 1.), col, f);
    if (length (qHit.xy) < 1.5 * wcRad) col = float4 (0.1, 0.1, 0., -1.);
  } else if (idObj == idCbl) {
    col = float4 (0.9, 1., 0.9, 0.3);
  } else if (idObj == idCage) {
    col = float4 (0.9, 0.9, 1., 0.3);
  } else if (idObj == idHub) {
    col = float4 (0.9, 0.9, 1., 0.3);
  }
  return col;
}

float3 PlCol (float3 rd, float3 vn)
{
  float4 gCol, cCol;
  float3 col, sn, vc;
  float spec, s;
  sn = vn;
  sn.xz = Rot2D (sn.xz, 0.1 * tCur);
  s = min ((0.5 + 0.5 * sn.y * sn.y) *
     Fbm3 (3. * normalize (float3 (sn.xz, 0.8 * sn.y))), 1.);
  col = float3 (0.5, 0.5, 1.);
  col = lerp (col, float3 (0.1, 0.2, 0.1), step (0.6, s));
  col = lerp (col, float3 (0.3, 0.25, 0.1), step (0.7, s));
  col = lerp (col, float3 (0.7), step (0.9, s));
  spec = lerp (0.8, 0.1, step (0.6, s));
  gCol = float4 (col, spec);
  sn = vn;
  sn.xz = Rot2D (sn.xz, 0.12 * tCur);
  vc = normalize (float3 (0.7 * sn.xz, sn.y));
  cCol = float4 (1., 1., 1., 0.2) * (1. - 0.2 * Fbm3 (27. * vc));
  gCol = lerp (gCol, cCol, smoothstep (0.5, 0.9, 0.7 * Fbm3 (7. * vc)));
  if (s > 0.6) vn = VaryNf (20. * sn, vn, 1.);
  col = gCol.rgb * (0.1 +
     0.1 * max (dot (vn, - normalize (float3 (sunDir.x, 0., sunDir.z))), 0.) +
     max (0., max (dot (vn, sunDir), 0.)) + gCol.a *
     pow (max (0., dot (sunDir, reflect (rd, vn))), 16.));
  return col;
}

float3 HvnCol (float3 ro, float3 rd)
{
  float3 rds, col;
  float a, s, plAng;
  plAng = 0.06;
  a = atan (length (rd.xy), - rd.z);
  if (a < plAng) {
    s = sin (a);
    s *= cos (a) / plAng - sqrt (1. - s * s / (plAng * plAng));
    col = PlCol (rd, float3 (s * normalize (rd.xy), sqrt (1. - s * s)));
  } else {
    rds = floor (2000. * rd);
    rds = 0.00015 * rds + 0.1 * Noisefv3 (0.0005 * rds.yzx);
    for (int j = 0; j < 19; j ++) rds = abs (rds) / dot (rds, rds) - 0.9;
    col = float3 (0.8, 0.8, 0.6) *
       min (1., 0.5e-3 * pow (min (6., length (rds)), 5.));
    s = pow (max (0., dot (rd, sunDir)), 16.);
    col = float3 (0., 0., 0.03) + col * (1. - smoothstep (0.9, 1., s));
    s = pow (s, 128.);
    col += float3 (1., 0.95, 0.8) * (0.2 * s + 0.9 * pow (s, 8.));
  }
  return col;
}

float3 ShowScene (float3 ro, float3 rd)
{
  float3 col, rCol, vn, roo, rdo;
  float4 objCol;
  float2 ss;
  float dstHit, dstWin, sh, spec, a;
  int idObjT;
  roo = ro;
  rdo = rd;
  if (showElev) dstWin = TransObjRay (ro, rd);
  dstHit = ObjRay (ro, rd);
  if (dstHit < dstFar) {
    ro += rd * dstHit;
    idObjT = idObj;
    vn = ObjNf (ro);
    idObj = idObjT;
    objCol = ObjCol ();
    if (idObj == idCage) {
      a = mod (16. * atan (qHit.y, - qHit.x) / (2. * pi), 1.) - 0.5;
      if (abs (a) < 0.1) {
        ss = SSBump (0.02, 0.03, a);
        if (ss.x + ss.y != 0.) {
          vn.xy *= 1. + 0.5 * (ss.y - ss.x);
          vn = normalize (vn);
        }
      }
    }
    vn = VaryNf (50. * ro, vn, 0.1);
    rCol = (objCol.a >= 0.) ? HvnCol (ro, reflect (rd, vn)) : float3 (0.);
    sh = ObjSShadow (ro, sunDir);
    col = objCol.rgb * (0.1 +
       0.2 * max (dot (vn, - normalize (float3 (sunDir.x, 0., sunDir.z))), 0.) +
       0.7 * sh * max (0., max (dot (vn, sunDir), 0.)));
    col = sh * lerp (col, rCol, 0.2);
    col += objCol.a * sh * float3 (1., 1., 0.95) *
       pow (max (0., dot (sunDir, reflect (rd, vn))), 16.);
  } else col = HvnCol (ro, rd);
  if (idObj == idCbl && abs (qHit.z) > 0.9 * cbLen)
     col = lerp (col, HvnCol (roo, rdo), smoothstep (0.9, 1., abs (qHit.z) / cbLen));
  if (showElev && dstWin < min (dstHit, dstFar)) {
    vn = float3 (normalize (roo.xy), 0.);
    rCol = float3 (0.05, 0.05, 0.) + HvnCol (roo, reflect (rdo, vn));
    col = lerp (col * float3 (1., 1., 0.5), rCol,
       1. - pow (max (- dot (rdo, vn), 0.), 5.));
  }
  col = pow (clamp (col, 0., 1.), float3 (0.8));
  return col;
}



float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
  float3x3 vuMat;
  float3 ro, rd, vd, u;
  float2 uv;
  float f, t, tm, tCyc;
  uv = 2. * fragCoord.xy / iResolution.xy - 1.;
  uv.x *= iResolution.x / iResolution.y;
  tCur = iTime;
  dstFar = 1000.;
  cbLen = 150.;
  wRad = 20.;
  wGap = wRad / 8.;
  wtRad = wRad / 20.;
  wcRad = wRad / 40.;
  cbRad = 1.;
  cgRad = 4.;
  cgLen = 5.;
  wPos = float3 (0., 0., 400.);
  cgPos = float3 (0.);
  tCyc = 80.;
  showElev = (mod (tCur, 0.5 * tCyc) < 0.25 * tCyc);
  if (showElev) {
    t = 0.05 * mod (tCur, 0.25 * tCyc);
    f = SmoothBump (0.25, 0.75, 0.15, mod (2. * t, 1.)) *
       (2. * step (0.5 * tCyc, mod (tCur, tCyc)) - 1.);
    tm = 2. * step (0.5, mod (t + 0.25, 1.)) - 1.;
    ro = float3 (- (50. + 50. * abs (f)) * tm, 10. * f, 0.);
    ro.xz = Rot2D (ro.xz, 0.5 * pi * f * (2. * step (0.5, t) - 1.));
    ro.z *= tm;
    ro.z += cgPos.z;
    vd = normalize (cgPos - ro);
  } else {
    ro = float3 (150., 0., 0.);
    ro.xz = Rot2D (ro.xz, 0.1 * tCur);
    ro.xy = Rot2D (ro.xy, 0.032 * tCur);
    ro.z += wPos.z;
    vd = normalize (wPos - ro);
  }
  u = - vd.y * vd;
  f = 1. / max (0.001, sqrt (1. - vd.y * vd.y));
  vuMat = float3x3 (f * float3 (vd.z, 0., - vd.x), f * float3 (u.x, 1. + u.y, u.z), vd);
  rd = vuMat * normalize (float3 (uv, 6.));
  sunDir = float3 (1., 0., 0.);
  fragColor = float4 (ShowScene (ro, rd), 2.);
	return fragColor;
}

float PrBox2Df (float2 p, float2 b)
{
  float2 d;
  d = abs (p) - b;
  return min (max (d.x, d.y), 0.) + length (max (d, 0.));
}

float PrSphDf (float3 p, float s)
{
  return length (p) - s;
}

float PrCylDf (float3 p, float r, float h)
{
  return max (length (p.xy) - r, abs (p.z) - h);
}

float PrCylAnDf (float3 p, float r, float w, float h)
{
  return max (abs (length (p.xy) - r) - w, abs (p.z) - h);
}

float PrRCylDf (float3 p, float r, float rt, float h)
{
  float2 dc;
  float dxy, dz;
  dxy = length (p.xy) - r;
  dz = abs (p.z) - h;
  dc = float2 (dxy, dz) + rt;
  return min (min (max (dc.x, dz), max (dc.y, dxy)), length (dc) - rt);
}

float PrTorusDf (float3 p, float ri, float rc)
{
  return length (float2 (length (p.xy) - rc, p.z)) - ri;
}

float PrHexp2Df (float2 p, float r)
{
  float2 pa;
  pa = abs (p);
  return max (0.5 * (pa.x * sqrt3 + pa.y), pa.y) - r;
}

float4 cHashA4 = float4 (0., 1., 57., 58.);
float3 cHashA3 = float3 (1., 57., 113.);
float cHashM = 43758.54;

float4 Hashv4f (float p)
{
  return frac (sin (p + cHashA4) * cHashM);
}

float Noisefv2 (float2 p)
{
  float4 t;
  float2 ip, fp;
  ip = floor (p);
  fp = frac (p);
  fp = fp * fp * (3. - 2. * fp);
  t = Hashv4f (dot (ip, cHashA3.xy));
  return lerp (lerp (t.x, t.y, fp.x), lerp (t.z, t.w, fp.x), fp.y);
}

float Noisefv3 (float3 p)
{
  float4 t1, t2;
  float3 ip, fp;
  float q;
  ip = floor (p);
  fp = frac (p);
  fp = fp * fp * (3. - 2. * fp);
  q = dot (ip, cHashA3);
  t1 = Hashv4f (q);
  t2 = Hashv4f (q + cHashA3.z);
  return lerp (lerp (lerp (t1.x, t1.y, fp.x), lerp (t1.z, t1.w, fp.x), fp.y),
              lerp (lerp (t2.x, t2.y, fp.x), lerp (t2.z, t2.w, fp.x), fp.y), fp.z);
}

float Fbm3 (float3 p)
{
  float f, a;
  f = 0.;
  a = 1.;
  for (int i = 0; i < 5; i ++) {
    f += a * Noisefv3 (p);
    a *= 0.5;
    p *= 2.;
  }
  return f;
}

float Fbmn (float3 p, float3 n)
{
  float3 s;
  float a;
  s = float3 (0.);
  a = 1.;
  for (int i = 0; i < 5; i ++) {
    s += a * float3 (Noisefv2 (p.yz), Noisefv2 (p.zx), Noisefv2 (p.xy));
    a *= 0.5;
    p *= 2.;
  }
  return dot (s, abs (n));
}

float3 VaryNf (float3 p, float3 n, float f)
{
  float3 e = float3 (0.1, 0., 0.);
  float3 g;
  float s;
  s = Fbmn (p, n);
  g = float3 (Fbmn (p + e.xyy, n) - s,
     Fbmn (p + e.yxy, n) - s, Fbmn (p + e.yyx, n) - s);
  return normalize (n + f * (g - n * dot (n, g)));
}

float SmoothBump (float lo, float hi, float w, float x)
{
  return (1. - smoothstep (hi - w, hi + w, x)) * smoothstep (lo - w, lo + w, x);
}

float2 SSBump (float w, float s, float x)
{
  return float2 (step (x + s, w) * step (- w, x + s),
     step (x - s, w) * step (- w, x - s));
}

float2 Rot2D (float2 q, float a)
{
  return q * cos (a) + q.yx * sin (a) * float2 (-1., 1.);
}

