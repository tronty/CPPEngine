/******************************************************************************
File:  vnoise.glsl

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:

sgreen 5/02/02:

This is based on Perlin's original code:
    http://mrl.nyu.edu/~perlin/doc/oscar.html

    It combines the permutation and gradient tables into one array of
    vec4's to conserve constant memory.
    The table is duplicated twice to avoid modulo operations.

jallen@nvidia.com: 10/12/03:

    GLSL version of Cg vertex noise shader

Notes:

    Should use separate tables for 1, 2 and 3D versions

******************************************************************************/

#define B  32      // table size
#define B2 66      // B*2 + 2
#define BR 0.03125 // 1 / B

// this is the smoothstep function f(t) = 3t^2 - 2t^3, without the normalization
vec3 s_curve(vec3 t)
{
    return t*t*( vec3(3.0, 3.0, 3.0) - vec3(2.0, 2.0, 2.0)*t);
}

vec2 s_curve(vec2 t)
{
    return t*t*( vec2(3.0, 3.0) - vec2(2.0, 2.0)*t);
}

float s_curve(float t)
{
    return t*t*(3.0-2.0*t);
}

// 3D version
float noise(vec3 v, vec4 pg[])
{
    v = v + vec3(10000.0, 10000.0, 10000.0);   // hack to avoid negative numbers

    vec3 i = fract(v * BR) * float(B);   // index between 0 and B-1
    vec3 f = fract(v);            // fractional position

    // lookup in permutation table
    vec2 p;
    p.x = pg[ int(i[0])     ].w;
    p.y = pg[ int(i[0]) + 1 ].w;
    p = p + i[1];

    vec4 b;
    b.x = pg[ int(p[0]) ].w;
    b.y = pg[ int(p[1]) ].w;
    b.z = pg[ int(p[0]) + 1 ].w;
    b.w = pg[ int(p[1]) + 1 ].w;
    b = b + i[2];

    // compute dot products between gradients and vectors
    vec4 r;
    r[0] = dot( pg[ int(b[0]) ].xyz, f );
    r[1] = dot( pg[ int(b[1]) ].xyz, f - vec3(1.0, 0.0, 0.0) );
    r[2] = dot( pg[ int(b[2]) ].xyz, f - vec3(0.0, 1.0, 0.0) );
    r[3] = dot( pg[ int(b[3]) ].xyz, f - vec3(1.0, 1.0, 0.0) );

    vec4 r1;
    r1[0] = dot( pg[ int(b[0]) + 1 ].xyz, f - vec3(0.0, 0.0, 1.0) );
    r1[1] = dot( pg[ int(b[1]) + 1 ].xyz, f - vec3(1.0, 0.0, 1.0) );
    r1[2] = dot( pg[ int(b[2]) + 1 ].xyz, f - vec3(0.0, 1.0, 1.0) );
    r1[3] = dot( pg[ int(b[3]) + 1 ].xyz, f - vec3(1.0, 1.0, 1.0) );

    // interpolate
    f = s_curve(f);
    r = mix( r, r1, f[2] );
    r = mix( r.xyyy, r.zwww, f[1] );
    return mix( r.x, r.y, f[0] );
}

// 2D version
float noise(vec2 v, vec4 pg[])
{
    v = v + vec2(10000.0, 10000.0);

    vec2 i = fract(v * BR) * float(B);   // index between 0 and B-1
    vec2 f = fract(v);            // fractional position

    // lookup in permutation table
    vec2 p;
    p[0] = pg[ int(i[0])   ].w;
    p[1] = pg[ int(i[0]) + 1 ].w;
    p = p + i[1];

    // compute dot products between gradients and vectors
    vec4 r;
    r[0] = dot( pg[ int(p[0]) ].xy,   f);
    r[1] = dot( pg[ int(p[1]) ].xy,   f - vec2(1.0, 0.0) );
    r[2] = dot( pg[ int(p[0]) + 1 ].xy, f - vec2(0.0, 1.0) );
    r[3] = dot( pg[ int(p[1]) + 1 ].xy, f - vec2(1.0, 1.0) );

    // interpolate
    f = s_curve(f);
    r = mix( r.xyyy, r.zwww, f[1] );
    return mix( r.x, r.y, f[0] );
}

// 1D version
float noise(float v, vec4 pg[])
{
    v = v + 10000.0;

    float i = fract(v * BR) * float(B);   // index between 0 and B-1
    float f = fract(v);            // fractional position

    // compute dot products between gradients and vectors
    vec2 r;
    r[0] = pg[int(i)].x * f;
    r[1] = pg[int(i) + 1].x * (f - 1.0);

    // interpolate
    f = s_curve(f);
    return mix( r[0], r[1], f);
}

uniform float Displacement;
uniform vec4 pg[B2];            // permutation/gradient table

void main()
{
    vec4 noisePos = gl_TextureMatrix[0] * gl_Vertex;

    float i = (noise(noisePos.xyz, pg) + 1.0) * 0.5;
    gl_FrontColor = vec4(i, i, i, 1.0);

    // displacement along normal
    vec4 position = gl_Vertex + (vec4(gl_Normal, 1.0) * i * Displacement);
    position.w = 1.0;

    gl_Position = gl_ModelViewProjectionMatrix * position;
}
