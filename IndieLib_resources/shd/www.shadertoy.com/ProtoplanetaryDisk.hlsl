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
// https://www.shadertoy.com/view/MdtGRl
// Protoplanetary disk
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
sampler2D iChannel0;
sampler2D iChannel1;
float     iTime=0;                        // shader playback time (in seconds)
const float2      iResolution=float2(1,1);                  // viewport resolution (in pixels)
// "Protoplanetary disk" by Duke
// https://www.shadertoy.com/view/MdtGRl
//-------------------------------------------------------------------------------------
// Based on "Dusty nebula 1" (https://www.shadertoy.com/view/4lSXD1) 
// and Shane's "Cheap Cloud Flythrough" (https://www.shadertoy.com/view/Xsc3R4) shaders
// Some ideas came from other shaders from this wonderful site
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0
//-------------------------------------------------------------------------------------

//#define MOUSE_CAMERA_CONTROL

// comment this string to see fluffy clouds
#define DENSE_DUST

#define DITHERING
#define BACKGROUND

//-------------------
#define pi 3.14159265
#define R(p, a) p=cos(a)*p+sin(a)*float2(p.y, -p.x)

float2x2 Spin(float angle)
{
	return float2x2(cos(angle),-sin(angle),sin(angle),cos(angle));
}

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

float fpn(float3 p)
{
   return pn(p*.06125)*.5 + pn(p*.125)*.25 + pn(p*.25)*.125;// + pn(p*.5)*.625;
}

float rand(float2 co)
{
	return fract(sin(dot(co*0.123,float2(12.9898,78.233))) * 43758.5453);
}

float Ring(float3 p)
{
  float2 q = float2(length(p.xy)-2.3,p.z);
  return length(q)-0.01;
}

float length2( float2 p )
{
	return sqrt( p.x*p.x + p.y*p.y );
}

float length8( float2 p )
{
	p = p*p; p = p*p; p = p*p;
	return pow( p.x + p.y, 1.0/8.0 );
}


float Disk( float3 p, float3 t )
{
    float2 q = float2(length2(p.xy)-t.x,p.z*0.5);
    return max(length8(q)-t.y, abs(p.z) - t.z);
}

float smin( float a, float b, float k )
{
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}

float map(float3 p)
{
    float t=0.7*iTime;
    float2 f2=mul(Spin(t*0.25+p.z*.10), p.xy)*20.;
	float d1 = Disk(p,float3(2.0,1.,0.05)) + fpn(float3(f2.x, f2.y, p.z*20.-t)*5.0) * 0.545;
    float d2 = Ring(p);
    return smin(d1,d2,1.0);

}

// assign color to the media
float3 computeColor( float density, float radius )
{
	// color based on density alone, gives impression of occlusion within
	// the media
	float3 result = mix( 1.1*float3(1.0,0.9,0.8), float3(0.4,0.15,0.1), density );
	
	// color added for disk
	float3 colCenter = 6.*float3(0.8,1.0,1.0);
	float3 colEdge = 2.*float3(0.48,0.53,0.5);
	result *= mix( colCenter, colEdge, min( (radius+.5)/2.0, 1.15 ) );
	
	return result;
}

