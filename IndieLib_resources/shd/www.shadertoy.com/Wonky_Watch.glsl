// "Wonky Watch" by dr2 - 2023
// License: Creative Commons Attribution-NonCommercial-ShareAlike 4.0

// Pocket watch with inexact timekeeping (up to +/- 10m) - only correct
// twice a day, just like Lewis Carroll's stopped clock, or when mouse pressed.

#define AA  1   // optional antialiasing

#define VAR_ZERO min (nFrame, 0)

float PrRoundBoxDf (vec3 p, vec3 b, float r);
float PrSphDf (vec3 p, float r);
float PrCylDf (vec3 p, float r, float h);
float PrTorusDf (vec3 p, float ri, float rc);
float PrTorusBxDf (vec3 p, vec3 b, float ri);
float SmoothBump (float lo, float hi, float w, float x);
mat3 StdVuMat (float el, float az);
vec2 Rot2D (vec2 q, float a);
vec2 Rot2Cs (vec2 q, vec2 cs);
float Minv2 (vec2 p);
float Maxv2 (vec2 p);
float ShowInt (vec2 q, vec2 cBox, float mxChar, float val);
float Fbm2 (vec2 p);

vec4 dateCur;
vec3 ltDir, qHit;
vec2 aaCs[3], wRotCs;
float dstFar, tCur, tShift;
int nFrame, idObj;
const int idFace = 1, idHandS = 2, idHand = 3, idAx = 4, idHub = 5,
   idCase = 6, idWnd = 7, idClsp = 8;
const float pi = 3.1415927;

#define CosSin(x) (sin ((x) + vec2 (0.5 * pi, 0.)))
#define DMINQ(id) if (d < dMin) { dMin = d;  idObj = id;  qHit = q; }

float ObjDf (vec3 p)
{
  vec3 q;
  float dMin, d, r, a;
  dMin = dstFar;
  p.xz = Rot2Cs (p.xz, wRotCs);
  p.yz = p.zy * vec2 (-1., 1.);
  q = p;
  r = length (q.xz);
  a = (r > 0.) ? atan (- q.z, q.x) / (2. * pi) + 0.5 : 0.;
  d = 0.9 * max (length (vec2 (r - 1.05, q.y)) - 0.08 - 0.003 * abs (sin (256. * pi * a)), 1.01 - r);
  DMINQ (idCase);
  d = PrCylDf (q.xzy, 1.05, 0.05);
  d = max (d, - max (length (vec2 (q.xz - vec2 (0., -0.5))) - 0.252, 0.03 - q.y));
  d = min (d, PrTorusBxDf ((q - vec3 (0.7, 0.05, 0.)).xzy, vec3 (0.06, 0.06, 0.01), 0.007));
  DMINQ (idFace);
  q = p;
  q.z -= 1.16;
  d = PrCylDf (q, 0.03, 0.06);
  DMINQ (idClsp);
  q.z -= 0.14;
  d = PrSphDf (q, 0.1);
  DMINQ (idWnd);
  d = PrTorusDf (q.xzy, 0.015, 0.13);
  DMINQ (idClsp);
  q = p;
  q.y -= 0.06;
  d = PrCylDf (q.xzy, 0.02, 0.04);
  DMINQ (idAx);
  q = p;
  q.yz -= vec2 (0.04, -0.5);
  d = PrCylDf (q.xzy, 0.015, 0.02);
  DMINQ (idAx);
  q = p;
  q.yz -= vec2 (0.045, -0.5);
  d = PrCylDf (q.xzy, 0.03, 0.008);
  DMINQ (idHub);
  q.xz = Rot2Cs (q.xz, aaCs[0]);
  q.z -= -0.1;
  d = PrRoundBoxDf (q, vec3 (0.008, 0., 0.1), 0.005);
  DMINQ (idHandS);
  q = p;
  q.y -= 0.07;
  d = PrCylDf (q.xzy, 0.04, 0.007);
  DMINQ (idHub);
  q.xz = Rot2Cs (q.xz, aaCs[1]);
  q.z -= -0.4;
  d = PrRoundBoxDf (q, vec3 (0.015, 0., 0.4), 0.005);
  DMINQ (idHand);
  q = p;
  q.y -= 0.09;
  d = PrCylDf (q.xzy, 0.04, 0.007);
  DMINQ (idHub);
  q.xz = Rot2Cs (q.xz, aaCs[2]);
  q.z -= -0.3;
  d = PrRoundBoxDf (q, vec3 (0.01, 0., 0.3), 0.005);
  DMINQ (idHand);
  return dMin;
}

