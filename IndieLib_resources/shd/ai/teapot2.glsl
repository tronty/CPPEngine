//-----------------------------------------------------
// Created by sebastien durand - 2021
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//-----------------------------------------------------
// inspired by nguyen nhut work https://twitter.com/nguyenhut_art
// Teapot alone: https://www.shadertoy.com/view/XsSGzG
//-----------------------------------------------------

#define PI 3.14159265

#define WITH_AO

// We remove GROUND-related references:
#define TEAPOT 6.
// #define METAL 8. // not used in this snippet

#define ZERO (min(0, iFrame))

// If you’d like to keep smin, or other utility code, no problem:
float gTime;

mat2 rot, rot1, rot2;

///////////////////////////////////////////////////////////
// Utility + Hash
///////////////////////////////////////////////////////////

// Smooth min
float smin(float a, float b, float k){
    float h = clamp(.5 + .5*(b-a)/k, 0., 1.);
    return mix(b, a, h) - k * h * (1. - h);
}

vec2 min2(vec2 a, vec2 b) {
    return (a.x < b.x) ? a : b; 
}

// Hash function (iq)
vec2 hash22(vec2 p) {
    p = fract(p * vec2(5.3983, 5.4427));
    p += dot(p.yx, p.xy + vec2(21.5351, 14.3137));
    return fract(vec2(p.x * p.y * 95.4337, p.x * p.y * 97.597));
}

///////////////////////////////////////////////////////////
// Bezier distance (two versions)
///////////////////////////////////////////////////////////

// 2D curve + z correction
vec2 sdBezier(vec2 m, vec2 n, vec2 o, vec3 p) {
    vec2 q = p.xy;
    m -= q; n -= q; o -= q;
    float x = (m.x*o.y - o.x*m.y), 
          y = 2. * (n.x*m.y - m.x*n.y),
          z = 2. * (o.x*n.y - n.x*o.y);
    vec2 i = o - m, j = o - n, k = n - m,
         s = 2. * (x * i + y * j + z * k),
         r = m + (y * z - x * x) * vec2(s.y, -s.x) / dot(s, s);
    float t = clamp(( (r.x*i.y - i.x*r.y) + 2. * (k.x*r.y - r.x*k.y )) / (x + x + y + z), 0.,1.);
    r = m + t * (k + k + t * (j - k));
    return vec2(sqrt(dot(r,r) + p.z*p.z), t);
}

// 3D direct version (iq)
vec2 sdBezier(in vec3 p, in vec3 b0, in vec3 b1, in vec3 b2 ) {
    b0 -= p; b1 -= p; b2 -= p;
    vec3 b01 = cross(b0,b1), b12 = cross(b1,b2), b20 = cross(b2,b0),
         n   = b01 + b12 + b20;
    float a = -dot(b20,n), b = -dot(b01,n), d = -dot(b12,n), m = -dot(n,n);
    vec3  g = (d - b)*b1 + (b + a*.5)*b2 + (-d - a*.5)*b0;
    float t = clamp(
        ( a*.5 + b - .5*(a*a*.25 - b*d)*dot(g, b0 - 2.*b1 + b2)/dot(g,g) ) / m,
        0., 1.
    );
    return vec2(length(mix(mix(b0,b1,t), mix(b1,b2,t), t)), t);
}

///////////////////////////////////////////////////////////
// Teapot SDF
///////////////////////////////////////////////////////////

/*
   A[]  -> teapot body profile
   T1[] -> teapot spout
   T2[] -> teapot handle
*/

// Global arrays set up in main():
vec2 A[15];
vec2 T1[5];
vec2 T2[5];

float sdTeapot(vec3 p) {
    // Spout
    vec2 spout = sdBezier(T1[2], T1[3], T1[4], p);
    float b = min(
        min(
            sdBezier(T2[0],T2[1],T2[2], p).x,
            sdBezier(T2[2],T2[3],T2[4], p).x
        ) - 0.06,
        max(
            p.y - 0.9,
            min(
                abs(sdBezier(T1[0],T1[1],T1[2], p).x - 0.07) - 0.01,
                spout.x*(1. - 0.75*spout.y) - 0.08
            )
        )
    );

    // Body - revolve around y-axis
    vec3 qq = vec3(
        sqrt(dot(p, p) - p.y*p.y),
        p.y,
        0.
    );
    float a = 99.;
    for(int i=ZERO; i<13; i+=2) {
        float dBody = (sdBezier(A[i], A[i+1], A[i+2], qq).x - 0.035)*0.9;
        a = min(a, dBody);
    }
    // Smooth min handle/spout vs body
    return smin(a, b, 0.02);
}

