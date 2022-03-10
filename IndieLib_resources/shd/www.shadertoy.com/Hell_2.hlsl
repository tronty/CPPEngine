/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
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
// https://www.shadertoy.com/view/MdfGRX
// Hell
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
float3      iChannelResolution[4];        // channel resolution (in pixels)
const float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
const float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
sampler2D iChannel7;

// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work in any form,
// including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it.
// I share this Work for educational purposes, and you can link to it,
// through an URL, proper attribution and unmodified screenshot, as part
// of your educational material. If these conditions are too restrictive
// please contact me and we'll definitely work it out.

// Just a remix of https://www.shadertoy.com/view/MdfGRX

//------------------------------------------------------------------
// noise
//------------------------------------------------------------------
float hash( float n )
{
    return frac( n*17.0*frac( n*0.3183099 ) );
}

float noise1( in float x )
{
    float p = floor(x);
    float w = frac(x);
    float u = w*w*(3.0-2.0*w);
    return lerp(hash(p+0.0),hash(p+1.0),u);
}

float noise( in float3 x )
{
#if 1
    float3 p = floor(x);
    float3 w = frac(x);
    float3 u = w*w*(3.0-2.0*w);
    float n = 1.0*p.x + 317.0*p.y + 157.0*p.z;
    return lerp( lerp( lerp(hash(n+  0.0),hash(n+  1.0),u.x),
                     lerp(hash(n+317.0),hash(n+318.0),u.x),u.y),
                lerp( lerp(hash(n+157.0),hash(n+158.0),u.x),
                     lerp(hash(n+474.0),hash(n+475.0),u.x),u.y),u.z);   
#else
    return textureLod(iChannel7,x/32.0,0.0).x;
#endif    
}

//------------------------------------------------------------------

float4 map( float3 p, float time )
{
    // density
	float den = 0.2 - p.y;

    // invert space	(https://en.wikipedia.org/wiki/Inversive_geometry)
	p = p/dot(p,p);
    
    // scale and reverse
    p = -7.0*p;

    // twist space	
	float co = cos(0.8*den);
	float si = sin(0.8*den);
	p.xz = mul(float2x2(co,-si,si,co), p.xz);

    // cloud	
	float f;
    float t = time + 9.0;
	float3 q = p                           - float3(0.0,t*0.2,0.0);
    f  = 0.500000*noise( q ); q = q*2.21 - float3(0.0,t*0.4,0.0);
    f += 0.250000*noise( q ); q = q*2.15 - float3(0.0,t*0.8,0.0);
    f += 0.125000*noise( q ); q = q*2.13 - float3(0.0,t*1.6,0.0);
    f += 0.062500*noise( q ); q = q*2.05 - float3(0.0,t*3.2,0.0);
    f += 0.031250*noise( q );

	den = den + 4.0*f + 0.015;
	
    float3 col = lerp( float3(0.8,0.8,0.8), float3(0.5,0.5,0.5), den ) + 0.02*sin(p);
	
	return float4( col, den );
}

float3 raymarch( in float3 ro, in float3 rd, in float2 pixel, float time )
{
    // lightining
    float li = 1.0;
    li *= smoothstep(0.6,0.65,noise1( time*11.2 + 6.1 ));
    li *= smoothstep(0.4,0.45,noise1( time*1.1 + 6.1 ));

    // raymarch
    float4 sum = float4( 0,0,0,0 );
    
    const float stepFactor = 0.5;

	// with dithering
    float t = 0.05 *frac(sin(iTime+pixel.x*11.0+17.0*pixel.y)*1.317);    
	for( int i=0; i<256; i++ )
	{
		float3 pos = ro + t*rd;
		float4 col = map( pos, time );

        if( col.w>0.0 )
        {
            float len = length(pos);
            float at = smoothstep(2.0,0.0,len);
            col.xyz *= lerp( 2.5*float3(0.3,0.4,0.5), 0.9*float3(0.4,0.45,0.55), clamp( (pos.y-0.1)/2.0, 0.0, 1.0 ) );
            col.xyz *= 1.0 + 0.15*at + 1.5*li*at;

            //if( li>0.001 )
            {
            float3 dir = pos/len;
            float nn = max(0.0,col.w - map( pos-dir*0.05, time ).w);
            col.xyz += 2.0*li*(0.5+1.5*at)*nn*float3(0.8,0.8,0.8)*(1.0-col.w);
            }

            // fog
            col.xyz *= 1.15*exp2(-t*0.1);

            // compose		
            col.a *= stepFactor;
            col.rgb *= col.a;
            sum = sum + col*(1.0 - sum.a);	
            if( sum.a > 0.99 ) break;
        }

		t += 0.1*stepFactor;
	}

	return clamp( sum.xyz, 0.0, 1.0 );
}

float4 main(in float2 fragCoord : TEXCOORD0 ) : COLOR0
{
    float time = iTime;

    float2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
	
    // camera
    float3 ro = 4.0*normalize(float3(1.0, 1.5, 0.0));
	float3 ta = float3(0.0, 1.0, 0.0);
	float cr = 0.4*cos(0.4*iTime);
	
    // shake		
	ro += 0.01*(-1.0+2.0*noise1(3.1*time));
	ta += 0.01*(-1.0+2.0*noise1(3.3*time));
	
	// build ray
    float3 ww = normalize( ta - ro);
    float3 uu = normalize(cross( float3(sin(cr),cos(cr),0.0), ww ));
    float3 vv = normalize(cross(ww,uu));
    float3 rd = normalize( p.x*uu + p.y*vv + 2.0*ww );
	
    // raymarch	
    
	float3 col = raymarch( ro, rd, fragCoord, time );

    // color grade
    col = col*col*(3.0-2.0*col);
    col = col*0.5 + 0.5*col*col*(3.0-2.0*col);
    col *= 1.2;
    
    // vignette
	float2 q = fragCoord.xy / iResolution.xy;
	col *= 0.1 + 0.9*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.15 );

    return float4( col, 1.0 );
}