float ObjRay (vec3 ro, vec3 rd)
{
  float dHit, d;
  dHit = 0.;
  for (int j = VAR_ZERO; j < 120; j ++) {
    d = ObjDf (ro + dHit * rd);
    if (d < 0.0005 || dHit > dstFar) break;
    dHit += d;
  }
  return dHit;
}

vec3 ObjNf (vec3 p)
{
  vec4 v;
  vec2 e;
  e = vec2 (0.001, -0.001);
  for (int j = VAR_ZERO; j < 4; j ++) {
    v[j] = ObjDf (p + ((j < 2) ? ((j == 0) ? e.xxx : e.xyy) : ((j == 2) ? e.yxy : e.yyx)));
  }
  v.x = - v.x;
  return normalize (2. * v.yzw - dot (v, vec4 (1.)));
}

float TrObjDf (vec3 p)
{
  vec3 q;
  float d;
  p.xz = Rot2Cs (p.xz, wRotCs);
  p.yz = p.zy * vec2 (-1., 1.);
  q = p;
  d = max (PrSphDf (q - vec3 (0., -7.35, 0.), 7.5), max (length (q.xz) - 1.05, - q.y));
  return d;
}

float TrObjRay (vec3 ro, vec3 rd)
{
  float dHit, d;
  dHit = 0.;
  for (int j = VAR_ZERO; j < 60; j ++) {
    d = TrObjDf (ro + dHit * rd);
    if (d < 0.0005 || dHit > dstFar) break;
    dHit += d;
  }
  return dHit;
}

vec3 TrObjNf (vec3 p)
{
  vec4 v;
  vec2 e;
  e = vec2 (0.001, -0.001);
  for (int j = VAR_ZERO; j < 4; j ++) {
    v[j] = TrObjDf (p + ((j < 2) ? ((j == 0) ? e.xxx : e.xyy) : ((j == 2) ? e.yxy : e.yyx)));
  }
  v.x = - v.x;
  return normalize (2. * v.yzw - dot (v, vec4 (1.)));
}

float ObjSShadow (vec3 ro, vec3 rd)
{
  float sh, d, h;
  sh = 1.;
  d = 0.01;
  for (int j = VAR_ZERO; j < 30; j ++) {
    h = ObjDf (ro + d * rd);
    sh = min (sh, smoothstep (0., 0.1 * d, h));
    d += h;
    if (sh < 0.05) break;
  }
  return 0.6 + 0.4 * sh;
}

vec3 BgCol (vec3 rd)
{
  vec3 col, skCol;
  vec2 u;
  float el, f;
  skCol = vec3 (0.1, 0.15, 0.6);
  el = asin (rd.y);
  rd.xz = Rot2D (rd.xz, 0.01 * tCur);
  u = vec2 (atan (rd.z, - rd.x) + pi, tan (2. * atan (0.5 * el))) / (2. * pi);
  f = 64.;
  col = mix (skCol, vec3 (0.8, 0.8, 0.7), mix (Fbm2 (f * u),
     Fbm2 (f * (u - vec2 (1., 0.))), u.x));
  col = mix (col, skCol, smoothstep (0.95, 0.98, abs (el) / (0.5 * pi)));
  return col;
}

float LabSym (vec2 p)
{  // (from "Booze Cruise")
  vec2 q;
  float d, r;
  r = length (p);
  d = max (min (0.06 - abs (0.1 - abs (r - 0.8)), p.y), min (0.06 - abs (p.y), 1.1 - abs (p.x)));
  q = Rot2D (p, 2. * pi * floor (16. * ((r > 0.) ? atan (p.y, - p.x) / (2. * pi) : 0.) + 0.5) / 16.);
  d = max (d, min (min (0.06 - abs (q.y), 0.2 - abs (q.x + 1.1)), p.y + 0.1));
  q.x += 1.5;
  d = max (d, min (0.1 - length (q), p.y + 0.1));
  return d;
}

