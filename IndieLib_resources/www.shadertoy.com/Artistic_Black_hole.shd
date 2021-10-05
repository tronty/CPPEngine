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
ROW_MAJOR 
float4x4 worldViewProj 
MVPSEMANTIC
;
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
float2      iResolution=float2(1,1);                  // viewport iResolution (in pixels)
float     iTime=0;                        // shader playback iTime (in seconds)
float2      iMouse=float2(1,1);                       // iMouse pixel coords
sampler2D iChannel0;
#define PI 3.14

// This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
// Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
// or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
// =========================================================================================================

float3 doBloom(float2 uv, float blur, float threshold)
{
    float3 col;
    int cnt = 20;
    float fcnt = float(cnt);
    for (int i = 0;i <cnt;++i)
    {
        float fi = float(i);
        float coef = (fi/fcnt);
        float sz = 1.+pow(coef,2.)*blur;
        float samplePerTurn = 3.;
        float an = (fi/(fcnt/samplePerTurn))*PI;
        float2 p = uv - float2(sin(an), cos(an))*an*blur*.1;
        float3 smple = tex2D(iChannel0, p).xyz;
        if (length(smple) > threshold)
            col += smple;
    	
        
    }
    
    return col/float(cnt);
}

float4 main(VsOut IN): COLOR
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 uv = fragCoord/iResolution.xy;
    float2 cuv = (fragCoord-float2(.5,.5)*iResolution.xy)/iResolution.xx;
    
    float3 col = tex2D(iChannel0, uv).xyz;

    float bloomIntensity = 224./640.;  
    float3 bloomSample = doBloom(uv, 5./360., 237./ 640.);
    bloomSample = pow(bloomSample, float3(0.8,0.8,0.8));
    
    col = col + (bloomSample*bloomIntensity);
    


    col = lerp(col, col.zyx, pow(saturate(length(cuv*2.)),4.));


    return float4(col, 1.);
}    