///////////////////////////////////////////////////////////
// "sdMan": just calls sdTeapot here
///////////////////////////////////////////////////////////
vec2 sdMan(in vec3 pos){
    vec2 res = vec2(999.0, 0.0);

    // The teapot
    {
        // transform for the teapot
        vec3 posTeapot = 3.0 * (pos - vec3(-0.9,0.25,0.4)).yxz;
        posTeapot.xy *= rot2;
        posTeapot.yz *= rot;
        posTeapot.yx *= -1.0;
        float dTeapot = 0.7 * sdTeapot(posTeapot - vec3(1., -1., 0.)) / 3.0;
        // store distance + material
        res = min2(res, vec2(dTeapot, TEAPOT));
    }

    // A small scale factor helps certain edges converge
    res.x *= 0.8;
    return res;
}

///////////////////////////////////////////////////////////
// map() - returns distance to the teapot only
///////////////////////////////////////////////////////////
vec2 map(in vec3 p0) {
    // No ground, no stones:
    return sdMan(p0);
}

///////////////////////////////////////////////////////////
// Basic Raymarcher
///////////////////////////////////////////////////////////
vec2 Trace(in vec3 ro, in vec3 rd) {
    vec2 res = vec2(999., 0.);
    float t = 0.5;
    for(int i=ZERO; i<128 && t<100.; i++) {
        vec2 h = map(ro + rd*t);
        if(abs(h.x) < 0.0005 * t) {
            return vec2(t, h.y);
        }
        t += h.x;
    }
    return res;
}

///////////////////////////////////////////////////////////
// Ambient Occlusion
///////////////////////////////////////////////////////////
#ifdef WITH_AO
float calcAO(in vec3 pos, in vec3 nor) {
    float totao = 0.0;
    float sca = 1.0;
    for(int aoi=ZERO; aoi<5; aoi++) {
        float hr = 0.01 + 0.05 * float(aoi);
        vec3 aopos = pos + nor * hr;
        float d = map(aopos).x;
        totao += -(d - hr) * sca;
        sca *= 0.75;
    }
    return clamp(1.0 - 4.0 * totao, 0., 1.);
}
#endif

///////////////////////////////////////////////////////////
// Lighting & Shading
///////////////////////////////////////////////////////////
vec3 doColor(in vec3 p, in vec3 rd, in vec3 n, in vec2 res) {
    // Basic sky
    vec3 skyColor = 0.5 * vec3(0.5, 0.6, 0.9);
    vec3 col = skyColor - max(rd.y, 0.0) * 0.5;

    // If we didn't hit the teapot, just return the sky color
    if (res.y != TEAPOT) {
        return col;
    }

    // Otherwise, color the teapot:
    col = vec3(1.0, 0.01, 0.01);

    // Directional light
    vec3 ld = normalize(vec3(50, 100, -100) - p);
    float diff = max(0.0, dot(n, ld));

    // SSS effect (fake subsurface)
    float sss = 0.5 * 0.2 * clamp(0.5 + 0.5 * dot(ld, n), 0.0, 1.0) *
                (2.0 + dot(rd, n));

    // Reflect
    vec3 r = reflect(rd, n);
    float spec = pow(max(0.0, dot(r, ld)), 40.);
    float fres = pow(abs(0.7 + dot(rd, n)), 3.0);

    // AO
    #ifdef WITH_AO
    float ao = calcAO(p, n);
    #else
    float ao = 1.0;
    #endif

    // Combine
    float amb = dot(n, ld) * 0.45 + 0.55;
    col = col * mix(1.2*vec3(0.25,0.08,0.13), vec3(0.984,0.996,0.804), 
                    mix(amb, diff, 0.75)) 
          + spec * 0.5 + fres * mix(col, vec3(1), 0.7) * 0.4;

    col += sss * vec3(1.0, 0.3, 0.2);              // fake SSS
    col += max(0.0, dot(vec3(0, -1, 0), n)) * 0.1 * skyColor; // sky “bounce”
    col *= mix(ao, 1.0, 0.5);                      // AO
    // fade with distance
    return mix(col, skyColor, smoothstep(30., 100., res.x));
}

