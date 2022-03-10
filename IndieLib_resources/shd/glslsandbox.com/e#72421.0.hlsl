/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
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

// Forked from: https://www.shadertoy.com/view/NsS3Dt

float2 mouse=float2(1, 1);
float2 resolution=float2(1, 1);
float time=0.0;

float vec_smootherstep(float edge0, float edge1, float x) {
  float t = clamp(((x - edge0) / (edge1 - edge0)), 0.0, 1.0);
  return (((t * t) * t) * ((t * ((t * 6.0) - 15.0)) + 10.0));
}
float2 sincos(float x) {
  return float2(sin(x), cos(x));
}
float noise_hash1_2(float2 v) {
  float3 v3 = v.xyx;
  v3 = frac((v3 * 0.1031));
  v3 = (v3 + dot(v3, (v3.yzx + 33.33)));
  return frac(((v3.x + v3.y) * v3.z));
}
float2 noise_hash2_1(float2 v) {
  float3 v3 = v.xyx;
  v3 = (v3 * float3(0.1031, 0.103, 0.0973));
  v3 = (v3 + dot(v3, (v3.yzx + 33.33)));
  return frac(((v3.xx + v3.yz) * v3.zy));
}
float noise_noisemix2(float a, float b, float c, float d, float2 f) {
  float2 u = ((f * f) * (3.0 - (2.0 * f)));
  return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}
