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

/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

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
#define textureLod tex2D
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
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

// https://www.shadertoy.com/view/NsKGRD
// Created by Arrangemonk 2021, arrangemonk@gmail.com
// where i have a clear source i added shadertoy link


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 uv = fragCoord/iResolution.xy;
    
    float2 o = float2(-1.,1.)/iResolution.xy *4.;
    
    float2 msaa[4];
    msaa[0] = o.xx;
    msaa[1] = o.xy;
    msaa[2] = o.yx;
    msaa[3] = o.yy;
    
    float4 c = tex2D(iChannel0,uv);

    float3 color = c.xyz;
    for (int i = 0; i < 4; ++i)
    {
        color += tex2D(iChannel0,uv + msaa[i]* c.w *c.w).xyz;
    }
    
    //color = color /5.;
     //color = pow(color*color * 0.055,float3(1./2.2));
     color = Uncharted2ToneMapping(4.*pow(color*0.7+0.5,float3(4,4,4)));
     return float4(color,1);

}



