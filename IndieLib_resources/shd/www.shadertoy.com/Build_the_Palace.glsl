// "Build the Palace" by dr2 - 2022
// License: Creative Commons Attribution-NonCommercial-ShareAlike 4.0

/*
 Minimalist assembly instructions for "Rector's Palace" (Xl2yzK).
 Cycle through all pages twice to see everything.
 Mouse overrides paging, view type (inside or outside), orientation (scene or book).
 For monochrome images latest additions are shown in green.
*/

#define AA  1  // (= 0/1) optional antialiasing (recommended)

#if 0
#define VAR_ZERO min (iFrame, 0)
#else
#define VAR_ZERO 0
#endif

float PrBoxDf (vec3 p, vec3 b);
float PrRoundBoxDf (vec3 p, vec3 b, float r);
float PrBox2Df (vec2 p, vec2 b);
float PrBoxAn2Df (vec2 p, vec2 b, float w);
float PrSphDf (vec3 p, float s);
float PrCylDf (vec3 p, float r, float h);
float PrCylAnDf (vec3 p, float r, float w, float h);
float PrTorusDf (vec3 p, float ri, float rc);
vec3 HexGrid (vec2 p);
float Minv2 (vec2 p);
float Maxv2 (vec2 p);
float Maxv3 (vec3 p);
float SmoothMin (float a, float b, float r);
float SmoothMax (float a, float b, float r);
float SmoothBump (float lo, float hi, float w, float x);
vec2 Rot2D (vec2 q, float a);
vec2 Rot2Cs (vec2 q, vec2 cs);
mat3 StdVuMat (float el, float az);
float Hashfv2 (vec2 p);
float Noisefv2 (vec2 p);
float IFbm1 (float p);
float Fbm2 (vec2 p);
vec3 VaryNf (vec3 p, vec3 n, float f);

vec3 ltDirEx, sunDir, pgSize, qHit;
vec2 stCent;
float dstFar, tCur, msAz, msEl, dstFarEx, tRead, tpCyc, tpWait, tpBook, phsTurn, idPage,
   outWd, corWd, flrHt, refSpRad;
int idObj, bState;
const int nPage = 13;
bool isBw, incBg, isWide;
const int idDnFlr = 1, idUpFlr = 2, idRf = 3, idStr = 4, idStWall = 5, idWall = 6, idPil = 7,
   idBallSt = 8, idBallRf = 9, idCen = 10, idRefSph = 11;
const float pi = 3.1415927, sqrt3 = 1.7320508;

#define DMINQ(id) if (d < dMin) { dMin = d;  idObj = id;  qHit = q; }

void SetBldParms ()
{
  outWd = 20.;
  corWd = 2.;
  flrHt = 3.;
  stCent = vec2 (-16., 2.);
  refSpRad = 2.1;
}

