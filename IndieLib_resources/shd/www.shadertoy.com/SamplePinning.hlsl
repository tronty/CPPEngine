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
// https://www.shadertoy.com/view/XdfXzn
// Sample Pinning
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
sampler2D iChannel0;
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
/*
The MIT License (MIT)

Copyright (c) 2015 Huw Bowles & Daniel Zimmermann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// UPDATE: this method is now superseded by a new approach named Structured Sampling,
// see here: https://www.shadertoy.com/view/Mt3GWs
//
// Example to illustrate volume sampling research undertaken right here on
// shadertoy and published at siggraph 2015:
//
// http://advances.realtimerendering.com/s2015/index.html
//
// In particular this shader demonstrates Forward Pinning and Adaptive Sampling.
// The general advection requires state and is not implemented here, see the Unity
// implementation for this:
//
// https://github.com/huwb/volsample
//
// For a diagram shader illustrating the adaptive sampling:
//
// https://www.shadertoy.com/view/llXSD7
// 
//
// Credits - this scene is mostly mash up of these two amazing shaders:
//
// Clouds by iq: https://www.shadertoy.com/view/XslGRr
// Cloud Ten by nimitz: https://www.shadertoy.com/view/XtS3DD
// 

#define SAMPLE_COUNT 32
#define DIST_MAX 128.
#define MOUSEY (3.*iMouse.y/iResolution.y)
#define SAMPLES_ADAPTIVITY 0.2

// mouse toggle
bool useNewApproach = true;

// cam moving in a straight line
float3 lookDir = float3(-1.,0.,0.);
float3 camVel = float3(-3.,0.,0.);
float zoom = 1.2; // 1.5;

// cam spin around on spot
float samplesCurvature = 0.; // can lerp between fixed z and fixed radius sampling

float3 sundir = normalize(float3(-1.0,0.0,-1.));

// LUT based 3d value noise_
float noise_( in float3 x )
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
    
    float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
    float2 rg = tex2D( iChannel0, (uv+ 0.5)/256.0 ).yx;
    return lerp( rg.x, rg.y, f.z );
}


float4 map( in float3 p )
{
	float d = 0.1 + .8 * sin(0.6*p.z)*sin(0.5*p.x) - p.y;

    float3 q = p;
    float f;
    
    f  = 0.5000*noise_( q ); q = q*2.02;
    f += 0.2500*noise_( q ); q = q*2.03;
    f += 0.1250*noise_( q ); q = q*2.01;
    f += 0.0625*noise_( q );
    d += 2.75 * f;

    d = clamp( d, 0.0, 1.0 );
    
    float4 res = float4( d,d,d,d );
    
    float3 col = 1.15 * float3(1.0,0.95,0.8);
    col += float3(1.,0.,0.) * exp2(res.x*10.-10.);
    res.xyz = lerp( col, float3(0.7,0.7,0.7), res.x );
    
    return res;
}

// compute desired spacing between samples, modelled as a 1/z curve
float spacing(float t )
{
    // restrict to domain
    t = max(t,0.);
    
    // unnorm pdf - plot this in graphtoy to see shape
    float pdf = 1. / (SAMPLES_ADAPTIVITY*t + 1.);
	// integral of pdf over dist
	float norm = (1. / SAMPLES_ADAPTIVITY)*log(1. + SAMPLES_ADAPTIVITY*DIST_MAX);
    // norm pdf
    pdf /= norm;
    
    // sample spacing for our sample count
    return 1. / (float(SAMPLE_COUNT) * pdf);
}

// mod but moves the boundaries to keep them stationary with the camera
float mov_mod( float x, float y )
{
    return mod(x + (useNewApproach ? dot(camVel*iTime,lookDir) : 0.), y) ;
}

bool on_boundary( float x, float y )
{
    // the +0.25 solves numerical issues without changing the result
    float numericalFixOffset = y*0.25;
    return mov_mod( x + numericalFixOffset, y ) < y*0.5;
}

// put t on an appropriate sample location and initialise sampling data
void firstT( inout float t, out float dt, out float wt, out bool even )
{
    dt = exp2(floor(log2(spacing(0.))));
    t = dt - mov_mod(t,dt); // ???
    even = on_boundary(t,2.*dt);
    wt = 1.;
}

// advance t to next sample location
void nextT( inout float t, inout float dt, inout float wt, inout bool even )
{
    float s = spacing(t); // get desired sample spacing
    if( s < dt ) { dt /= 2.; even = true; } // can immediately move to higher density
    else if( even && s > 2.*dt ) { dt *= 2.; wt = 1.; even = on_boundary(t,2.*dt); } // move to lower density if a sample is there

    if( even ) wt = clamp( 2. - s/dt,0.,1.); // update wt for next odd sample - based on how far this even sample is into its band
    
    // next sample
    t += dt;
    even = !even;
}

// wt for blending in/out samples without pops
float sampleWt( float wt, bool even )
{
    return even ? (2.-wt) : wt;
}

float4 raymarch( in float3 ro, in float3 rd )
{
    float4 sum = float4(0, 0, 0, 0);
    
    // setup sampling
    float t, dt, wt; bool even;
    t=0;
    firstT( t, dt, wt, even );
    
    for(int i=0; i<SAMPLE_COUNT; i++)
    {
        if( sum.a > 0.99 ) continue;

        float3 pos = ro + t*rd;
        float4 col = map( pos );
        
        // iqs goodness
        float dif = clamp((col.w - map(pos+0.6*sundir).w)/0.6, 0.0, 1.0 );
        float3 lin = float3(0.51, 0.53, 0.63)*1.35 + 0.55*float3(0.85, 0.57, 0.3)*dif;
        col.xyz *= lin;
        
        col.xyz *= col.xyz;
        
        col.a *= 0.35;
        col.rgb *= col.a;

        // fade samples at far field
        float fadeout = 1.-clamp((t/(DIST_MAX*.3)-.85)/.15,0.,1.); // .3 is an ugly fudge factor due to oversampling
            
        // integrate
        float thisDt = dt * sampleWt( wt, even); // blend in dts
        thisDt = sqrt(thisDt/5. )*5.; // hack to soften and brighten
        sum += thisDt * col * (1.0 - sum.a) * fadeout;

        // next sample
        nextT( t, dt, wt, even );
    }

    sum.xyz /= (0.001+sum.w);

    return clamp( sum, 0.0, 1.0 );
}

float3 sky( float3 rd )
{
    float3 col = float3(0,0,0);
    
    float hort = 1. - clamp(abs(rd.y), 0., 1.);
    col += 0.5*float3(.99,.5,.0)*exp2(hort*8.-8.);
    col += 0.1*float3(.5,.9,1.)*exp2(hort*3.-3.);
    col += 0.55*float3(.6,.6,.9);
    
    float sun = clamp( dot(sundir,rd), 0.0, 1.0 );
    col += .2*float3(1.0,0.3,0.2)*pow( sun, 2.0 );
    col += .5*float3(1.,.9,.9)*exp2(sun*650.-650.);
    col += .1*float3(1.,1.,0.1)*exp2(sun*100.-100.);
    col += .3*float3(1.,.7,0.)*exp2(sun*50.-50.);
    col += .5*float3(1.,0.3,0.05)*exp2(sun*10.-10.); 
    
    float ax = atan(rd.y,length(rd.xz))/1.;
    float ay = atan(rd.z,rd.x)/2.;
    float st = tex2D( iChannel0, float2(ax,ay) ).x;
    float st2 = tex2D( iChannel0, .25*float2(ax,ay) ).x;
    st *= st2;
    st = smoothstep(0.65,.9,st);
    col = lerp(col,col+1.8*st,clamp(1.-1.1*length(col),0.,1.));
    
    return col;
}


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    //if( iMouse.z > 0. ) // ???
    //    useNewApproach = false;
    
    float2 q = fragCoord.xy / iResolution.xy;
    float2 p = -1.0 + 2.0*q;
    p.x *= iResolution.x/ iResolution.y;
    float2 mo = -1.0 + 2.0*iMouse.xy / iResolution.xy;
   
    // camera
    float3 ro = float3(0.,1.9,0.) + iTime*camVel;
    float3 ta = ro + lookDir; //float3(ro.x, ro.y, ro.z-1.);
    float3 ww = normalize( ta - ro);
    float3 uu = normalize(cross( float3(0.0,1.0,0.0), ww ));
    float3 vv = normalize(cross(ww,uu));
    float3 rd = normalize( p.x*uu + 1.2*p.y*vv + 1.5*ww );
    
    // sky
    float3 col = sky(rd);
    
    // divide by forward component to get fixed z layout instead of fixed dist layout
    float3 rd_layout = rd/lerp(dot(rd,ww),1.0,samplesCurvature);
    float4 clouds = raymarch( ro, rd_layout );
    
    col = lerp( col, clouds.xyz, clouds.w );
    
	col = clamp(col, 0., 1.);
    col = smoothstep(0.,1.,col);
	col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12 ); //Vign
        
    fragColor = float4( col, 1.0 );
	return fragColor;
}

