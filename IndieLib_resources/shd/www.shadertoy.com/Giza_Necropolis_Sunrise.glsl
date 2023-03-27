
float opSmoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); 

}

#define PI 3.14159265358979
#define PI_4 PI*0.25
vec3 sun_direction=vec3(0.0);
vec3 fog(vec3 col, float t){return col;}

// Description : Array and textureless GLSL 2D/3D/4D simplex
//               snoise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-snoise
//

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 permute(vec4 x) { return mod289(((x*34.0)+1.0)*x); }
vec4 taylorInvSqrt(vec4 r){ return 1.79284291400159 - 0.85373472095314 * r; }

float snoise(vec3 v)
{
	const vec2  C = vec2(1.0/6.0, 1.0/3.0);
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	// First corner
	vec3 i  = floor(v + dot(v, C.yyy));
	vec3 x0 = v - i + dot(i, C.xxx);

	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min(g.xyz, l.zxy);
	vec3 i2 = max(g.xyz, l.zxy);
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

	// Permutations
	i = mod289(i);
	vec4 p = permute( permute( permute( i.z + vec4(0.0, i1.z, i2.z, 1.0)) + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float n_ = 0.142857142857; // 1.0/7.0
	vec3  ns = n_ * D.wyz - D.xzx;
	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_);    // mod(j,N)

	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;

	vec4 h = 1.0 - abs(x) - abs(y);
	vec4 b0 = vec4(x.xy, y.xy);
	vec4 b1 = vec4(x.zw, y.zw);

	vec4 s0 = floor(b0) * 2.0 + 1.0;
	vec4 s1 = floor(b1) * 2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	vec3 p0 = vec3(a0.xy, h.x);
	vec3 p1 = vec3(a0.zw, h.y);
	vec3 p2 = vec3(a1.xy, h.z);
	vec3 p3 = vec3(a1.zw, h.w);

	//Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));

	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	// Mix final snoise value
	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	m = m * m;

	return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3)));
}

vec3 rotate_x(vec3 p, float phi)
{   float c = cos(phi);	float s = sin(phi);
    return vec3(p.x, c*p.y - s*p.z, s*p.y + c*p.z);
}
vec3 rotate_y(vec3 p, float phi)
{	float c = cos(phi);	float s = sin(phi);
	return vec3(c*p.x + s*p.z, p.y, c*p.z - s*p.x);
}
vec3 rotate_z(vec3 p, float phi)
{	float c = cos(phi);	float s = sin(phi);
	return vec3(c*p.x - s*p.y, s*p.x + c*p.y, p.z);
}
 
mat2 r2D(float a)
{
	float si = sin(a);
	float co = cos(a);
	return mat2(si, co, -co, si);
}

float fbm(vec2 p)//ground
{
    float s=.3;
    mat2 m = r2D(1.);
    vec2 r= vec2(0.5);
    for(int i=0;i<7;i++)r+=sin(p), p=m*(p*s+.2*r+.1),s*=1.6;
    return (r.x+r.y)*.8 - 2.;
}

/*---------------------------------------------

Giza Necropolis, Sandaruwan Silva 2022

Nine pyramids of the Giza pyramid complex.

1. Clouds and snoise functions etc. based on Inigo Quilez's Rainforest
   https://www.shadertoy.com/view/4ttSWf
2. Dust storm roughly based on VoidChicken's The Rude Sandstorm 
   https://www.shadertoy.com/view/Mly3WG

---------------------------------------------*/

float sdSphere(in vec3 p, in float radius) {
     return length(p) - radius;
}

