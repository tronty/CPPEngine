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
// https://www.shadertoy.com/view/XslGRr
// Clouds
#define ivec2 int2
float iTime=0;
float2 iResolution=float2(1,1);
float2 iMouse=float2(1,1);
sampler2D iChannel0;
sampler2D iChannel1;
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
// Created by inigo quilez - iq/2013
//   https://www.youtube.com/c/InigoQuilez
//   https://iquilezles.org/
// I share this piece (art and code) here in Shadertoy and through its Public API, only for educational purposes. 
// You cannot use, sell, share or host this piece or modifications of it as part of your own commercial or non-commercial product, website or project.
// You can share a link to it or an unmodified screenshot of it provided you attribute "by Inigo Quilez, @iquilezles and iquilezles.org". 
// If you are a teacher, lecturer, educator or similar and these conditions are too restrictive for your needs, please contact me and we'll work it out.

// Volumetric clouds. Not physically correct in any way - 
// it does the wrong extintion computations and also
// works in sRGB instead of linear RGB color space. No
// shadows are computed, no scattering is computed. It is
// a volumetric raymarcher than samples an fBM and tweaks
// the colors to make it look good.
//
// Lighting is done with only one extra sample per raymarch
// step instead of using 3 to compute a density gradient,
// by using this directional derivative technique:
//
// https://iquilezles.org/www/articles/derivative/derivative.htm


// 0: one 3d texture lookup
// 1: two 2d texture lookups with hardware interpolation
// 2: two 2d texture lookups with software interpolation
#define NOISE_METHOD 1

// 0: no LOD
// 1: yes LOD
#define USE_LOD 1



float noise( in float3 x )
{
    float3 p = floor(x);
    float3 f = frac(x);
	f = f*f*(3.0-2.0*f);

#if NOISE_METHOD==0
    x = p + f;
    return textureLod(iChannel2,(x+0.5)/32.0,0.0).x*2.0-1.0;
#endif
#if NOISE_METHOD==1
	float2 uv = (p.xy+float2(37.0,239.0)*p.z) + f.xy;
    float2 rg = tex2D(iChannel0,(uv+0.5)/256.0).yx;
	return lerp( rg.x, rg.y, f.z )*2.0-1.0;
#endif    
#if NOISE_METHOD==2
    ivec3 q = ivec3(p);
	ivec2 uv = q.xy + ivec2(37,239)*q.z;
	float2 rg = lerp(lerp(tex2D(iChannel0,(uv           )&255),
				      tex2D(iChannel0,(uv+ivec2(1,0))&255),f.x),
				  lerp(tex2D(iChannel0,(uv+ivec2(0,1))&255),
				      tex2D(iChannel0,(uv+ivec2(1,1))&255),f.x),f.y).yx;
	return lerp( rg.x, rg.y, f.z )*2.0-1.0;
#endif    
}

float map( in float3 p, int oct )
{
	float3 q = p - float3(0.0,0.1,1.0)*iTime;
    float g = 0.5+0.5*noise( q*0.3 );
    
	float f;
    f  = 0.50000*noise( q ); q = q*2.02;
    #if USE_LOD==1
    if( oct>=2 ) 
    #endif
    f += 0.25000*noise( q ); q = q*2.23;
    #if USE_LOD==1
    if( oct>=3 )
    #endif
    f += 0.12500*noise( q ); q = q*2.41;
    #if USE_LOD==1
    if( oct>=4 )
    #endif
    f += 0.06250*noise( q ); q = q*2.62;
    #if USE_LOD==1
    if( oct>=5 )
    #endif
    f += 0.03125*noise( q ); 
    
    f = lerp( f*0.1-0.75, f, g*g ) + 0.1;
    return 1.5*f - 0.5 - p.y;
}

float3 sundir = normalize( float3(-1.0,0.0,-1.0) );
int kDiv = 1; // make bigger for higher quality

