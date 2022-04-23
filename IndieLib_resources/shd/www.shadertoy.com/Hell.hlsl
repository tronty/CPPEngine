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
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
sampler2D iChannel7;

// Copyright Inigo Quilez, 2013 - https://iquilezles.org/
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work in any form,
// including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it.
// I share this Work for educational purposes, and you can link to it,
// through an URL, proper attribution and unmodified screenshot, as part
// of your educational material. If these conditions are too restrictive
// please contact me and we'll definitely work it out.

float noise( in float3 x )
{
#if 0
    float3 p = floor(x);
    float3 w = frac(x);
    float3 u = w*w*(3.0-2.0*w);
    float n = 1.0*p.x + 317.0*p.y + 157.0*p.z;
    return lerp( lerp( lerp(hash(n+  0.0),hash(n+  1.0),u.x),
                     lerp(hash(n+317.0),hash(n+318.0),u.x),u.y),
                lerp( lerp(hash(n+157.0),hash(n+158.0),u.x),
                     lerp(hash(n+474.0),hash(n+475.0),u.x),u.y),u.z);   
#else
    float3 p = floor(x);
    float3 f = frac(x);
	f = f*f*(3.0-2.0*f);
	
	float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
	float2 rg = tex2D( iChannel7, (uv+ 0.5)/256.0).yx;
	return lerp( rg.x, rg.y, f.z );
#endif
}

float4 map( float3 p )
{
	float den = 0.2 - p.y;

    // invert space	
	p = -7.0*p/dot(p,p);

    // twist space	
	float co = cos(den - 0.25*iTime);
	float si = sin(den - 0.25*iTime);
	p.xz = mul(float2x2(co,-si,si,co), p.xz);

    // smoke	
	float f;
	float3 q = p                          - float3(0.0,1.0,0.0)*iTime;;
    f  = 0.50000*noise( q ); q = q*2.02 - float3(0.0,1.0,0.0)*iTime;
    f += 0.25000*noise( q ); q = q*2.03 - float3(0.0,1.0,0.0)*iTime;
    f += 0.12500*noise( q ); q = q*2.01 - float3(0.0,1.0,0.0)*iTime;
    f += 0.06250*noise( q ); q = q*2.02 - float3(0.0,1.0,0.0)*iTime;
    f += 0.03125*noise( q );

	den = den + 4.0*f;
	
	float3 col = lerp( float3(1.0,0.9,0.8), float3(0.4,0.15,0.1), den ) + 0.05*sin(p);
	
	return float4( col, den );
}

float3 raymarch( in float3 ro, in float3 rd, in float2 pixel )
{
	float4 sum = float4( 0,0,0,0);

	float t = 0.0;

    // dithering	
	t += 0.05*tex2D( iChannel7, pixel.xy/iChannelResolution[0].x).x;
	
	for( int i=0; i<150; i++ )
	{
		float3 pos = ro + t*rd;
		float4 col = map( pos );
        if( col.w>0.0 )
        {
            //float len = length(pos);
            col.w = min(col.w,1.0);
            
            col.xyz *= lerp( 3.1*float3(1.0,0.5,0.05), float3(0.48,0.53,0.5), clamp( (pos.y-0.2)/1.9, 0.0, 1.0 ) );
            //col.xyz *= lerp( 3.1*float3(1.0,0.5,0.05), float3(0.48,0.53,0.5), clamp( 0.35*col.w+0.15*dot(pos,pos), 0.0, 1.0 ) );

            col.a *= 0.6;
            col.rgb *= col.a;

            sum = sum + col*(1.0 - sum.a);	
            if( sum.a > 0.99 ) break;
        }
		t += 0.05;
	}

	return clamp( sum.xyz, 0.0, 1.0 );
}

float4 main(in float2 fragCoord : TEXCOORD0 ) : COLOR0
{
    float2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
	
    // camera
    float3 ro = 4.0*normalize(float3(1.0, 1.5, 0.0));
	float3 ta = float3(0.0, 1.0, 0.0) + 0.05*(-1.0+2.0*tex2D( iChannel7, iTime*float2(0.013,0.008) ).xyz);
	float cr = 0.5*cos(0.7*iTime);
	
	// build ray
    float3 ww = normalize( ta - ro);
    float3 uu = normalize(cross( float3(sin(cr),cos(cr),0.0), ww ));
    float3 vv = normalize(cross(ww,uu));
    float3 rd = normalize( p.x*uu + p.y*vv + 2.0*ww );
	
    // raymarch	
	float3 col = raymarch( ro, rd, fragCoord );
	
	// color grade
	col = col*0.5 + 0.5*col*col*(3.0-2.0*col);
    
    // vignetting	
    float2 q = fragCoord.xy / iResolution.xy;
	col *= 0.2 + 0.8*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.1 );
	
    return float4( col, 1.0 );
}

