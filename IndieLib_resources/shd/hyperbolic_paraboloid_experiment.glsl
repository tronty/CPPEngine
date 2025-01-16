// https://www.shadertoy.com/view/styBWd hyperbolic paraboloid experiment by jt

// Experiment:
// Construct a hyperbolic paraboloid z=x*y by combining limited repetition and rotation.
// Works roughly but there are glitches (probably SDF discontinuities).
// Endpoints look slightly misaligned - perhaps due to SDF problems, too?
// Ideas for improvement welcome.

// tags: architecture, saddle, construction, hyperbolic, paraboloid, structure, building, ruled, roof

// https://en.wikipedia.org/wiki/Ruled_surface
// https://en.wikipedia.org/wiki/Saddle_roof

// MIT license, from https://www.shadertoy.com/terms

/*

Copyright (c) 2022 Jakob Thomsen

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#define EPSILON 0.001
#define DIST_MAX 50.0

#define pi 3.1415926

// https://iquilezles.org/articles/distfunctions/
float sdSphere( vec3 p, float s )
{
    return length(p) - s;
}

// https://iquilezles.org/articles/distfunctions/
float sdBoxFrame( vec3 p, vec3 b, float e )
{
       p = abs(p  )-b;
  vec3 q = abs(p+e)-e;
  return min(min(
      length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
      length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
      length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

// https://iquilezles.org/articles/distfunctions/
float line( vec3 p, vec3 a, vec3 b )
{
    vec3 pa = p - a, ba = b - a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}

float halfspace(vec3 p)
{
    return p.z+1.0;
}

// A hyperbolic paraboloid by combining limited repetition and rotation.
// Works roughly but there are glitches (probably SDF discontinuities).
// Endpoints look slightly misaligned - perhaps due to SDF problems, too?
float hyperbolic_paraboloid_half(vec3 p, float n)
{
    float d = DIST_MAX;
    float n2 = n/2.0; // half number of lines
    vec3 r = p - clamp(round(p*n2), vec3(0,0,-n2), vec3(0,0,+n2)) / n2; // repeat
    float t = round(p.z*n2)/n2; // index in [-1,+1]
    float phi = t * pi / 4.0; // rotate from -45 deg to +45 deg.
    vec2 a = vec2(cos(phi), sin(phi)); // rotation
    float s = sqrt(t*t+1.0); // length-correction
    return min(d, line(r, vec3(-a.x,-a.y,0)*s, vec3(+a.x,+a.y,0)*s));
}

float hyperbolic_paraboloid(vec3 p)
{
    float d = DIST_MAX;
    d = min(d, hyperbolic_paraboloid_half(p,20.0));
    d = min(d, hyperbolic_paraboloid_half(p.zyx,20.0));
    return d;
}

float map(vec3 p)
{
    float d = DIST_MAX;
    d = min(d, sdBoxFrame(p, vec3(1), 0.01)); // unit-frame
    d = min(d, halfspace(p));
    //d = min(d, sdSphere(p, 0.1)); // mark origin
    d = min(d, hyperbolic_paraboloid(p) - 0.02);
    return d;
}

// https://iquilezles.org/articles/normalsSDF tetrahedron normals
vec3 normal( vec3 p ) 
{
    const float h = EPSILON;
    const vec2 k = vec2(1,-1);
    return normalize( k.xyy*map( p + k.xyy*h ) + 
                      k.yyx*map( p + k.yyx*h ) + 
                      k.yxy*map( p + k.yxy*h ) + 
                      k.xxx*map( p + k.xxx*h ) );
}

float trace(vec3 ro, vec3 rd, float t0, float t1)
{
    for(float t = t0; t < t1;)
    {
        float h = map(ro + rd * t);
        if(h < EPSILON)
            return t;
        t += h;
    }
    return t1;
}
/*
float trace(vec3 ro, vec3 rd, float t0, float t1)
{
    if(t0 < 1.0)
    {
        for(float t = t0; t < 1.0;)
        {
            float h = map(ro + rd * t);
            if(h < EPSILON)
                return t;
            t += h;
        }
        t0 = 1.0;
    }

    for(float t = t0; t < t1;)
    {
        float h = map(ro + rd * t);
        if(h < EPSILON * t) // optimization: coarser epsilon in the distance
            return t;
        t += h;
    }

    return t1;
}
*/
// https://iquilezles.org/articles/rmshadows
float shadow( in vec3 ro, in vec3 rd, float mint, float maxt )
{
    for( float t=mint; t<maxt; )
    {
        float h = map(ro + rd*t);
        if( h<EPSILON )
            return 0.0;
        t += h;
    }
    return 1.0;
}