float4 raymarch( in float3 ro, in float3 rd, in float3 bgcol, in ivec2 px )
{
    // bounding planes	
    float yb = -3.0;
    float yt =  0.6;
    float tb = (yb-ro.y)/rd.y;
    //float tt = (yt-ro.y)/rd.t;
    float tt = (yt-ro.y)/rd.x; // ??? rd.t = rd.x

    // find tigthest possible raymarching segment
    float tmin, tmax;
    if( ro.y>yt )
    {
        // above top plane
        if( tt<0.0 ) return float4(0,0,0,1); // early exit
        tmin = tt;
        tmax = tb;
    }
    else
    {
        // inside clouds slabs
        tmin = 0.0;
        tmax = 60.0;
        if( tt>0.0 ) tmax = min( tmax, tt );
        if( tb>0.0 ) tmax = min( tmax, tb );
    }
    
    // dithered near distance
    float t = tmin + 0.1;//???*tex2D( iChannel1, px&1023 ).x;
    
    // raymarch loop
	float4 sum = float4(0,0,0,1);
    for( int i=0; i<190*kDiv; i++ )
    {
       // step size
       float dt = max(0.05,0.02*t/float(kDiv));

       // lod
       #if USE_LOD==0
       int oct = 5;
       #else
       int oct = 5 - int( log2(1.0+t*0.5) );
       #endif
       
       // sample cloud
       float3 pos = ro + t*rd;
       float den = map( pos,oct );
       if( den>0.01 ) // if inside
       {
           // do lighting
           float dif = clamp((den - map(pos+0.3*sundir,oct))/0.3, 0.0, 1.0 );
           float3  lin = float3(0.65,0.65,0.75)*1.1 + 0.8*float3(1.0,0.6,0.3)*dif;
           float4  col = float4( lerp( float3(1.0,0.95,0.8), float3(0.25,0.3,0.35), den ), den );
           col.xyz *= lin;
           // fog
           col.xyz = lerp(col.xyz,bgcol, 1.0-exp2(-0.075*t));
           // composite front to back
           col.w    = min(col.w*8.0*dt,1.0);
           col.rgb *= col.a;
           sum += col*(1.0-sum.a);
       }
       // advance ray
       t += dt;
       // until far clip or full opacity
       if( t>tmax || sum.a>0.99 ) break;
    }

    return clamp( sum, 0.0, 1.0 );
}

float3x3 setCamera( in float3 ro, in float3 ta, float cr )
{
	float3 cw = normalize(ta-ro);
	float3 cp = float3(sin(cr), cos(cr),0.0);
	float3 cu = normalize( cross(cw,cp) );
	float3 cv = normalize( cross(cu,cw) );
    return float3x3( cu, cv, cw );
}

float4 render( in float3 ro, in float3 rd, in ivec2 px )
{
	float sun = clamp( dot(sundir,rd), 0.0, 1.0 );

    // background sky
    float3 col = float3(0.76,0.75,0.86);
    col -= 0.6*float3(0.90,0.75,0.95)*rd.y;
	col += 0.2*float3(1.00,0.60,0.10)*pow( sun, 8.0 );

    // clouds    
    float4 res = raymarch( ro, rd, col, px );
    col = col*(1.0-res.w) + res.xyz;
    
    // sun glare    
	col += 0.2*float3(1.0,0.4,0.2)*pow( sun, 3.0 );

    // tonemap
    col = smoothstep(0.15,1.1,col);
 
    return float4( col, 1.0 );
}

float4 main(in float2 fragCoord : TEXCOORD0 ) : COLOR0
{
    float2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    float2 m =                iMouse.xy      /iResolution.xy;

    // camera
    float3 ro = 4.0*normalize(float3(sin(3.0*m.x), 0.8*m.y, cos(3.0*m.x))) - float3(0.0,0.1,0.0);
	float3 ta = float3(0.0, -1.0, 0.0);
    float3x3 ca = setCamera( ro, ta, 0.07*cos(0.25*iTime) );
    // ray
    float3 rd = mul(ca , normalize( float3(p.xy,1.5)));
    
    return render( ro, rd, ivec2(fragCoord-0.5) ); 
}

void mainVR( out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir )
{
    fragColor = render( fragRayOri, fragRayDir, ivec2(fragCoord-0.5) );
}
