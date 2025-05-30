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
// https://www.shadertoy.com/view/lsVSRW
// Dusty nebula 3
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
const float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
const float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
sampler2D iChannel0;
sampler2D iChannel1;

// "Dusty nebula 3" by Duke
//-------------------------------------------------------------------------------------
// Based on "Protoplanetary disk" (https://www.shadertoy.com/view/MdtGRl) 
// otaviogood's "Alien Beacon" (https://www.shadertoy.com/view/ld2SzK)
// and Shane's "Cheap Cloud Flythrough" (https://www.shadertoy.com/view/Xsc3R4) shaders
// Some ideas came from other shaders from this wonderful site
// Press 1-2-3 to zoom in and zoom out.
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0
//-------------------------------------------------------------------------------------

#define ROTATION

//#define MOUSE_CAMERA_CONTROL

#define DITHERING
#define BACKGROUND

//-------------------
#define pi 3.14159265
#define R(p, a) p=cos(a)*p+sin(a)*float2(p.y, -p.x)

// iq's noise_
float noise_( in float3 x )
{
    float3 p = floor(x);
    float3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
	float2 rg = tex2D( iChannel0, (uv+ 0.5)/256.0 ).yx;
	return 1. - 0.82*mix( rg.x, rg.y, f.z );
}

float fbm(float3 p)
{
   return noise_(p*.06125)*.5 + noise_(p*.125)*.25 + noise_(p*.25)*.125 + noise_(p*.4)*.2;
}

float rand(float2 co)
{
	return fract(sin(dot(co*0.123,float2(12.9898,78.233))) * 43758.5453);
}

//=====================================
// otaviogood's noise_ from https://www.shadertoy.com/view/ld2SzK
//--------------------------------------------------------------
// This spiral noise_ works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise_ functions if you're ok with some repetition.
const float nudge = 0.739513;	// size of perpendicular vector
// pythagorean theorem on that perpendicular to maintain scale
#define normalizer 1.0 / sqrt(1.0 + nudge*nudge)
float SpiralNoiseC(float3 p)
{
    float n = 0.0;	// noise_ amount
    float iter = 1.0;
    for (int i = 0; i < 8; i++)
    {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter;	// abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += float2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += float2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        // increase the frequency
        iter *= 1.733733;
    }
    return n;
}

float SpiralNoise3D(float3 p)
{
    float n = 0.0;
    float iter = 1.0;
    for (int i = 0; i < 5; i++)
    {
        n += (sin(p.y*iter) + cos(p.x*iter)) / iter;
        p.xz += float2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        iter *= 1.33733;
    }
    return n;
}

float Nebulae(float3 p)
{
	float final = p.y + 4.5;
    final += SpiralNoiseC(p.zxy*0.123+100.0)*3.0;	// large scale features
    final -= SpiralNoise3D(p);	// more large scale features, but 3d

    return final;
}

float map(float3 p) 
{
   #ifdef ROTATION
   R(p.xz, iMouse.x*0.008*pi+iTime*0.1);
   #endif
   p.y+=4.1;
   return Nebulae(p) + fbm(p*50.+iTime);
}

// assign color to the media
float3 computeColor( float density, float radius )
{
	// color based on density alone, gives impression of occlusion within
	// the media
	float3 result = mix( float3(1.0,0.9,0.8), float3(0.4,0.15,0.1), density );
	
	// color added to the media
	float3 colCenter = 7.*float3(0.8,1.0,1.0);
	float3 colEdge = 1.5*float3(0.48,0.53,0.5);
	result *= mix( colCenter, colEdge, min( (radius+.05)/1.30, 1.15 ) );
	
	return result;
}

bool RaySphereIntersect(float3 org, float3 dir, out float near, out float far)
{
	float b = dot(dir, org);
	float c = dot(org, org) - 20.;
	float delta = b*b - c;
	if( delta < 0.0) 
		return false;
	float deltasqrt = sqrt(delta);
	near = -b - deltasqrt;
	far = -b + deltasqrt;
	return far > 0.0;
}



float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv; 
    const float KEY_1 = 49.5/256.0;
	const float KEY_2 = 50.5/256.0;
	const float KEY_3 = 51.5/256.0;
    float key = 0.0;
    key += 0.7*tex2D(iChannel1, float2(KEY_1,0.25)).x;
    key += 0.7*tex2D(iChannel1, float2(KEY_2,0.25)).x;
    key += 0.7*tex2D(iChannel1, float2(KEY_3,0.25)).x;

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
    R(rd.yz, -pi*3.93);
    R(rd.xz, pi*3.2);
    R(ro.yz, -pi*3.93);
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
    
    const float h = 0.1;
   
	float4 sum = float4(0,0,0,0);
   
    float min_dist=0.0, max_dist=0.0;

    if(RaySphereIntersect(ro, rd, min_dist, max_dist))
    {
       
	t = min_dist*step(t,min_dist);
   
	// raymarch loop
	for (int i=0; i<64; i++) 
	{
	 
		float3 pos = ro + t*rd;
  
		// Loop break conditions.
        if(td>0.9 || d<0.1*t || t>10. || sum.a > 0.99 || t>max_dist) break;
	    
        // evaluate distance function
        float d = map(pos);
		       
		// change this string to control density 
		d = max(d,0.08);
      
		if (d<h) 
		{
			// compute local density 
			ld = h - d;
            
            // compute weighting factor 
			w = (1. - td) * ld;
     
			// accumulate density
			td += w + 1./200.;
		
			float radiusFromCenter = length(pos - float3(0,0,0));
			float4 col = float4( computeColor(td,radiusFromCenter), td );
		
			// uniform scale density
			col.a *= 0.185;
			// colour by alpha
			col.rgb *= col.a;
			// alpha blend in contribution
			sum = sum + col*(1.0 - sum.a);  
       
		}
      
		td += 1./70.;
       
        // point light calculations
        float3 ldst = float3(0,0,0)-pos;
        float lDist = max(length(ldst), 0.001);

        // star in center
        float3 lightColor=float3(1.0,0.5,0.25);
        sum.rgb+=lightColor/(lDist*lDist*6.); //add a bloom around the light

        sum.a *= 0.8;
        
        // enforce minimum stepsize
        d = max(d, 0.1); 
      
        #ifdef DITHERING
        // add in noise_ to reduce banding and create fuzz
        d=abs(d)*(1.+0.2*rand(seed*float2(i,i)));
        #endif 
	  
        //t += max(d * 0.25, 0.02);
        t += max(d * 0.1 * max(length(ldst),2.0), 0.02);
      
	}
    
    // simple scattering
	sum *= 1. / exp( ld * 0.2 ) * 0.6;
        
   	sum = clamp( sum, 0.0, 1.0 );
   
    sum.xyz = sum.xyz*sum.xyz*(3.0-2.0*sum.xyz);
    
	}

    #ifdef BACKGROUND
    // stars background
    if (td<.8)
    {
    	float f=noise_(rd*500.0)*0.5+0.5;
        float3 stars = float3(f,f,f);
        float3 starbg = float3(0,0,0);
        starbg = mix(starbg, float3(0.8,0.9,1.0), smoothstep(0.99, 1.0, stars)*clamp(dot(float3(0,0,0),rd)+0.75,0.0,1.0));
        starbg = clamp(starbg, 0.0, 1.0);
        sum.xyz += starbg; 
    }
	#endif
    
   fragColor = float4(sum.xyz,1.0);
	return fragColor;
}
