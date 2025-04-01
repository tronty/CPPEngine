/*
  Copyright (c) 2022 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

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

float iTime=0.0;
float2 iResolution=float2(1,1);
sampler2D iChannel0;

// Clouds: slice based volumetric height-clouds with god-rays, density, sun-radiance/shadow
// and 
// Water: simple reflecting sky/sun and cloud shaded height-modulated waves
//
// Created by Frank Hugenroth 03/2013
//
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//
// noise and raymarching based on concepts and code from shaders by inigo quilez
//

// some variables to change :)

#define RENDER_GODRAYS    1    // set this to 1 to enable god-rays
//#define RENDER_GODRAYS    0    // disable god-rays

#define RENDER_CLOUDS 1
#define RENDER_WATER   1

float waterlevel = 70.0;        // height of the water
float wavegain   = 1.0;       // change to adjust the general water wave level
float large_waveheight = 1.0; // change to adjust the "heavy" waves (set to 0.0 to have a very still ocean :)
float small_waveheight = 1.0; // change to adjust the small waves

float3 fogcolor    = float3( 0.5, 0.7, 1.1 );              
float3 skybottom   = float3( 0.6, 0.8, 1.2 );
float3 skytop      = float3(0.05, 0.2, 0.5);
float3 reflskycolor= float3(0.025, 0.10, 0.20);
float3 watercolor  = float3(0.2, 0.25, 0.3);

float3 light       = normalize( float3(  0.1, 0.25,  0.9 ) );









// random/hash function              
float hash( float n )
{
  return frac(cos(n)*41415.92653);
}

// 2d noise function
float noise(float2 p)
{
  return tex2D(iChannel0,p*float2(1./256.)).x;
}


// 3d noise function
float noise( in float3 x )
{
  float3 p  = floor(x);
  float3 f  = smoothstep(0.0, 1.0, fract(x));
  float n = p.x + p.y*57.0 + 113.0*p.z;

  return lerp(lerp(lerp( hash(n+  0.0), hash(n+  1.0),f.x),
    lerp( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
    lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
    lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}


float3x3 m = float3x3( 0.00,  1.60,  1.20, -1.60,  0.72, -0.96, -1.20, -0.96,  1.28 );

// Fractional Brownian motion
float fbm( float3 p )
{
  float f = 0.5000*noise( p ); p = mul(m,p)*1.1;
  f += 0.2500*noise( p ); p = mul(m,p)*1.2;
  f += 0.1666*noise( p ); p = mul(m,p);
  f += 0.0834*noise( p );
  return f;
}

float2x2 m2 = float2x2(1.6,-1.2,1.2,1.6);

// Fractional Brownian motion
float fbm( float2 p )
{
  float f = 0.5000*noise( p ); p = mul(m2,p);
  f += 0.2500*noise( p ); p = mul(m2,p);
  f += 0.1666*noise( p ); p = mul(m2,p);
  f += 0.0834*noise( p );
  return f;
}


// this calculates the water as a height of a given position
float water( float2 p )
{
  float height = waterlevel;

  float2 shift1 = 0.001*float2( iTime*160.0*2.0, iTime*120.0*2.0 );
  float2 shift2 = 0.001*float2( iTime*190.0*2.0, -iTime*130.0*2.0 );

  // coarse crossing 'ocean' waves...
  float wave = 0.0;
  wave += sin(p.x*0.021  + shift2.x)*4.5;
  wave += sin(p.x*0.0172+p.y*0.010 + shift2.x*1.121)*4.0;
  wave -= sin(p.x*0.00104+p.y*0.005 + shift2.x*0.121)*4.0;
  // ...added by some smaller faster waves...
  wave += sin(p.x*0.02221+p.y*0.01233+shift2.x*3.437)*5.0;
  wave += sin(p.x*0.03112+p.y*0.01122+shift2.x*4.269)*2.5 ;
  wave *= large_waveheight;
  wave -= fbm(p*0.004-shift2*.5)*small_waveheight*24.;
  // ...added by some distored random waves (which makes the water looks like water :)

  float amp = 6.*small_waveheight;
  shift1 *= .3;
  for (int i=0; i<7; i++)
  {
    wave -= abs(sin((noise(p*0.01+shift1)-.5)*3.14))*amp;
    amp *= .51;
    shift1 *= 1.841;
    p *= m2*0.9331;
  }
  
  height += wave;
  return height;
}


// cloud intersection raycasting
float trace_fog(in float3 rStart, in float3 rDirection )
{
#if RENDER_CLOUDS
  // makes the clouds moving...
  float2 shift = float2( iTime*80.0, iTime*60.0 );
  float sum = 0.0;
  // use only 12 cloud-layers ;)
  // this improves performance but results in "god-rays shining through clouds" effect (sometimes)...
  float q2 = 0., q3 = 0.;
  for (int q=0; q<10; q++)
  {
    float c = (q2+350.0-rStart.y) / rDirection.y;// cloud distance
    float3 cpos = rStart + c*rDirection + float3(831.0, 321.0+q3-shift.x*0.2, 1330.0+shift.y*3.0); // cloud position
    float alpha = smoothstep(0.5, 1.0, fbm( cpos*0.0015 )); // cloud density
	sum += (1.0-sum)*alpha; // alpha saturation
    if (sum>0.98)
        break;
    q2 += 120.;
    q3 += 0.15;
  }
  
  return clamp( 1.0-sum, 0.0, 1.0 );
#else
  return 1.0;
#endif
}

// fog and water intersection function.
// 1st: collects fog intensity while traveling
// 2nd: check if hits the water surface and returns the distance
bool trace(in float3 rStart, in float3 rDirection, in float sundot, out float fog, out float dist)
{
  float h = 20.0;
  float t = 0.0;
  float st = 1.0;
  float alpha = 0.1;
  float asum = 0.0;
  float3 p = rStart;
	
  for( int j=1000; j<1120; j++ )
  {
    // some speed-up if all is far away...
    if( t>500.0 ) 
      st = 2.0;
    else if( t>800.0 ) 
      st = 5.0;
    else if( t>1000.0 ) 
      st = 12.0;

    p = rStart + t*rDirection; // calc current ray position

#if RENDER_GODRAYS
    if (rDirection.y>0. && sundot > 0.001 && t>400.0 && t < 2500.0)
    {
      alpha = sundot * clamp((p.y-waterlevel)/waterlevel, 0.0, 1.0) * st * 0.024*smoothstep(0.80, 1.0, trace_fog(p,light));
      asum  += (1.0-asum)*alpha;
      if (asum > 0.9)
        break;
    }
#endif

    h = p.y - water(p.xz);

    if( h<0.1 ) // hit the water?
    {
      dist = t; 
      fog = asum;
      return true;
    }

    if( p.y>450.0 ) // lost in space? quit...
      break;
    
    // speed up ray if possible...    
    if(rDirection.y > 0.0) // look up (sky!) -> make large steps
      t += 30.0 * st;
    else
      t += max(1.0,1.0*h)*st;
  }

  dist = t; 
  fog = asum;
  if (h<10.0)
   return true;
  return false;
}


float3 camera( float time )
{
  return float3( 500.0 * sin(1.5+1.57*time), 0.0, 1200.0*time );
}

float4 main(VsOut IN) : COLOR0
{
	float4 fragColor;
	float2 fragCoord=IN.uv;

  float2 xy = -1.0 + 2.0*fragCoord.xy / iResolution.xy;
  float2 s = xy*float2(1.75,1.0);

  // get camera position and view direction
  float time = (iTime+13.5+44.)*.05;
  float3 campos = camera( time );
  float3 camtar = camera( time + 0.4 );
  campos.y = max(waterlevel+30.0, waterlevel+90.0 + 60.0*sin(time*2.0));
  camtar.y = campos.y*0.5;

  float roll = 0.14*sin(time*1.2);
  float3 cw = normalize(camtar-campos);
  float3 cp = float3(sin(roll), cos(roll),0.0);
  float3 cu = normalize(cross(cw,cp));
  float3 cv = normalize(cross(cu,cw));
  float3 rd = normalize( s.x*cu + s.y*cv + 1.6*cw );

  float sundot = clamp(dot(rd,light),0.0,1.0);

  float3 col;
  float fog=0.0, dist=0.0;

  if (!trace(campos,rd,sundot, fog, dist))
  {
    // render sky
    float t = pow(1.0-0.7*rd.y, 15.0);
    col = 0.8*(skybottom*t + skytop*(1.0-t));
    // sun
    col += 0.47*float3(1.6,1.4,1.0)*pow( sundot, 350.0 );
    // sun haze
    col += 0.4*float3(0.8,0.9,1.0)*pow( sundot, 2.0 );

#if RENDER_CLOUDS
    // CLOUDS
    float2 shift = float2( iTime*80.0, iTime*60.0 );
    float4 sum = float4(0,0,0,0); 
    for (int q=1000; q<1100; q++) // 100 layers
    {
      float c = (float(q-1000)*12.0+350.0-campos.y) / rd.y; // cloud height
      float3 cpos = campos + c*rd + float3(831.0, 321.0+float(q-1000)*.15-shift.x*0.2, 1330.0+shift.y*3.0); // cloud position
      float alpha = smoothstep(0.5, 1.0, fbm( cpos*0.0015 ))*.9; // fractal cloud density
      float3 localcolor = lerp(float3( 1.1, 1.05, 1.0 ), 0.7*float3( 0.4,0.4,0.3 ), alpha); // density color white->gray
      alpha = (1.0-sum.w)*alpha; // alpha/density saturation (the more a cloud layer's density, the more the higher layers will be hidden)
      sum += float4(localcolor*alpha, alpha); // sum up weightened color
      
      if (sum.w>0.98)
        break;
    }
    float alpha = smoothstep(0.7, 1.0, sum.w);
    sum.rgb /= sum.w+0.0001;

    // This is an important stuff to darken dense-cloud parts when in front (or near)
    // of the sun (simulates cloud-self shadow)
    sum.rgb -= 0.6*float3(0.8, 0.75, 0.7)*pow(sundot,13.0)*alpha;
    // This brightens up the low-density parts (edges) of the clouds (simulates light scattering in fog)
    sum.rgb += 0.2*float3(1.3, 1.2, 1.0)* pow(sundot,5.0)*(1.0-alpha);

    col = lerp( col, sum.rgb , sum.w*(1.0-t) );
#endif

    // add god-rays
    col += float3(0.5, 0.4, 0.3)*fog;
  }
  else
  {
#if RENDER_WATER        
    //  render water
    
    float3 wpos = campos + dist*rd; // calculate position where ray meets water

    // calculate water-mirror
    float2 xdiff = float2(0.1, 0.0)*wavegain*4.;
    float2 ydiff = float2(0.0, 0.1)*wavegain*4.;

    // get the reflected ray direction
    rd = reflect(rd, normalize(float3(water(wpos.xz-xdiff) - water(wpos.xz+xdiff), 1.0, water(wpos.xz-ydiff) - water(wpos.xz+ydiff))));  
    float refl = 1.0-clamp(dot(rd,float3(0.0, 1.0, 0.0)),0.0,1.0);
  
    float sh = smoothstep(0.2, 1.0, trace_fog(wpos+20.0*rd,rd))*.7+.3;
    // water reflects more the lower the reflecting angle is...
    float wsky   = refl*sh;     // reflecting (sky-color) amount
    float wwater = (1.0-refl)*sh; // water-color amount

    float sundot = clamp(dot(rd,light),0.0,1.0);

    // watercolor

    col = wsky*reflskycolor; // reflecting sky-color 
    col += wwater*watercolor;
    col += float3(.003, .005, .005) * (wpos.y-waterlevel+30.);

    // Sun
    float wsunrefl = wsky*(0.5*pow( sundot, 10.0 )+0.25*pow( sundot, 3.5)+.75*pow( sundot, 300.0));
    col += float3(1.5,1.3,1.0)*wsunrefl; // sun reflection

#endif

    // global depth-fog
    float fo = 1.0-exp(-pow(0.0003*dist, 1.5));
    float3 fco = fogcolor + 0.6*float3(0.6,0.5,0.4)*pow( sundot, 4.0 );
    col = lerp( col, fco, fo );

    // add god-rays
    col += float3(0.5, 0.4, 0.3)*fog; 
  }

  return float4(col,1.0);
}