float sdBox( vec3 p, vec3 b ) {
  vec3 q = abs(p) - b;
  return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdOctahedron(in vec3 p, in float s) {
    p = abs(p);
    return (p.x + p.y + p.z - s) * 0.57735027;
}

float sdOctahedronStepped(in vec3 p, in float s) {
    p = abs(p);
    return (p.x + floor(p.y * 15.0) / 15.0 + p.z - s) * 0.47735027;
}

float sdGiza(in vec3 p, in float s, in float dmg) {
    p = abs(p);
    float v = (p.x + p.y + p.z - s) * 0.57735027;
    
    p.y -= s + 0.25 - dmg;
    
    float b = sdBox(p, vec3(0.25));
    v = max(v, -b);
    return v;
}

// -- cloud generation -----------------------

vec4 sky( in vec3 ro, in vec3 rd ) {
    vec3 col = vec3(0.3, 0.4, 1.0) - rd.y * 0.7;
    float t = (1000.0 - ro.y) / rd.y;

    
    if(t > 0.0) {
        vec2 uv = (ro + t * rd).xz;
        float cl = fbm(uv * 0.002 - iTime * 0.2);
        float dl = smoothstep(-0.2, 0.6, cl);
        col = mix(col, vec3(1.0), 0.12 * dl);
    }
    
	return vec4(col, t);
}

// -- scene ----------------------------------

// scene map
vec3 map(in vec3 p) {
    vec3 res = vec3(-1.0);
    
    vec3 offset = vec3(0.3, 0.0, -8.0);
    vec3 offset2 = vec3(0.6, 0.0, -8.0);
    
    float d = 0.0;
    
    // snoise values
    float nv1 = snoise(p * 0.25 + 2.1);
    float nv2 = snoise(p * 230.0 + 2.1);
    float nv3 = snoise(p * 12.5 + 2.1);
    float nv4 = snoise(vec3(p.x, p.y, 0.0) * 1.5 + 2.1);
    float nv5 = snoise(p * 49.5 + 2.1);
    float nv6 = snoise(vec3(p.x, p.y, 0.0) * 0.5 + 2.1);
    
    
    // height map
    float heightMap = nv1 * 0.5 + nv2 * 0.002;
    float terrain = p.y + heightMap;
    
    // common values for pyramids
    p = rotate_y(p, PI_4);
    vec3 mainOffset = p + offset;
    float mainNoise1 = nv3 * 0.02;
    float mainNoise2 = nv4 * 0.07;
    float mainNoiseTop = nv5 * 0.015;
	p = rotate_y(p, PI_4);
    
    vec3 smallOffset = p + offset2;
    
    float ySin = smoothstep(0.93, 1.0, sin(p.y * 400.0)) * 0.01;
    float xSin = smoothstep(0.94, 1.0, sin(p.x * 260.0)) * 0.005;
    float zSin = smoothstep(0.94, 1.0, sin(p.z * 260.0)) * 0.005;
    float sinComb = ySin + xSin + zSin;
    
    float ySinM1 = smoothstep(0.2, 1.0, sin(p.y * 200.)) * 0.007;
    float xSinM1 = smoothstep(0.2, 1.0, sin(p.x * 200.)) * 0.005;
    float zSinM1 = smoothstep(0.2, 1.0, sin(p.z * 200.)) * 0.005;
    float sinMComb1 = ySinM1 + xSinM1 + zSinM1;
    
    float ySinM2 = smoothstep(0.2, 1.0, sin(p.y * 100.)) * 0.005;
    float xSinM2 = smoothstep(0.2, 1.0, sin(p.x * 100.)) * 0.005;
    float zSinM2 = smoothstep(0.2, 1.0, sin(p.z * 100.)) * 0.005;
    float sinMComb2 = ySinM2 * 4.0 + xSinM2 + zSinM2;
    
    // pyramid of Menkaure, one in the front
    float p1 = sdGiza(mainOffset + vec3(-3.0, 2.8, 1.0),
        4.9 - sinMComb1 - mainNoise1 * 0.2 - nv5 * 0.01 - mainNoise2 * 0.2, 0.08);
    
    // pyramid of Khafre, one in the middle
    float p2 = sdOctahedron(mainOffset + vec3(0.0, 2.0, 15.0),
        7.5 - sinMComb2 - mainNoise1 - mainNoise2);
    // pyramid top
    float ptop = sdGiza(mainOffset + vec3(0.01, -4.0, 14.98),
        1.58 - mainNoiseTop, 0.05);
    ptop = max(ptop, -sdSphere(mainOffset + vec3(0.04, -1.8, 13.98), 2.8 + nv1 * 0.2 +  nv3 * 0.1));

    // pyramid of Khufu, distant one
    float p3 = sdGiza(mainOffset + vec3(0.0, 2.0, 30.0),
        8.0 - sinMComb2 - mainNoise1 - mainNoise2, 0.2);

    // queens' pyramids
    float p4 = sdOctahedronStepped(smallOffset + vec3(-5.6, 0.47, -4.4),
        0.58 - sinComb - nv5 * 0.02 - nv6 * 0.07);
    float p5 = sdOctahedronStepped(smallOffset + vec3(-6.0, 0.37, -4.0),
        0.58 - sinComb - nv5 * 0.02 - nv6 * 0.07);
    float p6 = sdOctahedronStepped(smallOffset + vec3(-6.4, 0.34, -3.6),
        0.58 - sinComb - nv5 * 0.02 - nv6 * 0.07);
    
    // remove algorithmic artifacts on the top
    p5 = max(p5, -sdBox(smallOffset + vec3(-6.0, -0.3, -4.0), vec3(0.1)));
    
    // spheres to smoothen the pyramids
    float s4 = sdSphere(p + vec3(5.2, 0.2, -12.3), 0.3);
    float s5 = sdSphere(p + vec3(4.7, 0.1, -12.3), 0.22);
    float s6 = sdSphere(p + vec3(4.2, 0.1, -12.3), 0.22);
    
    
    // other pyramids in the complex
    // represent them with just spheres since they're far away
    float s7 = sdSphere(p + vec3(-22.2, -1.1, 11.0),
        0.2 - ySin - xSin - zSin - nv5 * 0.01 - nv6 * 0.07);
    float s8 = sdSphere(p + vec3(-22.2, -0.7, 13.0),
        0.5 - ySin - xSin - zSin - nv5 * 0.01 - nv6 * 0.07);
    float s9 = sdSphere(p + vec3(-22.2, -0.5, 15.0),
        0.5 - ySin - xSin - zSin - nv5 * 0.01 - nv6 * 0.07);
    
    
    d = min(terrain, p1);
    d = min(d, p2);
    d = min(d, ptop);
    d = min(d, p3);
    
    d = opSmoothUnion(d, s7, 2.6);
    d = opSmoothUnion(d, s8, 1.2);
    d = opSmoothUnion(d, s9, 1.2);
    
    // add queens' pyramids
    d = opSmoothUnion(opSmoothUnion(d, p4, 0.05), s4, 0.2);
    d = opSmoothUnion(opSmoothUnion(d, p5, 0.05), s5, 0.2);
    d = opSmoothUnion(opSmoothUnion(d, p6, 0.05), s6, 0.2);
    
    res.x = d;
    return res;
}

// ray marching through terrain and objects
vec3 trace(in vec3 ro, in vec3 rd) {
    float tMax = 1000.0;
    vec3  res  = vec3(-1.0);
    vec3  last = vec3(0.0);
    float t    = 0.0;

    vec3 dust = vec3(1.0);
    
    for (int i = 0; i < 150; i++) {
        vec3 pos = ro + t * rd;
        vec3 hit = map(pos);
        
        if (abs(hit.x) < 0.0001 * t || t > tMax) break;

        t += hit.x;
        last = hit;

        // add dust cloud
        vec3 a = vec3(snoise(pos + iTime * 0.13) + 0.1);
        dust += a * sqrt(float(i)) * 1.0 *
            pow(smoothstep(0.0, 1.0, clamp(float(i) / 15.0, 0.0, 1.0)), 1.0 / 2.5);
    }
    
    if (t < tMax) {
        res = vec3(t, last.yz);
    }
    
    dust.x = clamp(dust.x, -200.0, 200.0);
    res.y = dust.x * 0.001;
    return res;
}

// calculate normals
vec3 calcNormal(in vec3 p, in float t) {
    vec3 n = vec3(0.0);
    
    for (int i = 0; i < 4; i++) {
        vec3 e = 0.5773 * (2.0 * vec3((((i + 3) >> 1) & 1),
            ((i >> 1) &1 ),(i & 1)) - 1.0);
        n += e * map(p + e * 0.001 * t).x;
    }
    
    return normalize(n);
}

// calculate ao
float calcAO(in vec3 pos, in vec3 nor) {
	float occ = 0.0;
    float sca = 1.0;
    
    for(int i=0; i < 5; i++) {
        float h = 0.001 + 0.15 * float(i) / 4.0;
        float d = map(pos + h * nor).x;
        occ += (h - d) * sca;
        sca *= 0.95;
    }
    
    return clamp(1.0 - 1.5 * occ, 0.0, 1.0);    
}

// camera/projection
mat3 camera(in vec3 ro, in vec3 ta, float cr) {
	vec3 cw = normalize(ta - ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize(cross(cw, cp));
	vec3 cv = normalize(cross(cu, cw));
    return mat3(cu, cv, cw);
}

void main() {
    vec2  p    = (2.0 * xlv_TEXCOORD0 - iResolution.xy) / iResolution.y;
    vec3  col  = vec3(0.0);
    float time = iTime;
    
    // set up camera/projection
    float an = -0.35;
    float ra = 17.0;
    float fl = 4.0;
    vec3  ta = vec3(sin(time * 0.13 - 0.1) * 0.1,
        cos(time * 0.1 - 4.0) * 0.05 + 0.1, 0.0);
    vec3  ro = ta + vec3(ra * sin(an), 0.0, ra * cos(an));
    mat3  ca = camera(ro, ta, 0.0);
    vec3  rd = ca * normalize(vec3(p.x + 0.3, p.y + 0.4,fl));
    
    // ray marching
    vec3 ray = trace(ro, rd);
    float resT = 1000.;
    
    if (ray.x >= 0.0) {
        float t   = ray.x;
        vec3 pos  = ro + t * rd;
        vec3 norm = calcNormal(pos, t);
        
        // prepare diffusion
        float sun_dif = clamp(dot(norm, sun_direction), 0.0, 1.0);
        vec3  sky_dir = normalize(vec3(0.0, 1.0, 0.0));
        float sky_dif = clamp(dot(norm, sky_dir), 0.0, 1.0);
        
        // set up cheap shadows
        float sun_sha = trace(pos + norm * 0.0001, sun_direction).x;
        float dif = clamp(sun_dif, 0.0, 1.0);
        
        col += vec3(0.6, 0.4, 0.2) * dif * 1.29;
        col += vec3(0.6, 0.4, 0.8) * sky_dif * 0.4;
        col *= calcAO(pos, norm);
        
        if (sun_sha > 0.0) {
            col *= 0.5;
        }
        
        col += ray.y * vec3(1.0, 0.8, 0.4) * 0.6;
        
        col = fog(col, t);
        col *= 0.5 + 0.5 * vec3(0.8, 0.3, 0.0);
 
    } else {
        vec4 sky_color = sky(ro, rd);
        col = sky_color.rgb;
        
        col = fog(col, sky_color.a * 0.01);
    }
    

    // gamma
    col = pow(clamp(col * 1.1 - 0.04, 0.0, 1.0), vec3(0.4545));

    // contrast
    col = col * col * (3.0 - 2.0 * col);            

    // add sun glare
    float sun = clamp(dot(sun_direction, rd), 0.0, 1.0);
    col += 0.75 * vec3(0.8, 0.4, 0.2) * sun * sun * sun;
    
    gl_FragColor = vec4(col, 1.0);
}