vec3 DialCol (vec2 p)
{
  vec3 col, c;
  vec2 q;
  float a, f, s;
  col = vec3 (1.);
  c = vec3 (0.3);
  q = p;
  s = length (q);
  a = atan (q.x, q.y) / pi;
  if (abs (fract (6. * a + 0.5) - 0.5) < 0.03 && abs (s - 0.95) < 0.05 ||
     abs (fract (30. * a + 0.5) - 0.5) < 0.1 && abs (s - 0.97) < 0.03 ||
     abs (6. * a) < 0.03 && abs (s - 0.92) < 0.08) col = c;
  col = mix (col, c, SmoothBump (0.98, 1., 0.005, s));
  q = (p - vec2 (0., -0.5)) / 0.25;
  s = length (q);
  a = atan (q.x, q.y) / pi;
  if (abs (fract (6. * a + 0.5) - 0.5) < 0.03 && abs (s - 0.93) < 0.07 ||
     abs (6. * a) < 0.03 && abs (s - 0.85) < 0.15) col = c;
  col = mix (col, c, SmoothBump (0.96, 1., 0.01, s));
  if (Maxv2 (abs (p - vec2 (0.7, 0.))) < 0.065) {
    col = vec3 (0.5);
    if (ShowInt (p - vec2 (0.755, -0.045), 0.3 * vec2 (0.4, 0.3), 2., dateCur.z) != 0.)
       col = vec3 (0.7, 1., 1.);
  }
  s = LabSym (8. * (p - vec2 (0., 0.4)));
  col = mix (mix (col, vec3 (0., 0., 0.8), smoothstep (-0.05, -0.01, s)),
     vec3 (1., 1., 0.2), smoothstep (0.01, 0.05, s));
  return col;
}

vec3 ShowScene (vec3 ro, vec3 rd)
{
  vec4 col4;
  vec3 col, vn, roo;
  float dstObj, dstTrObj, gRot, sh, rFac, nDotL;
  wRotCs = CosSin (0.2 * pi * sin (0.1 * pi * tCur));
  gRot = (dateCur.w + tShift) * 720. / (12. * 3600.);
  aaCs[0] = CosSin (2. * pi * (floor (60. * gRot) + smoothstep (0.7, 1., fract (60. * gRot))) / 60. - pi);
  gRot /= 60.;
  aaCs[1] = CosSin (2. * pi * gRot - pi);
  gRot /= 12.;
  aaCs[2] = CosSin (2. * pi * gRot - pi);
  roo = ro;
  dstObj = ObjRay (ro, rd);
  if (dstObj < dstFar) {
    ro += dstObj * rd;
    vn = ObjNf (ro);
    rFac = 0.;
    if (idObj == idFace) {
      if (qHit.y > 0.) {
        col4 = vec4 (DialCol (qHit.xz), 0.);
      } else {
        col4 = vec4 (0.7, 0.6, 0.1, 0.2);
        col4.rgb *= 1. - 0.3 * smoothstep (-0.02, 0., LabSym (8. * qHit.xz));
        rFac = 0.2;
      }
    } else if (idObj == idHandS) {
      col4 = (tShift == 0.) ? vec4 (0., 1., 0., 0.2) : vec4 (1., 0., 0., 0.2);
    } else if (idObj == idHand) {
      col4 = (abs (qHit.x) < 0.007 && qHit.y > 0.) ? vec4 (0.3, 1., 0.3, 0.2) :
         vec4 (0.3, 0.3, 0.3, 0.);
    } else if (idObj == idAx) {
      col4 = vec4 (0.6, 0.6, 0.5, 0.2);
    } else if (idObj == idHub) {
      col4 = vec4 (0.3, 0.3, 0.3, 0.);
    } else if (idObj == idCase) {
      col4 = vec4 (0.7, 0.6, 0.1, 0.2);
      rFac = 0.2;
    } else if (idObj == idWnd) {
      col4 = vec4 (0.7, 0.6, 0.1, 0.2);
      if (abs (qHit.z) < 0.08) col4 *= 0.5 +
         0.5 * smoothstep (0.4, 0.5, fract (16. * (atan (qHit.x, qHit.y) / (2. * pi) + 0.5)));
      rFac = 0.2;
    } else if (idObj == idClsp) {
      col4 = vec4 (0.7, 0.7, 0.3, 0.2);
      rFac = 0.5;
    }
    nDotL = max (dot (vn, ltDir), 0.);
    if (idObj == idCase || idObj == idClsp || idObj == idWnd) nDotL *= nDotL;
    sh = ObjSShadow (ro + 0.01 * vn, ltDir);
    col = col4.rgb * (0.3 + 0.2 * max (dot (vn, ltDir * vec3 (-1., 1., -1.)), 0.) +
       0.7 * sh * nDotL) + col4.a * pow (max (dot (ltDir, reflect (rd, vn)), 0.), 32.);
    col = mix (col, 0.8 * BgCol (reflect (rd, vn)), rFac);
  } else {
    col = BgCol (rd);
  }
  ro = roo;
  dstTrObj = TrObjRay (ro, rd);
  if (dstTrObj < min (dstObj, dstFar)) {
    ro += dstTrObj * rd;
    vn = TrObjNf (ro);
    col = mix (col, 0.8 * BgCol (reflect (rd, vn)), 0.03 + 0.9 * pow (1. - abs (dot (vn, rd)), 4.));
  }
  return clamp (col, 0., 1.);
}

