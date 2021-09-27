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
// https://www.shadertoy.com/view/XdSXWt
// Logistic Map - Complex
float iTime=0;
float2 iResolution=float2(1,1);
// Created by inigo quilez - iq/2014
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.


// The Logistic Map in complex numbers. Since it's a quadratic funcion,
// it's isomorphic to the Mandelbrot Set.
//
// The bifurcation diagram for the Logistic Map in real numbers is
// superimposed to better see the overlap in the dynamics across the
// x axis. More info here: https://www.shadertoy.com/view/4dSXWt
//
// Since f(z) = h·z·(1-z), as |Zn| approaches infinity we have that
// Z = h^(2^n-1)·Zo^(2^n)
//
// Hence the normalization map phi = (Zn·h)^(1/2^n). 
//
// The Green function is therefore G = log|phi| = (log|Zn|+log|h|)/(2^n)


// supersampling factor (1 for slow machines, 5 for monsters!)
#define AA 2

// complex number operations
float2 cadd( float s, float2 a ) { return float2( a.x+s, a.y ); }
float2 cmul( float2 a, float2 b )  { return float2( a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x ); }

float4 main( in float2 fragCoord:TEXCOORD ) : COLOR
{
    float3 tot = float3(0,0,0);
    #if AA>1
    for( int jj=0; jj<AA; jj++ )
    for( int ii=0; ii<AA; ii++ )
    #else
    int ii = 1, jj = 1;
    #endif
    {
        float2 q = fragCoord.xy+float2(float(ii),float(jj))/float(AA);
        float2 p = (-iResolution.xy + 2.0*q)/iResolution.y;

        // zoom
        float zoo = 0.62 + 0.38*cos(.02*iTime);
        float coa = cos( 0.1*(1.0-zoo)*iTime );
        float sia = sin( 0.1*(1.0-zoo)*iTime );
        zoo = pow( zoo,8.0);
        float2 xy = float2( p.x*coa-p.y*sia, p.x*sia+p.y*coa);
        float2 cc = float2(1.0,0.0)+smoothstep(1.0,0.5,zoo)*float2(0.24814,0.7369) + xy*zoo*2.0;

        float3 col = float3( 0,0,0 );
        
        //---------------------------------
        // logistic map in complex numbers
        //---------------------------------
        
        float2 sc = float2( abs(cc.x-1.0)-1.0,cc.y);
        if( dot(sc,sc)<1.0 )
        {
            // trick: in order to accelerate the rendering, we can detect if we
            // are inside the convergent part of the set (any of the two bulbs of period 1).
            //col = float3(0.2);
        }
        else
        {
            float co = 0.0;
            float2 z  = float2(0.5,0.0);
            for( int i=0; i<256; i++ )
            {
                if( dot(z,z)>1024.0 ) break;
                z = cmul(cc, cmul( z, cadd(1.0,-z) ) );
                co += 1.0;
            }

            // smooth interation count = n + 1 - log2( log2|h| + log2|z| );
            float sco = co + 1.0 - log2( 0.5*(log2(dot(cc,cc)) + log2(dot(z,z))) );

            col = 0.5 + 0.5*cos( 3.0 + sco*0.1 + float3(0.0,0.5,1.0));
            if( co>255.5 ) col = float3(0,0,0);
        }

        // Hubbard-Douady potential, |G|
        //float d = (log(length(z)) + log(length(cc)))/pow(2.0,co);


        //---------------------------------
        // logic map in real numbers    
        //---------------------------------
        if( abs(cc.x-1.0)<3.0 )
        {
            float al = smoothstep( 17.0, 12.0, iTime );
            col = clamp(col,0.0,1.0);
            float x = 0.5;
            for( int i=0; i<200; i++ )
            x = cc.x*x*(1.0-x);
            for( int i=0; i<200; i++ )
            {
                x = cc.x*x*(1.0-x);
                col = lerp( col, float3(1.0,1.0,0.0), 
                           (0.15+0.85*pow(clamp(abs(sc.x+1.0)*0.4,0.0,1.0),4.0))*al*
                           0.06*exp(-15000.0*(cc.y-x)*(cc.y-x)) );
            }
        }

        tot += col;
    }
    
    tot = tot/float(AA*AA);
    
	return float4( tot.x, tot.y, tot.z, 1.0 );
}