float ObjDf (vec3 p)
{
  vec3 q, qq, qqq;
  vec2 wSz;
  float dMin, d, a, r, s;
  dMin = dstFar;
  qq = p;
  a = (length (qq.xz) > 0.) ? atan (qq.z, - qq.x) : 0.;
  qq.xz = Rot2D (qq.xz, 2. * pi * (floor (4. * a / (2. * pi) + 0.5) / 4.));
  if (bState >= 1) {
    q = p;
    d = PrBoxDf (q, vec3 (outWd + 1., 0.2, outWd + 1.));
    DMINQ (idDnFlr);
    q = p;
    q.y -= 0.8;
    d = PrCylAnDf (q.xzy, 2.4, 0.15, 0.7);
    DMINQ (idCen);
  }
  if (bState >= 2) {
    q = qq;
    q.x = abs (abs (q.x) - outWd + corWd) - corWd;
    q.y -= 2. * flrHt + 0.1;
    d = PrBox2Df (q.xy, vec2 (0.1, 2. * flrHt));
    q.yz = mod (q.yz, vec2 (2. * flrHt + 0.2, 2. * corWd)) - vec2 (flrHt + 0.5, corWd);
    wSz = vec2 (1.7, 1.3);
    d = SmoothMax (d, - min (min (length (q.yz) - wSz.x, PrBox2Df (vec2 (q.z, q.y + wSz.y), wSz)),
       PrBoxDf (((qq.x < corWd - outWd) ? vec3 (qq.x + outWd, qq.y - 0.5, abs (qq.z) - 2.) :
       vec3 (qq.x + outWd - 2. * corWd, q.y + flrHt - 0.1, qq.z + 6.)), vec3 (0.25, 0.5, wSz.x))), 0.01);
    if (bState == 2) d = max (d, qq.y - 0.25 * flrHt);
    else if (bState == 3 || bState == 4) d = max (d, qq.y - 2.25 * flrHt);
    DMINQ (idWall);
  }
  if (bState >= 4) {
    q = qq;
    q.x = abs (q.x) - outWd + corWd;
    q.y -= 2. * flrHt + 0.15;
    d = PrBox2Df (q.xy, vec2 (corWd + 0.2, 0.2));
    DMINQ (idUpFlr);
  }
  if (bState >= 6) {
    q = p;
    q.xz = abs (mod (q.xz + corWd, 2. * corWd) - corWd) - 0.1;
    q.y -= 2. * flrHt + 0.1;
    d = max (PrCylDf (q.xzy, 0.07, 2. * flrHt), PrBoxAn2Df (p.xz, vec2 (outWd - 2.), 3.));
    DMINQ (idPil);
  }
  if (bState >= 7) {
    qqq = qq - vec3 (stCent, flrHt + 0.3).xzy;
    q = qqq;
    q.xz = Rot2D (q.xz, -0.73 * pi);
    r = length (q.xz);
    a = (r > 0.) ? atan (q.z, - q.x) : 0.;
    q.xz = vec2 (26. * a / (2. * pi), r - 8.);
    q.xy = Rot2D (q.xy, -0.25 * pi);
    s = mod (q.x, 0.5 * sqrt (2.));
    d = max (0.5 * max (q.y + abs (s - 0.25 * sqrt (2.)) - 0.25 * sqrt (2.),
       max (- q.y - 0.1, abs (q.z) - 1.5)), abs (qqq.y + 0.2) - (flrHt + 0.2));
    DMINQ (idStr);
  }
  if (bState >= 8) {
    d = max (0.5 * max (abs (q.y - 0.3) - 0.4, abs (abs (q.z) - 1.6) - 0.1),
       max (max (- qqq.x, qqq.z), - qqq.y - (flrHt + 0.2)));
    q.x = a;
    DMINQ (idStWall);
    q = qq;
    q.x = abs (q.x + 8.);
    q -= vec3 (1.6, 0.9, 1.95);
    d = PrSphDf (q, 0.15);
    q = qq;
    q.z = abs (q.z + 6.);
    q -= vec3 (-15.95, 7.3, 1.6);
    d = min (d, PrSphDf (q, 0.15));
    DMINQ (idBallSt);
  }
  if (isBw && bState >= 9) {
    q = p;
    q.y -= refSpRad;
    d = PrSphDf (q, refSpRad);
    DMINQ (idRefSph);
  }
  if (bState >= 10) {
    q = qq;
    d = dot (vec2 (qq.y, abs (abs (qq.x) - outWd + corWd)), sin (0.15 * pi + vec2 (0.5 * pi, 0.))) -
       4. * flrHt + (isBw ? 0.01 : 0.02) * sin (4. * pi * qq.z) - 0.2;
    d = max (max (d, abs (qq.x + outWd - 2.) - 3.), 4. * flrHt + 0.1 - qq.y);
    DMINQ (idRf);
    q = p;
    q.xz = abs (q.xz) - (outWd - corWd);
    q.y -= 4. * flrHt + 1.8;
    d = PrSphDf (q, 0.2);
    DMINQ (idBallRf);
  }
  return dMin;
}