float noise_value_1(float2 p) {
  float2 i = floor(p);
  float2 f = frac(p);
  float2 I = floor((i + 1.0));
  float a = noise_hash1_2(i);
  float b = noise_hash1_2(float2(I.x, i.y));
  float c = noise_hash1_2(float2(i.x, I.y));
  float d = noise_hash1_2(I);
  return noise_noisemix2(a, b, c, d, f);
}
float noise_gradient_1(float2 p) {
  float2 i = floor(p);
  float2 f = frac(p);
  float2 I = floor((i + 1.0));
  float2 F = (f - 1.0);
  float a = dot((-0.5 + noise_hash2_1(i)), f);
  float b = dot((-0.5 + noise_hash2_1(float2(I.x, i.y))), float2(F.x, f.y));
  float c = dot((-0.5 + noise_hash2_1(float2(i.x, I.y))), float2(f.x, F.y));
  float d = dot((-0.5 + noise_hash2_1(I)), F);
  return (0.5 + noise_noisemix2(a, b, c, d, f));
}
float3 colorgrade_tonemap_aces(float3 col) {
  return clamp(((col * ((2.51 * col) + 0.03)) / ((col * ((2.43 * col) + 0.59)) + 0.14)), 0.0, 1.0);
}
float3 colorgrade_saturate(float3 col, float sat) {
  float grey = dot(col, float3(0.2125, 0.7154, 0.0721));
  return (grey + (sat * (col - grey)));
}
float3 colorgrade_tone_1(float3 col, float3 gain, float3 lift, float3 invgamma) {
  col = pow(col, invgamma);
  return (((gain - lift) * col) + lift);
}
float3 colorgrade_gamma_correction(float3 col) {
  return ((1.12661 * sqrt(col)) - (0.12661 * col));
}
float3 colorgrade_vignette(float3 col, float2 coord, float strength, float amount) {
  return (col * ((1.0 - amount) + (amount * pow(((((16.0 * coord.x) * coord.y) * (1.0 - coord.x)) * (1.0 - coord.y)), strength))));
}
float3 colorgrade_dither(float3 col, float2 coord, float amount) {
  return clamp((col + (noise_hash1_2(coord) * amount)), 0.0, 1.0);
}
float3 camera_perspective(float3 lookfrom, float3 lookat, float tilt, float vfov, float2 uv) {
  float2 sc = sincos(tilt);
  float3 vup = float3(sc.x, sc.y, 0.0);
  float3 w = normalize((lookat - lookfrom));
  float3 u = normalize(cross(w, vup));
  float3 v = cross(u, w);
  float wf = (1.0 / tan((vfov * 0.00872664626)));
  return normalize((((uv.x * u) + (uv.y * v)) + (wf * w)));
}
float demos_redlandscape_fbm_terrain(float2 p) {
  float a = 1.0;
  float t = 0.0;
  for(int i = 0; i < 4; i++) {
    t = (t + (a * noise_value_1(p)));
    a = (a * 0.5);
    p = (2.0 * p);
  }
  return t;
}
float demos_redlandscape_map(float3 p) {
  float2 q = p.xz;
  float h = (demos_redlandscape_fbm_terrain(q) * 0.5);
  float d = ((p.y + (h * 0.75)) + 0.0);
  return (d * 0.5);
}
float demos_redlandscape_ray_march(float3 ro, float3 rd) {
  float t = 0.0;
  for(int i = 0; i < 256; i++) {
    float3 p = (ro + (t * rd));
    float d = demos_redlandscape_map(p);
    if((d < (0.003 * t)) || (t >= 25.0)) {
      break;
    }
    t += d;
  }
  return t;
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
  float2 uv = (fragCoord.xy / resolution);
  float2 coord = ((2.0 * (fragCoord.xy - (resolution * 0.5))) / resolution.y);
  float z = (time * 1.0);
  float2 sc = sincos((time * 0.5));
  float y = 0.0;
  float3 lookat = float3((sc.x * 0.5), y, z);
  float3 ro = float3(((-sc.x) * 0.5), y, (z - 2.0));
  float3 rd = camera_perspective(ro, lookat, 0.0, 45.0, coord);
  float3 col = float3(0,0,0);
  float3 sun_dir = normalize(float3(0.3, 0.07, 1.0));
  float3 hor_col = float3(0.7, 0.05, 0.01);
  float3 sun_col = float3(0.9, 0.8, 0.7);
  float3 bou_col = float3(0.8, 0.3, 0.1);
  float t = demos_redlandscape_ray_march(ro, rd);
  float3 p = (ro + (rd * t));
  float3 back_col = float3(0,0,0);
  {
    back_col = lerp(hor_col, (hor_col * 0.3), vec_smootherstep(0.0, 0.25, rd.y));
    back_col = lerp(back_col, bou_col, max((0.1 - rd.y), 0.0));
    float sun_lightness = max(dot(rd, sun_dir), 0.0);
    back_col = (back_col + (sun_col * pow(sun_lightness, 2000.0)));
    back_col = (back_col + ((0.3 * sun_col) * pow(sun_lightness, 100.0)));
    back_col = (back_col + (float3(0.3, 0.2, 0.1) * pow(sun_lightness, 4.0)));
  }
  if(abs(coord.y) > 0.75) {
    col = float3(0,0,0);
  } else if(t < 25.0) {
    float decay = (1.0 - exp((-0.12 * t)));
    col = lerp(col, back_col, decay);
  } else {
    col = back_col;
    float clouds_altitude = 1000.0;
    float clouds_dist = ((1.0 - (ro.y / clouds_altitude)) / rd.y);
    if(clouds_dist > 0.0) {
      float clouds_zoom = 1.0;
      float2 clouds_pos = (ro.xz + ((rd.xz * clouds_dist) * clouds_zoom));
      float clouds_lightness = max((noise_gradient_1(clouds_pos) - 0.3), 0.0);
      float clouds_decay = vec_smootherstep(0.0, 0.3, rd.y);
      float3 clouds_col = (2.0 * col);
      col = lerp(col, clouds_col, (clouds_lightness * clouds_decay));
    }
    col = clamp(col, 0.0, 1.0);
  }
  col = colorgrade_tonemap_aces(col);
  col = colorgrade_gamma_correction(col);
  col = colorgrade_tone_1(col, float3(1.3, 0.9, 0.7), (float3(0.5, 0.1, 0.1) * 0.1), float3(3.0, 2.0, 1.2));
  col = colorgrade_saturate(col, 0.7);
  col = colorgrade_vignette(col, uv, 0.25, 0.7);
  col = colorgrade_dither(col, fragCoord.xy, 0.01);
  fragColor = float4(col, 1.0);
	return fragColor;
	}