// https://iquilezles.org/articles/rmshadows
float softshadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
{
    float res = 1.0;
    float ph = 1e20;
    for( float t=mint; t<maxt; )
    {
        float h = map(ro + rd*t);
        if( h < EPSILON )
            return 0.0;
        float y = h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, k*d/max(0.0,t-y) );
        ph = h;
        t += h;
    }
    return res;
}

// https://www.shadertoy.com/view/Xds3zN raymarching primitives
float calcAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float h = 0.01 + 0.12*float(i)/4.0;
        float d = map( pos + h*nor );
        occ += (h-d)*sca;
        sca *= 0.95;
        if( occ>0.35 ) break;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 ) ;
}

void main( )
{
    vec2 p = 2.0 * (xlv_TEXCOORD0.xy) / iResolution.xy - 1.0;
    p.x *= float(iResolution.x) / float(iResolution.y);
    float mx = 2.0 * pi * float(iMouse.x) / float(iResolution.x);
    float my = pi / 2.0 + pi / 2.0 * float(iMouse.y) / float(iResolution.y);
    mx = (iMouse.x > 10.0) ? mx : 2.0 * pi * fract(iTime * 0.05);
    my = (iMouse.y > 10.0) ? my : pi / 2.0;

    mat2 Yaw = mat2(vec2(cos(mx), sin(mx)), vec2(-sin(mx), cos(mx)));
    vec3 ro = vec3(0.0, 0.0, -5.0 );//vec3(0.0, -10.0 * my, 0.0);
    //mat2 Pitch = mat2(vec2(0.0, 1.0), vec2(-1.0, 0.0));
    mat2 Pitch = mat2(vec2(cos(my), sin(my)), vec2(-sin(my), cos(my)));
    ro.yz = Pitch * ro.yz;
    ro.xy = Yaw * ro.xy;

    vec3 rd = normalize(vec3(0.5 * p.xy, 1.0)); // NOTE: omitting normalization results in clipped edges artifact
    rd.yz = Pitch * rd.yz;
    rd.xy = Yaw * rd.xy;

    float dist = trace(ro, rd, 0.0, DIST_MAX);
    vec3 dst = ro + rd * dist;
    vec3 n = normal(dst);

    vec3 lightdir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 ambient = vec3(0.1);
    float brightness = max(dot(lightdir, n), 0.0);
    brightness *= shadow(ro+rd*dist,lightdir, 0.01, DIST_MAX );
    //brightness *= softshadow(ro+rd*dist,lightdir, 0.01, DIST_MAX, 50. );
    vec3 color = (n * 0.5 + 0.5);
    //color = vec3(5.0-abs(opRepIdx(dst, 5.0)));
    //vec3 color = mix(vec3(0.25), vec3(1), step(0.01, dst.z)); // quick hack to color ground differently
    color = (ambient * calcAO(dst, n) + brightness) * color;
    //color = (ambient + calcAO(dst, n) * brightness) * color;

    //color = mix(color, vec3(0.0), step(DIST_MAX, dist));
    color = mix(color, vec3(0.0), smoothstep(DIST_MAX - 2.0, DIST_MAX, dist));
    
    gl_FragColor = vec4(sqrt(color), 1.0); // approximate gamma
}