float ObjRay (vec3 ro, vec3 rd)
{
  float dHit, d;
  dHit = 0.;
  for (int j = VAR_ZERO; j < 160; j ++) {
    d = ObjDf (ro + dHit * rd);
    dHit += d;
    if (d < 0.0005 || dHit > dstFar) break;
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

vec4 ObjNfL (vec3 p)
{
  vec4 v;
  vec2 e;
  float s;
  e = vec2 (0.001, -0.001);
  for (int j = VAR_ZERO; j < 4; j ++) {
    v[j] = ObjDf (p + ((j < 2) ? ((j == 0) ? e.xxx : e.xyy) : ((j == 2) ? e.yxy : e.yyx)));
  }
  v.x = - v.x;
  s = dot (v, vec4 (1.));
  return vec4 (normalize (2. * v.yzw - s), (s - 2. * v.x - 4. * ObjDf (p)) / (e.x * e.x));
}

float ObjSShadow (vec3 ro, vec3 rd)
{
  vec3 qHitT;
  float sh, d, h;
  int idObjT;
  idObjT = idObj;
  qHitT = qHit;
  sh = 1.;
  d = 0.05;
  for (int j = VAR_ZERO; j < 40; j ++) {
    h = ObjDf (ro + d * rd);
    sh = min (sh, smoothstep (0., 0.05 * d, h));
    d += h;
    if (sh < 0.05) break;
  }
  idObj = idObjT;
  qHit = qHitT;
  return 0.5 + 0.5 * sh;
}

float Truch (vec2 p)
{
  vec2 ip;
  ip = floor (p);
  p -= ip + 0.5;
  if (Hashfv2 (ip) < 0.5) p = vec2 (- p.y, p.x);
  return min (length (0.5 + p), length (0.5 - p));
}

vec3 ShGrid (vec2 p, vec2 g, int stag)
{
  vec2 q, sq, ss;
  q = p * g;
  if (stag > 0 && 2. * floor (0.5 * floor (q.y)) != floor (q.y)) q.x += 0.5;
  sq = smoothstep (0.02, 0.03, abs (fract (q + 0.5) - 0.5));
  q = fract (q) - 0.5;
  ss = 0.07 * pi * smoothstep (0.4, 0.47, abs (q.xy)) * sign (q.xy);
  if (abs (q.x) < abs (q.y)) ss.x = 0.;
  else ss.y = 0.;
  return vec3 (ss.x, 0.8 + 0.2 * sq.x * sq.y, ss.y);
}

vec4 ObjCol (vec3 ro, vec3 rd, inout vec3 vn)
{
  vec4 col4;
  vec3 rg;
  vec2 vf;
  float s, w;
  bool fxz;
  vf = vec2 (0.);
  if (idObj == idWall) {
    col4 = vec4 (0.9, 0.8, 0.7, 0.1);
    if (abs (qHit.x) < 0.095) {
      if (! isBw) {
        col4 *= 0.9;
        vf = vec2 (64., 1.);
      }
    } else if (abs (vn.y) < 0.01) {
      rg = ro;
      rg.y = mod (rg.y, 2. * flrHt + 0.2);
      fxz = (abs (vn.x) > 0.99);
      rg = ShGrid ((fxz ? rg.zy : rg.xy), vec2 (2., 4.), 1);
      col4 *= rg.y;
      if (! isBw) {
        rg.xz *= sign (fxz ? vn.x : vn.z);
        if (fxz) {
          if (rg.x == 0.) vn.xy = Rot2D (vn.xy, rg.z);
          else vn.xz = Rot2D (vn.xz, rg.x);
        } else {
          if (rg.x == 0.) vn.zy = Rot2D (vn.zy, rg.z);
          else vn.zx = Rot2D (vn.zx, rg.x);
        }
        vf = vec2 (32., 1.);
      }
    }
  } else if (idObj == idDnFlr) {
    col4 = vec4 (0.8, 0.75, 0.7, 0.1);
    rg = ShGrid (ro.xz, vec2 (1.), 0);
    s = Maxv2 (abs (ro.xz));
    w = length (ro.xz) - 2.5;
    if (w < 0.) col4.rgb *= vec3 (0., 0.4, 0.6);
    else col4.rgb *= rg.y;
    if (s > outWd) col4.rg *= 0.95;
    if (! isBw) {
      if (vn.y > 0.99) {
        if (rg.x == 0.) vn.yz = Rot2D (vn.yz, rg.z);
        else vn.yx = Rot2D (vn.yx, rg.x);
      }
      if (w > 0. && s < outWd - 2. * corWd) col4.rg *= 0.8 + 0.1 *
         smoothstep (0.3, 0.4, length (fract (ro.xz) - 0.5));
      else if (s < outWd) col4.rgb = mix (col4.rgb, vec3 (0.5, 0.5, 0.8),
         SmoothBump (0.45, 0.55, 0.02, Truch (ro.xz)));
      vf = vec2 (64., 0.5);
    }
 } else if (idObj == idUpFlr) {
    col4 = vec4 (0.75, 0.65, 0.7, 0.1);
    rg = ShGrid (ro.xz, vec2 (1.), 0);
    col4.rgb *= rg.y;
    if (! isBw) {
      if (vn.y > 0.99) {
        if (rg.x == 0.) vn.yz = Rot2D (vn.yz, rg.z);
        else vn.yx = Rot2D (vn.yx, rg.x);
        if (abs (Maxv2 (abs (ro.xz)) - outWd + corWd) < corWd) col4.rgb = mix (col4.rgb,
           vec3 (0.3, 0.7, 0.3), SmoothBump (0.45, 0.55, 0.02, Truch (2. * ro.xz)));
      }
      vf = vec2 (64., 0.5);
    } else {
      col4.rgb *= 1.1;
    }
  } else if (idObj == idRf) {
    col4 = (vn.y > 0.) ? vec4 (0.8, 0.3, 0., 0.1) : vec4 (0.8, 0.5, 0.4, 0.1);
    if (! isBw) col4 *= 0.6 + 0.4 * smoothstep (0.03, 0.05, abs (fract (2. * qHit.x + 0.5) - 0.5));
  } else if (idObj == idStr) {
    col4 = vec4 (0.8, 0.8, 0.9, 0.1);
    col4 *= 0.6 + 0.4 * smoothstep (0.03, 0.05, abs (fract (2. * qHit.z + 0.5) - 0.5));
    if (! isBw) {
      vf = vec2 (64., 0.5);
    }
  } else if (idObj == idStWall) {
    col4 = vec4 (0.8, 0.8, 0.9, 0.1);
    rg = ShGrid (qHit.xy - vec2 (0., 0.1), vec2 (20., 5.), 1);
    col4 *= rg.y;
    if (! isBw) {
      if (abs (vn.y) < 0.01) {
        if (rg.x == 0.) vn = normalize (vec3 (vn.xz, rg.z)).xzy;
        else vn.xz = Rot2D (vn.xz, - rg.x);
      }
      vf = vec2 (32., 1.);
    }
  } else if (idObj == idPil) {
    col4 = vec4 (0.7, 0.7, 0.8, 0.1);
    if (! isBw) {
      s = fract (4. * mod (ro.y, 2. * flrHt + 0.2));
      col4 *= 0.8 + 0.2 * smoothstep (0.03, 0.05, abs (s));
      vn = normalize (vec3 (vn.xz, -0.3 * (1. - smoothstep (0.05, 0.1, abs (s))) * sign (s))).xzy;
      vf = vec2 (32., 1.);
    }
  } else if (idObj == idBallSt || idObj == idBallRf) {
    col4 = vec4 (1., 1., 0.1, 0.3);
  } else if (idObj == idCen) {
    col4 = vec4 (0.8, 0.9, 0.7, 0.1);
    rg = ShGrid (vec2 (atan (ro.z, - ro.x) / pi, ro.y + 0.05), vec2 (16., 4.), 1);
    col4 *= rg.y;
    if (! isBw) {
      if (vn.y < 0.01) {
        if (rg.x == 0.) vn = normalize (vec3 (vn.xz, rg.z)).xzy;
        else vn.xz = Rot2D (vn.xz, - rg.x);
      }
      vf = vec2 (32., 1.);
    }
  } else if (idObj == idRefSph) {
    col4 = vec4 (1., 1., 0.1, 0.3);
  }
  if (! isBw && vf.y > 0.) vn = VaryNf (vf.x * ro, vn, vf.y);
  return col4;
}

vec3 GrndCol (vec3 ro, vec3 rd, float dstGrnd, inout vec3 vn)
{
  vec3 col, qh;
  vec2 vf;
  vn = vec3 (0., 1., 0.);
  vf = vec2 (0.);
  if (length (ro.xz) < 50.) {
    col = vec3 (0.5, 0.5, 0.55);
    qh = HexGrid (1.5 * ro.xz);
    vn.yz = Rot2D (vn.yz, 0.5 * max (length (qh.xy) - 0.3, 0.) *
       (1. - smoothstep (-0.1, -0.05, rd.y)));
    vn.zx = vn.z * vec2 (qh.x, - qh.y) / length (qh.xy);
    vf = vec2 (16., 0.5);
  } else {
    col = vec3 (0.4, 0.4, 0.3) * (0.85 + 0.15 * smoothstep (0.3, 0.7, Fbm2 (8. * ro.xz)));
    vf = vec2 (4., 1.);
  }
  vf.y *= 1. - smoothstep (0.3, 0.6, dstGrnd / dstFar);
  if (vf.y > 0.) vn = VaryNf (vf.x * ro, vn, vf.y);
  return col;
}

vec3 SkyBgCol (vec3 ro, vec3 rd)
{
  vec3 col, clCol, skCol;
  vec2 q;
  float f, fd, ff, sd, a;
  a = atan (rd.z, - rd.x) / (2. * pi) + 0.5;
  if (rd.y > -0.02 && rd.y < 0.02 * mix (IFbm1 (16. * (a + 1.)), IFbm1 (16. * a), a)) {
    col = vec3 (0.3, 0.35, 0.45);
  } else {
    q = 0.02 * (ro.xz + 2. * tCur + ((100. - ro.y) / rd.y) * rd.xz);
    ff = Fbm2 (q);
    f = smoothstep (0.2, 0.8, ff);
    fd = smoothstep (0.2, 0.8, Fbm2 (q + 0.01 * sunDir.xz)) - f;
    clCol = (0.7 + 0.5 * ff) * (vec3 (0.7) - 0.7 * vec3 (0.3, 0.3, 0.2) * sign (fd) *
       smoothstep (0., 0.05, abs (fd)));
    sd = max (dot (rd, sunDir), 0.);
    skCol = vec3 (0.3, 0.4, 0.8) + step (0.1, sd) * vec3 (1., 1., 0.9) *
       min (0.3 * pow (sd, 64.) + 0.5 * pow (sd, 2048.), 1.);
    col = mix (skCol, clCol, 0.1 + 0.9 * f * smoothstep (0.01, 0.1, rd.y));
  }
  return col;
}

vec4 SphHit (vec3 ro, vec3 rd, float rad)
{
  vec3 vn;
  float b, d, w, dMin;
  dMin = dstFar;
  b = dot (rd, ro);
  w = b * b - dot (ro, ro) + rad * rad;
  if (w > 0.) {
    d = - b - sqrt (w);
    if (d > 0.) {
      dMin = d;
      vn = (ro + d * rd) / rad;
    }
  }
  return vec4 (dMin, vn);
}

vec3 ShowScene (vec3 ro, vec3 rd)
{
  vec4 col4, vn4, ds4;
  vec3 col, vn;
  float dstObj, dstSph, dstGrnd, sh;
  bool isNu;
  dstObj = ObjRay (ro, rd);
  dstGrnd = dstFar;
  for (int k = VAR_ZERO; k < 2; k ++) {
    dstObj = ObjRay (ro, rd);
    if (! isBw) {
      if (k == 0) {
        ds4 = SphHit (ro - vec3 (0., refSpRad, 0.), rd, refSpRad);
        dstSph = ds4.x;
        if (dstSph < min (dstObj, dstFar)) {
          ro += dstSph * rd;
          rd = reflect (rd, ds4.yzw);
          ro += 0.01 * rd;
        } else break;
      }
    } else break;
  }
  if (dstObj < dstFar) {
    ro += dstObj * rd;
    if (isBw) {
      vn4 = ObjNfL (ro);
      vn = vn4.xyz;
    } else {
      vn = ObjNf (ro);
    }
  } else if (incBg && rd.y < 0.) {
    dstGrnd = - ro.y / rd.y;
    ro += dstGrnd * rd;
    vn = vec3 (0., 1., 0.);
  }
  sh = (incBg && (min (dstObj, dstGrnd) < dstFar || rd.y < 0.)) ?
     ObjSShadow (ro + 0.01 * vn, sunDir) : 1.;
  isNu = false;
  if (dstObj < dstFar) {
    col4 = ObjCol (ro, rd, vn);
    if (isBw) {
      col = vec3 (1.) * pow (Maxv3 (col4.rgb), 0.7);
      if (bState == 1) isNu = (idObj == idCen);
      if (bState == 2 || bState == 3 && ro.y > 0.85 ||
         bState == 5 && ro.y > 1.05 + 2. * flrHt) isNu = (idObj == idWall);
      if (bState == 4) isNu = (idObj == idUpFlr);
      if (bState == 6) isNu = (idObj == idPil);
      if (bState == 7) isNu = (idObj == idStr);
      if (bState == 8) isNu = (idObj == idStWall || idObj == idBallSt);
      if (bState == 9) isNu = (idObj == idRefSph);
      if (bState == 10) isNu = (idObj == idRf || idObj == idBallRf);
      if (isNu) col.rb *= 0.2;
      col *= (1. - 0.5 * smoothstep (0.5, 1., abs (vn4.w) / 64.)) *
         (0.5 + 0.5 * max (dot (vn, sunDir), 0.));
    } else {
      col = col4.rgb * (0.2 + 0.1 * max (dot (vn, sunDir * vec3 (-1., 1., -1.)), 0.) +
         0.1 * step (sh, 0.95) * abs (dot (vn, sunDir * normalize (vec3 (-1., 0., -1.)))) +
         0.8 * sh * max (dot (vn, sunDir), 0.)) +
         col4.a * step (0.95, sh) * pow (max (dot (sunDir, reflect (rd, vn)), 0.), 32.);
    }
  } else if (incBg && rd.y < 0.) {
    col4.rgb = GrndCol (ro, rd, dstGrnd, vn);
    col = col4.rgb * (0.2 + 0.8 * sh * max (dot (vn, sunDir), 0.));
    col = mix (col, vec3 (0.3, 0.35, 0.45), pow (1. + rd.y, 16.));
  } else {
    col = (incBg) ? SkyBgCol (ro, rd) : vec3 (0.7, 0.7, 0.75);
  }
  col = clamp (col, 0., 1.);
  if (! isBw) col = pow (col, vec3 (0.8));
  return col;
}

void ExMain  (out vec3 fCol, in vec2 fCoord, float asp)
{
  mat3 vuMat;
  vec3 ro, rd;
  float el, az, zmFac;
  isBw = (bState < 11);
  incBg = (bState >= 12);
  if (! isWide) {
    el = -0.15 * pi;
    az = 0.2 * pi;
    ro = vec3 (0., 1.9, -100.);
    zmFac = 3.9;
    fCoord /= zmFac;
  } else {
    el = -0.02 * pi;
    az = 0.;
    ro = vec3 (0., 3., -16.);
    zmFac = 1.5;
    fCoord /= zmFac;
    fCoord.x = 2. * tan (0.5 * atan (fCoord.x / asp)) * asp;
  }
  if (idPage > 0.) {
    az += (msAz > -999.) ? msAz : 0.;
    el += (msEl > -999.) ? msEl : 0.;
    el = clamp (el, -0.4 * pi, -0.02 * pi);
  }
  vuMat = StdVuMat (el, az);
  ro = vuMat * ro;
  dstFar = 200.;
  rd = vuMat * normalize (vec3 (fCoord, 1.));
  sunDir = normalize (vec3 (1., 3., -1.));
  SetBldParms ();
  fCol = ShowScene (ro, rd);
}

float ExObjDf (vec3 p)
{
  vec3 q;
  float dMin, d, rRad, hRad, hOff, np, bc, thFac, nRing, y, s, w;
  dMin = dstFarEx;
  rRad = 0.3;
  hRad = 0.06;
  nRing = 4.;
  hOff = 0.15;
  thFac = 2.2;
  np = float (nPage - 1);
  q = p;
  s = mod (nRing, 2.);
  w = pgSize.x / nRing;
  bc = q.x;
  bc = mod (bc + s * w, 2. * w) - w;
  for (int k = VAR_ZERO; k < nPage; k ++) {
    q = p;
    q.y -= 0.5 * np * thFac * pgSize.y;
    y = q.y + np * thFac * pgSize.y * phsTurn;
    if (k == 0) q.zy = Rot2D (vec2 (q.z, y), 2. * pi * phsTurn);
    else q.y -= thFac * pgSize.y * (phsTurn - float (k));
    d = hRad - length (vec2 (q.z + rRad, bc));
    q.z -= - (pgSize.z - hOff + rRad);
    d = max (PrRoundBoxDf (q, pgSize - pgSize.y, pgSize.y), d);
    DMINQ (k + 1);
  }
  q = p;
  d = abs (q.x) - pgSize.x;
  q.x = bc;
  q = q.zyx;
  d = max (PrTorusDf (q, 0.8 * hRad, rRad + 0.3 * hRad), d);
  DMINQ (nPage + 1);
  return dMin;
}

float ExObjRay (vec3 ro, vec3 rd)
{
  float dHit, d;
  dHit = 0.;
  for (int j = 0; j < 120; j ++) {
    d = ExObjDf (ro + dHit * rd);
    if (d < 0.0005 || dHit > dstFarEx) break;
    dHit += d;
  }
  return dHit;
}

vec3 ExObjNf (vec3 p)
{
  vec4 v;
  vec2 e;
  e = vec2 (0.001, -0.001);
  for (int j = VAR_ZERO; j < 4; j ++) {
    v[j] = ExObjDf (p + ((j < 2) ? ((j == 0) ? e.xxx : e.xyy) : ((j == 2) ? e.yxy : e.yyx)));
  }
  v.x = - v.x;
  return normalize (2. * v.yzw - dot (v, vec4 (1.)));
}

float ExObjSShadow (vec3 ro, vec3 rd)
{
  float sh, d, h;
  sh = 1.;
  d = 0.02;
  for (int j = VAR_ZERO; j < 30; j ++) {
    h = ExObjDf (ro + d * rd);
    sh = min (sh, smoothstep (0., 0.05 * d, h));
    d += h;
    if (sh < 0.05) break;
  }
  return 0.7 + 0.3 * sh;
}

vec3 ExShowScene (vec3 ro, vec3 rd)
{
  vec4 col4;
  vec3 col, c, vn;
  vec2 s, w;
  float dstObj, sh, npCyc, nDotL, t, asp, d;
  bool isImg;
  pgSize = vec3 (3.4, 0.011, 1.7);
  t = (tpBook < 0.) ? (- tpBook * float (nPage - 1) * (tpCyc + tpWait)) :
     mod (tpBook - tpWait, tRead);
  npCyc = mod (floor (t / (tpCyc + tpWait)), float (nPage));
  phsTurn = min (fract (t / (tpCyc + tpWait)) * (tpCyc + tpWait) / tpCyc, 1.);
  isImg = false;
  dstObj = ExObjRay (ro, rd);
  if (dstObj < dstFarEx) {
    ro += dstObj * rd;
    vn = ExObjNf (ro);
    nDotL = max (dot (vn, ltDirEx), 0.);
    if (idObj <= nPage) {
      idPage = mod ((float (idObj - 1) + npCyc), float (nPage));
      c = vec3 (0.85, 0.85, 0.9) * (0.85 + 0.15 * Fbm2 (8. * qHit.xz));
      col4 = vec4 (c, 0.2);
      s = pgSize.xz - abs (qHit.xz);
      if (qHit.y > 0.) {
        w = qHit.xz / pgSize.z;
        if (min (s.x, s.y) > 0.3) {
          isImg = true;
          asp = pgSize.x / pgSize.z;
          if (idPage > 0.) {
            bState = int (idPage);
          } else {
            bState = 12;
            isWide = (qHit.x < 0.);
            w -= sign (qHit.x) * vec2 (0.45 * asp, -0.2);
            if (abs (w.x) > 0.84 || abs (w.y) > 0.37) isImg = false;
            w *= 2.2;
          }
          if (isImg) ExMain (col, w, asp);
          if (idPage == 0.) {
            if (fract (tCur) < 0.8) {
              w -= vec2 (1.7, -0.7);
              d = min (length (max (abs (w) - vec2 (0.1, 0.002), 0.)),
                 length (max (abs (Rot2D (vec2 (w.x - 0.1, abs (w.y)), 0.2 * pi)) -
                 vec2 (0.1, 0.002), 0.)));
              col = mix (vec3 (0., 1., 0.), col, smoothstep (0., 0.01, length (d)));
            }
          }
        } else if (min (s.x, s.y) > 0.27) {
          col4 = vec4 (0.6 * c, 0.2);
        }
      }
    } else if (idObj == nPage + 1) {
      col4 = vec4 (0.95, 0.95, 0.8, 0.2);
      nDotL *= nDotL;
    }
    sh = ExObjSShadow (ro + 0.001 * vn, ltDirEx);
    if (isImg) col *= 0.2 + 0.8 * sh;
    else col = col4.rgb * (0.2 + 0.8 * sh * nDotL) +
       col4.a * step (0.95, sh) * pow (max (dot (normalize (ltDirEx - rd), vn), 0.), 32.);
  } else {
    col = vec3 (0.8);
  }
  return clamp (col, 0., 1.);
}

void main( )
{
  mat3 vuMat;
  vec4 mPtr;
  vec3 ro, rd, col;
  vec2 canvas, uv;
  float el, az, zmFac, sr;
  canvas = iResolution.xy;
  uv = 2. * xlv_TEXCOORD0.xy / canvas - 1.;
  uv.x *= canvas.x / canvas.y;
  tCur = iTime;
  mPtr = iMouse;
  mPtr.xy = mPtr.xy / canvas - 0.5;
  tpBook = max (1.3 * tCur - 0.3, 0.);
  az = 0.;
  el = -0.5 * pi;
  msAz = -999.;
  msEl = -999.;
  tpCyc = 3.;
  tpWait = 4.;
  tRead = float (nPage) * (tpCyc + tpWait);
  isWide = (mod (tpBook - tpWait, 2. * tRead) > tRead);
  if (mPtr.z > 0.) {
    if (abs (mPtr.y) > 0.4) {
      tpBook = - clamp (1.05 * mPtr.x + 0.45, 0., 1.);
      isWide = (mPtr.y > 0.);
    } else if (abs (mPtr.x) > 0.45) {
      el += pi * mPtr.y * smoothstep (0.1, 0.15, abs (mPtr.y));
    } else if (abs (mPtr.x) < 0.42 && abs (mPtr.y + 0.05) < 0.35) {
      msAz = pi * clamp (mPtr.x / 0.4, -1., 1.);
      msEl = pi * mPtr.y;
    }
  }
  vuMat = StdVuMat (el, az);
  ro = vec3 (0., -1.65, -19.);
  zmFac = 9.;
  dstFarEx = 40.;
  ltDirEx = normalize (vec3 (1., 1., -0.3));
  ro = vuMat * ro;
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
    col += (1. / naa) * ExShowScene (ro, rd);
  }
  gl_FragColor = vec4 (col, 1.);
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

float PrRoundBoxDf (vec3 p, vec3 b, float r)
{
  return length (max (abs (p) - b, 0.)) - r;
}

float PrBoxAn2Df (vec2 p, vec2 b, float w)
{
  return max (PrBox2Df (p, vec2 (b + w)), - PrBox2Df (p, vec2 (b - w)));
}

float PrSphDf (vec3 p, float s)
{
  return length (p) - s;
}

float PrCylDf (vec3 p, float r, float h)
{
  return max (length (p.xy) - r, abs (p.z) - h);
}

float PrCylAnDf (vec3 p, float r, float w, float h)
{
  return max (abs (length (p.xy) - r) - w, abs (p.z) - h);
}

float PrTorusDf (vec3 p, float ri, float rc)
{
  return length (vec2 (length (p.xy) - rc, p.z)) - ri;
}

vec2 PixToHex (vec2 p)
{
  vec3 c, r, dr;
  c.xz = vec2 ((1./sqrt3) * p.x - (1./3.) * p.y, (2./3.) * p.y);
  c.y = - c.x - c.z;
  r = floor (c + 0.5);
  dr = abs (r - c);
  r -= step (dr.yzx, dr) * step (dr.zxy, dr) * dot (r, vec3 (1.));
  return r.xz;
}

vec2 HexToPix (vec2 h)
{
  return vec2 (sqrt3 * (h.x + 0.5 * h.y), (3./2.) * h.y);
}

vec3 HexGrid (vec2 p)
{
  vec2 q;
  p -= HexToPix (PixToHex (p));
  q = abs (p);
  return vec3 (p, (sqrt3/2.) - q.x + 0.5 * min (q.x - sqrt3 * q.y, 0.));
}

float Minv2 (vec2 p)
{
  return min (p.x, p.y);
}

float Maxv2 (vec2 p)
{
  return max (p.x, p.y);
}

float Maxv3 (vec3 p)
{
  return max (p.x, max (p.y, p.z));
}

float SmoothMin (float a, float b, float r)
{
  float h;
  h = clamp (0.5 + 0.5 * (b - a) / r, 0., 1.);
  return mix (b - h * r, a, h);
}

float SmoothMax (float a, float b, float r)
{
  return - SmoothMin (- a, - b, r);
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

mat3 StdVuMat (float el, float az)
{
  vec2 ori, ca, sa;
  ori = vec2 (el, az);
  ca = cos (ori);
  sa = sin (ori);
  return mat3 (ca.y, 0., - sa.y, 0., 1., 0., sa.y, 0., ca.y) *
         mat3 (1., 0., 0., 0., ca.x, - sa.x, 0., sa.x, ca.x);
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

float IFbm1 (float p)
{
  float f, a;
  f = 0.;
  a = 10.;
  for (int j = 0; j < 3; j ++) {
    f += floor (a * Noiseff (p));
    a *= 0.5;
    p *= 2.;
  }
  return f * (1. / (1.75 * a));
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

float Fbmn (vec3 p, vec3 n)
{
  vec3 s;
  float a;
  s = vec3 (0.);
  a = 1.;
  for (int i = 0; i < 5; i ++) {
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