bool Raycylinderintersect(float3 org, float3 dir, out float near, out float far)
{
	// quadratic x^2 + y^2 = 0.5^2 => (org.x + t*dir.x)^2 + (org.y + t*dir.y)^2 = 0.5
	float a = dot(dir.xy, dir.xy);
	float b = dot(org.xy, dir.xy);
	float c = dot(org.xy, org.xy) - 12.;

	float delta = b * b - a * c;
	if( delta < 0.0 )
		return false;

	// 2 roots
	float deltasqrt = sqrt(delta);
	float arcp = 1.0 / a;
	near = (-b - deltasqrt) * arcp;
	far = (-b + deltasqrt) * arcp;
	
	// order roots
	float temp = min(far, near);
	far = max(far, near);
	near = temp;

	float znear = org.z + near * dir.z;
	float zfar = org.z + far * dir.z;

	// top, bottom
	float2 zcap = float2(1.85, -1.85);
	float2 cap = (zcap - org.z) / dir.z;

	if ( znear < zcap.y )
		near = cap.y;
	else if ( znear > zcap.x )
		near = cap.x;

	if ( zfar < zcap.y )
		far = cap.y;
	else if ( zfar > zcap.x )
		far = cap.x;
	
	return far > 0.0 && far > near;
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv; 
    const float KEY_1 = 49.5/256.0;
	const float KEY_2 = 50.5/256.0;
	const float KEY_3 = 51.5/256.0;
    float key = 0.0;
    key += 0.7* tex2D(iChannel1, float2(KEY_1,0.25)).x;
    key += 0.7* tex2D(iChannel1, float2(KEY_2,0.25)).x;
    key += 0.7* tex2D(iChannel1, float2(KEY_3,0.25)).x;

	// ro: ray origin
	// rd: direction of the ray
	float3 rd = normalize(float3((fragCoord.xy-0.5*iResolution.xy)/iResolution.y, 1.));
	float3 ro = float3(0., 0., -6.+key*1.6);

    #ifdef MOUSE_CAMERA_CONTROL
    R(rd.yz, -iMouse.y*0.01*pi*2.);
    R(rd.xz, iMouse.x*0.01*pi*2.);
    R(ro.yz, -iMouse.y*0.01*pi*2.);
    R(ro.xz, iMouse.x*0.01*pi*2.);
    #else
    R(rd.yz, -pi*3.65);
    R(rd.xz, pi*3.2);
    R(ro.yz, -pi*3.65);
   	R(ro.xz, pi*3.2);    
    #endif 
    
    #ifdef DITHERING
	float2 dpos = ( fragCoord.xy / iResolution.xy );
	float2 seed = dpos + fract(iTime);
   	// randomizing the length 
    //rd *= (1. + fract(sin(dot(float3(7, 157, 113), rd.zyx))*43758.5453)*0.1-0.03); 
	#endif 
    
	// ld, td: local, total density 
	// w: weighting factor
	float ld=0., td=0., w=0.;

	// t: length of the ray
	// d: distance function
	float d=1., t=0.;
   
	float4 sum = float4(0, 0, 0, 0);
   
    float min_dist=0.0, max_dist=0.0;

    if(Raycylinderintersect(ro, rd, min_dist, max_dist))
    {
       
	t = min_dist*step(t,min_dist);
   
	// raymarch loop
	for (int i=0; i<56; i++) 
	{
	 
		float3 pos = ro + t*rd;

		float fld = 0.0;
       
		// Loop break conditions.
        if(td>(1.-1./80.) || d<0.008*t || t>10. || sum.a > 0.99 || t>max_dist) break;
	  
		// evaluate distance function
		d = map(pos); 
       
		// direction to center
		float3 stardir = normalize(float3(0, 0, 0)-pos);
      
		// change this string to control density 
		d = max(d,0.08);
      
		if (d<0.1) 
		{
			// compute local density 
			ld = 0.1 - d;
			
            #ifdef DENSE_DUST          
			fld = clamp((ld - map(pos+0.2*stardir))/0.4, 0.0, 1.0 );
			ld += fld;
            #endif
 			
            // compute weighting factor 
			w = (1. - td) * ld;
     
			// accumulate density
			td += w + 1./200.;
		
			float radiusFromCenter = length(pos - float3(0, 0, 0));
			float4 col = float4( computeColor(td,radiusFromCenter), td );
		
			// uniform scale density
			col.a *= 0.2;
			// colour by alpha
			col.rgb *= col.a/0.8;
			// alpha blend in contribution
			sum = sum + col*(1.0 - sum.a);  
		}
      
		td += 1./70.;
       
        // point light calculations
        float3 ldst = float3(0, 0, 0)-pos;
        float lDist = max(length(ldst), 0.001);

        // star in center
        float3 lightColor=float3(1.0,0.5,0.25);
        sum.rgb+=lightColor/(lDist*lDist*lDist*7.);//*10.); //add a bloom around the light

        // using the light distance to perform some falloff
        //float atten = 1./(1. + lDist*0.125 + lDist*lDist*0.4);
        // accumulating the color
        //sum += w*atten*fld;
       
        // enforce minimum stepsize
        d = max(d, 0.04); 
      
        #ifdef DITHERING
        // add in noise_ to reduce banding and create fuzz
        d=abs(d)*(1.+0.28*rand(seed*float2(i, i)));
        #endif 
	  
        t +=  max(d * 0.3, 0.02);
      
	}
    
    //scattering test
	//sum *= 1. / exp( ld * 0.2 ) * 1.05;
        
   	sum = clamp( sum, 0.0, 1.0 );
   
    sum.xyz = sum.xyz*sum.xyz*(3.0-2.0*sum.xyz);
    
	}

    #ifdef BACKGROUND
    // stars background
    if (td<.8)
    {
    	float f=pn(rd*300.0)*0.4+0.5;
        float3 stars = float3(f, f, f);
        float3 starbg = float3(0, 0, 0);
        starbg = mix(starbg, float3(0.8,0.9,1.0), smoothstep(0.99, 1.0, stars)*clamp(dot(float3(0, 0, 0),rd)+0.75,0.0,1.0));
        starbg = clamp(starbg, 0.0, 1.0);
        sum.xyz += starbg; 
    }
	#endif
    
   fragColor = float4(sum.xyz,1.0);
	return fragColor;
}