void main()
{
  mat3 vuMat;
  vec4 mPtr;
  vec3 ro, rd, col;
  vec2 canvas, uv;
  float el, az, zmFac, sr;
  nFrame = iFrame;
  canvas = iResolution.xy;
  uv = 2. * xlv_TEXCOORD0.xy / canvas - 1.;
  uv.x *= canvas.x / canvas.y;
  tCur = iTime;
  dateCur = iDate;
  mPtr = iMouse;
  mPtr.xy = mPtr.xy / canvas - 0.5;
  az = 0.;
  el = 0.;
  if (mPtr.z > 0.) {
    az += 2. * pi * mPtr.x;
    el += pi * mPtr.y;
  }
  el = clamp (el, -0.3 * pi, 0.3 * pi);
  tShift = (mPtr.z > 0.) ? 0. : 600. * sin (2. * pi * dateCur.w / (24. * 3600.));
  vuMat = StdVuMat (el, az);
  ro = vuMat * vec3 (0., 0.2, -8.);
  zmFac = 5.5;
  dstFar = 100.;
  ltDir = normalize (vec3 (0.3, 1.5, -1.));
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
  gl_FragColor = vec4 (col, 1.);
}

float PrRoundBoxDf (vec3 p, vec3 b, float r)
{
  return length (max (abs (p) - b, 0.)) - r;
}

float PrSphDf (vec3 p, float r)
{
  return length (p) - r;
}

float PrCylDf (vec3 p, float r, float h)
{
  return max (length (p.xy) - r, abs (p.z) - h);
}

float PrTorusDf (vec3 p, float ri, float rc)
{
  return length (vec2 (length (p.xy) - rc, p.z)) - ri;
}

float PrTorusBxDf (vec3 p, vec3 b, float ri)
{
  return length (vec2 (length (max (abs (p.xy) - b.xy, 0.)) - b.z, p.z)) - ri;
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

mat3 StdVuMat (float el, float az)
{
  vec2 ori, ca, sa;
  ori = vec2 (el, az);
  ca = cos (ori);
  sa = sin (ori);
  return mat3 (ca.y, 0., - sa.y, 0., 1., 0., sa.y, 0., ca.y) *
         mat3 (1., 0., 0., 0., ca.x, - sa.x, 0., sa.x, ca.x);
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

float DigSeg (vec2 q)
{
  q = 1. - smoothstep (vec2 (0.), vec2 (0.04, 0.07), abs (q) - vec2 (0.13, 0.5));
  return q.x * q.y;
}

#define DSG(q) k = kk;  kk = k / 2;  if (kk * 2 != k) d += DigSeg (q)

float ShowDig (vec2 q, int iv)
{
  vec2 vp, vm, vo;
  float d;
  int k, kk;
  vp = vec2 (0.5, 0.5);
  vm = vec2 (-0.5, 0.5);
  vo = vp - vm;
  if (iv == -1) k = 8;
  else if (iv < 2) k = (iv == 0) ? 119 : 36;
  else if (iv < 4) k = (iv == 2) ? 93 : 109;
  else if (iv < 6) k = (iv == 4) ? 46 : 107;
  else if (iv < 8) k = (iv == 6) ? 122 : 37;
  else             k = (iv == 8) ? 127 : 47;
  q = (q - 0.5) * vec2 (1.8, 2.3);
  d = 0.;
  kk = k;
  DSG (q.yx - vo);  DSG (q.xy - vp);  DSG (q.xy - vm);  DSG (q.yx);
  DSG (q.xy + vm);  DSG (q.xy + vp);  DSG (q.yx + vo);
  return d;
}

float ShowInt (vec2 q, vec2 cBox, float mxChar, float val)
{
  float nDig, idChar, s, sgn, v;
  q = vec2 (- q.x, q.y) / cBox;
  s = 0.;
  if (Minv2 (q) >= 0. && Maxv2 (q) < 1.) {
    q.x *= mxChar;
    sgn = sign (val);
    val = abs (val);
    nDig = (val > 0.) ? floor (max (log2 (val) / log2 (10.), 0.) + 0.0001) + 1. : 1.;
    idChar = mxChar - 1. - floor (q.x);
    q.x = fract (q.x);
    v = val / pow (10., mxChar - idChar - 1.);
    if (idChar == mxChar - nDig - 1. && sgn < 0.) s = ShowDig (q, -1);
    if (idChar >= mxChar - nDig) s = ShowDig (q, int (mod (floor (v), 10.)));
  }
  return s;
}

const float cHashM = 43758.54;

vec2 Hashv2v2 (vec2 p)
{
  vec2 cHashVA2 = vec2 (37., 39.);
  return fract (sin (dot (p, cHashVA2) + vec2 (0., cHashVA2.x)) * cHashM);
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