///////////////////////////////////////////////////////////
// Normal calculation
///////////////////////////////////////////////////////////
vec3 normal(in vec3 pos, in vec3 rd, float t) {
    vec3 n = vec3(0);
    for (int i=ZERO; i<4; i++){
        // Offsets for finite differences
        vec3 e = 0.5773 * (2.0 * vec3(
            float(((i+3) >> 1) & 1),
            float(((i   ) >> 1) & 1),
            float(( i        ) & 1)
        ) - 1.0);
        n += e * map(pos + 0.002 * e).x;
    }
    // This helps with self-intersection artifacts
    return normalize(n - max(0.0, dot(n, rd)) * rd);
}

///////////////////////////////////////////////////////////
// Camera
///////////////////////////////////////////////////////////
mat3 setCamera(in vec3 ro, in vec3 ta, in float roll) {
    vec3 w = normalize(ta - ro);
    vec3 p = vec3(sin(roll), cos(roll), 0.);
    vec3 u = normalize(cross(w, p));
    vec3 v = normalize(cross(u, w));
    return mat3(u, v, w);
}

///////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////

void main() {
    // Teapot body profile
    A[0] = vec2(0,0);     A[1] = vec2(.64,0);   A[2] = vec2(.64,.03);
    A[3] = vec2(.8,.12);  A[4] = vec2(.8,.3);   A[5] = vec2(.8,.48);
    A[6] = vec2(.64,.9);  A[7] = vec2(.6,.93);  A[8] = vec2(.56,.9);
    A[9] = vec2(.56,.96); A[10]= vec2(.12,1.02);A[11]= vec2(0,1.05);
    A[12]= vec2(.16,1.14);A[13]= vec2(.2,1.2);  A[14]= vec2(0,1.2);

    // Spout
    T1[0] = vec2(1.16, .96); T1[1] = vec2(1.04, .9);  T1[2] = vec2(1,.72);
    T1[3] = vec2(.92, .48);  T1[4] = vec2(.72, .42);

    // Handle
    T2[0] = vec2(-.6, .78);  T2[1] = vec2(-1.16, .84);T2[2] = vec2(-1.16,.63);
    T2[3] = vec2(-1.2, .42); T2[4] = vec2(-.72, .24);

    // Animate time
    gTime = iTime * 6.0;
    
    // Some rotation angles
    float a = -1.5708 * 0.4;    // ~-0.6283
    rot = mat2(cos(a), sin(a), -sin(a), cos(a));

    a = 0.2*cos(0.4*iTime) + 0.3*cos(0.05*iTime);
    rot1 = mat2(cos(a), sin(a), -sin(a), cos(a));

    a = 0.5*cos(0.5*PI*gTime);
    a = a*a;
    rot2 = mat2(cos(a), sin(a), -sin(a), cos(a));
    
    // Screen coordinates
    vec2 q = xlv_TEXCOORD0.xy / iResolution.xy;
    vec2 m = iMouse.xy / iResolution.y - 0.5;
    vec2 p = -1.0 + 2.0 * q;
    p.x *= iResolution.x / iResolution.y; 

    // Camera
    vec3 ro = vec3(
        12.0 * cos(PI*(0.05*iTime + m.x)),
        4.5 + 2.0*sin(0.1*iTime) + 4.0*(m.y + 0.3),
        12.0 * sin(PI*(0.05*iTime + m.x))
    );
    vec3 ta = vec3(1.2, 1.2, 0.0); // aim somewhere near the teapot’s center
    mat3 ca = setCamera(ro, ta, 0.0);
    vec3 rd = ca * normalize(vec3(p.xy, 4.5));

    // Raymarch
    vec2 res = Trace(ro, rd);

    // If you want absolutely no fallback plane:
    // res stays as-is. If we miss the teapot, res.x=999 => sky.

    // Final shading
    vec3 pos = ro + rd * res.x;
    vec3 n = normal(pos, rd, res.x);
    vec3 col = doColor(pos, rd, n, res);

    // Gamma
    col = pow(col, vec3(0.4545));
    // Vignette
    col *= pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.1);

    gl_FragColor = vec4(col, 1.0);
}

