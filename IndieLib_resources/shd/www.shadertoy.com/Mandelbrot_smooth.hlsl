/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
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
// https://www.shadertoy.com/view/4df3Rn
// Mandelbrot - smooth
float iTime=0;
float2 iResolution=float2(1,1);
// https://www.shadertoy.com/view/4df3Rn
// Created by inigo quilez - iq/2013
//   https://www.youtube.com/c/InigoQuilez
//   https://iquilezles.org
// I share this piece (art and code) here in Shadertoy and through its Public API, only for educational purposes. 
// You cannot use, sell, share or host this piece or modifications of it as part of your own commercial or non-commercial product, website or project.
// You can share a link to it or an unmodified screenshot of it provided you attribute "by Inigo Quilez, @iquilezles and iquilezles.org". 
// If you are a teacher, lecturer, educator or similar and these conditions are too restrictive for your needs, please contact me and we'll work it out.


// See here for more information on smooth iteration count:
//
// http://iquilezles.org/www/articles/mset_smooth/mset_smooth.htm


// increase this if you have a very fast GPU
#define AA 2




float mandelbrot( in float2 c )
{
    #if 1
    {
        float c2 = dot(c, c);
        // skip computation inside M1 - http://iquilezles.org/www/articles/mset_1bulb/mset1bulb.htm
        if( 256.0*c2*c2 - 96.0*c2 + 32.0*c.x - 3.0 < 0.0 ) return 0.0;
        // skip computation inside M2 - http://iquilezles.org/www/articles/mset_2bulb/mset2bulb.htm
        if( 16.0*(c2+2.0*c.x+1.0) - 1.0 < 0.0 ) return 0.0;
    }
    #endif


    float B = 256.0;
    float l = 0.0;
    float2 z  = float2(0,0);
    for( int i=0; i<512; i++ )
    {
        z = float2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y ) + c;
        if( dot(z,z)>(B*B) ) break;
        l += 1.0;
    }

    if( l>511.0 ) return 0.0;
    
    // ------------------------------------------------------
    // smooth interation count
    //float sl = l - log(log(length(z))/log(B))/log(2.0);

    // equivalent optimized smooth interation count
    float sl = l - log2(log2(dot(z,z))) + 4.0;

    float al = smoothstep( -0.1, 0.0, sin(0.5*6.2831*iTime ) );
    l = lerp( l, sl, al );

    return l;
}

float4 main(float2 fragCoord:TEXCOORD0 ) : COLOR
{
    float3 col = float3(0,0,0);
    
#if AA>1
    for( int m=0; m<AA; m++ )
    for( int n=0; n<AA; n++ )
    {
        float2 p = (-iResolution.xy + 2.0*(fragCoord.xy+float2(float(m),float(n))/float(AA)))/iResolution.y;
        float w = float(AA*m+n);
        float time = iTime + 0.5*(1.0/24.0)*w/float(AA*AA);
#else    
        float2 p = (-iResolution.xy + 2.0*fragCoord.xy)/iResolution.y;
        float time = iTime;
#endif
    
        float zoo = 0.62 + 0.38*cos(.07*time);
        float coa = cos( 0.15*(1.0-zoo)*time );
        float sia = sin( 0.15*(1.0-zoo)*time );
        zoo = pow( zoo,8.0);
        float2 xy = float2( p.x*coa-p.y*sia, p.x*sia+p.y*coa);
        float2 c = float2(-.745,.186) + xy*zoo;

        float l = mandelbrot(c);

        col += 0.5 + 0.5*cos( 3.0 + l*0.15 + float3(0.0,0.6,1.0));
#if AA>1
    }
    col /= float(AA*AA);
#endif

    return float4( col, 1.0 );
}
