/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
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
// https://www.shadertoy.com/view/MljXDw
// Cloudy spikeball
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
float iTime=0.0;
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
sampler2D iChannel0;
// port from http://glslsandbox.com/e#1802.0 with some modifications
//--------------
// Posted by las
// http://www.pouet.net/topic.php?which=7920&page=29&x=14&y=9

#define SCATTERING

#define pi 3.14159265
#define R(p, a) p=cos(a)*p+sin(a)*float2(p.y, -p.x)
#define hsv(h,s,v) mix(float3(1.), clamp((abs(fract(h+float3(3., 2., 1.)/3.)*6.-3.)-1.), 0., 1.), s)*v


/* original noise_
float pn(float3 p) {
   float3 i = floor(p);
   float4 a = dot(i, float3(1., 57., 21.)) + float4(0., 57., 21., 78.);
   float3 f = cos((p-i)*pi)*(-.5) + .5;
   a = mix(sin(cos(a)*a), sin(cos(1.+a)*(1.+a)), f.x);
   a.xy = mix(a.xz, a.yw, f.y);
   return mix(a.x, a.y, f.z);
}
*/

// iq's noise_
float pn( in float3 x )
{
    float3 p = floor(x);
    float3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
	float2 rg = tex2D( iChannel0, (uv+ 0.5)/256.0 ).yx;
	return -1.0+2.4*mix( rg.x, rg.y, f.z );
}


float fpn(float3 p) {
   return pn(p*.06125)*.5 + pn(p*.125)*.25 + pn(p*.25)*.125;
}

//float3 n1 = float3(1.000,0.000,0.000);
//float3 n2 = float3(0.000,1.000,0.000);
//float3 n3 = float3(0.000,0.000,1.000);
float3 n4 = float3(0.577,0.577,0.577);
float3 n5 = float3(-0.577,0.577,0.577);
float3 n6 = float3(0.577,-0.577,0.577);
float3 n7 = float3(0.577,0.577,-0.577);
float3 n8 = float3(0.000,0.357,0.934);
float3 n9 = float3(0.000,-0.357,0.934);
float3 n10 = float3(0.934,0.000,0.357);
float3 n11 = float3(-0.934,0.000,0.357);
float3 n12 = float3(0.357,0.934,0.000);
float3 n13 = float3(-0.357,0.934,0.000);
float3 n14 = float3(0.000,0.851,0.526);
float3 n15 = float3(0.000,-0.851,0.526);
float3 n16 = float3(0.526,0.000,0.851);
float3 n17 = float3(-0.526,0.000,0.851);
float3 n18 = float3(0.851,0.526,0.000);
float3 n19 = float3(-0.851,0.526,0.000);

float spikeball(float3 p) {
   float3 q=p;
   p = normalize(p);
   float4 b = max(max(max(
      abs(float4(dot(p,n16), dot(p,n17),dot(p, n18), dot(p,n19))),
      abs(float4(dot(p,n12), dot(p,n13), dot(p, n14), dot(p,n15)))),
      abs(float4(dot(p,n8), dot(p,n9), dot(p, n10), dot(p,n11)))),
      abs(float4(dot(p,n4), dot(p,n5), dot(p, n6), dot(p,n7))));
   b.xy = max(b.xy, b.zw);
   b.x = pow(max(b.x, b.y), 140.);
   return length(q)-2.5*pow(1.5,b.x*(1.-mix(.3, 1., sin(iTime*2.)*.5+.5)*b.x));
}

float f(float3 p) {
   p.z += 6.;
   R(p.xy, iTime);
   R(p.xz, iTime);
   return spikeball(p) + fpn(p*50.+iTime*15.) * 0.45;
}

/*
float3 g(float3 p) {
   float2 e = float2(.0001, .0);
   return normalize(float3(f(p+e.xyy) - f(p-e.xyy),f(p+e.yxy) - f(p-e.yxy),f(p+e.yyx) - f(p-e.yyx)));
}
*/



float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv; 
   // p: position on the ray
   // d: direction of the ray
   float3 p = float3(0.,0.,2.);
   float3 d = float3((fragCoord.xy/(0.5*iResolution.xy)-1.)*float2(iResolution.x/iResolution.y,1.0), 0.) - p;
   d = normalize(d); 
   
   // ld, td: local, total density 
   // w: weighting factor
   float ld=0., td=0.;
   float w=0.;
   
   // total color
   float3 tc = float3(0,0,0);
   
   // i: 0 <= i <= 1.
   // r: length of the ray
   // l: distance function
   float r=0., l=0., b=0.;

   // rm loop
   for (float i=0.; (i<1.); i+=1./64.) {
	   if(!((i<1.) && (l>=0.001*r) && (r < 50.)&& (td < .95)))
		   break;
      // evaluate distance function
      l = f(p) * 0.5;
      
      // check whether we are close enough (step)
      // compute local density and weighting factor 
      const float h = .05;
      ld = (h - l) * step(l, h);
      w = (1. - td) * ld;   
     
      // accumulate color and density
      tc += w; // * hsv(w, 1., 1.); // * hsv(w*3.-0.5, 1.-w*20., 1.); 
      td += w;
       
      td += 1./200.;
      
      // enforce minimum stepsize
      l = max(l, 0.03);
      
      // step forward
      p += l*d;
      r += l;
   }  
    
   #ifdef SCATTERING
   // simple scattering approximation
   tc *= 1. / exp( ld * 0.4 ) * 1.25;
   #endif
      
   fragColor = float4(tc, 1.0); //float4(tc.x+td*2., ld*3., 0, tc.x);
	return fragColor;
}
