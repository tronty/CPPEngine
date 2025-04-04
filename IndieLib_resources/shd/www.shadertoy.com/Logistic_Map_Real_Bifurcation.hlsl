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
// https://www.shadertoy.com/view/4dSXWt
// Logistic Map - Real Bifurcation
float iTime=0;
float2 iResolution=float2(1,1);
// https://www.shadertoy.com/view/4dSXWt
// Created by inigo quilez - iq/2014
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.


// Bifurcation diagram for the Logistic Map, the classic
// example for the Chaos Theory. See also this other shader:
// https://www.shadertoy.com/view/XdSXWt


// Period 1 (fixed points): f(x)=0 and |f'(x)|<1
// ----------------------------------------------
// f(x)  = rx(1-x)
// f(x)' = r(1-2x)
//
// f(x)=x --> x=0 and x=1-1/r
//
// x=0,     |f(0)'|     = r        < 1 --> 0 < r < 1 --> blue curve
// x=1-1/r, |f(1-1/r)'| = r|2/r-1| < 1 --> 1 < r < 3 --> green curve


// Period 2: f(f(x))=x and |f(f(x))'|<1
// ----------------------------------------------
// f(f(x))  = r²x(1-x)(1-rx(1-x))
// f(f(x))' = r²(1-2x)[1+2rx(x-1)]
//                                                  _______
// f(f(x))=x --> (f(f(x))-x)/(f(x)-x)=0 --> x=[r+1±?r²-2r-3]/2r
//                                      _
// |f(f(x))'|<1  ... --> --> 3 < r < 1+?6=3.4495 --> red curve


// Period 3: f(f(f(x)))=x and |f(f(f(x)))'|<1
// ----------------------------------------------
// f(f(f(x)))  = r³x(...
// f(f(f(x)))' = ...
// 
// f(f(f(x)))=x --> (f(f(f(x)))-x)/(f(x)-x)=0 --> 

float4 main(in float4 pos:POSITION, in float2 fragCoord:TEXCOORD0 ) : COLOR
{
	float r = 4.0*fragCoord.x/iResolution.x;
    float y = 1.0*fragCoord.y/iResolution.y;

	//-----------------------------------------------
    // Plot the attractor    
	//-----------------------------------------------
    float x = 0.5;
    float f = 0.0;

    // get rid of the transient state
    for( int i=0; i<128; i++ ) x = r*x*(1.0-x);
    // start plotting
    for( int j=0; j<512; j++ )
    {
        x = r*x*(1.0-x);
        f += 0.1*exp(-200000.0*(y-x)*(y-x));
    }
    float f_=max(1.0-f,0.0);
    float3 col = float3(f_,f_,f_);

	//-----------------------------------------------
    // Draw analytic fixed points and branches
    //-----------------------------------------------

    float al = smoothstep(-0.1,0.1,cos(iTime*3.141593));
    
    //
    // period 1: x=0 in r=[0..1]
	float3 branch0 = float3( 0.0, 
                         1.0,
                         0.0 );
    //
    // period 2: x=1-1/r in r=[1..3]
	float3 branch1 = float3( 1.0, 
                         3.0, 
                         1.0-1.0/r );
    //                   _______                 _
    // period 2: x=[r+1±?r²-2r-3]/2r in r=[3..1+?6]
	float4 branch2 = float4( 3.0,
                         1.0+sqrt(6.0),
                         (r+1.0-sqrt(r*r-2.0*r-3.0))/(2.0*r), 
                         (r+1.0+sqrt(r*r-2.0*r-3.0))/(2.0*r) );

    col = lerp( col, float3(0,0,0), al*(1.0-smoothstep(0.0,0.01,abs(r-branch0.x))));
    col = lerp( col, float3(0,0,0), al*(1.0-smoothstep(0.0,0.01,abs(r-branch1.x))));
    col = lerp( col, float3(0,0,0), al*(1.0-smoothstep(0.0,0.01,abs(r-branch2.x))));
    col = lerp( col, float3(0,0,0), al*(1.0-smoothstep(0.0,0.01,abs(r-branch2.y))));

    col = lerp( col, float3(0,0,1), al*(1.0-smoothstep(0.0,0.02,abs(y-branch0.z)))*step(branch0.x,r)*step(r,branch0.y));
    col = lerp( col, float3(0,1,0), al*(1.0-smoothstep(0.0,0.02,abs(y-branch1.z)))*step(branch1.x,r)*step(r,branch1.y));
    col = lerp( col, float3(1,0,0), al*(1.0-smoothstep(0.0,0.02,abs(y-branch2.z)))*step(branch2.x,r)*step(r,branch2.y));
    col = lerp( col, float3(1,0,0), al*(1.0-smoothstep(0.0,0.02,abs(y-branch2.w)))*step(branch2.x,r)*step(r,branch2.y));
    
	return float4( col, 1.0 );
}

