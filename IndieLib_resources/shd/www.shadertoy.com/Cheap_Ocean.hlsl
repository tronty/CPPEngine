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

// https://www.shadertoy.com/view/NdtXDN

float2x2 rotate2D(float r){
    return float2x2(cos(r), sin(r), -sin(r), cos(r));
}

float4 main(VsOut IN): COLOR
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float i,e,f,s,g,k=.01;    
    float o=0;    
    o++;    
    for(int i=0;i<100;i++)
    {
      s=2.;      
      g+=min(f,max(.03,e))*.3;      
      float3 p=float3((fragCoord.xy-iResolution.xy/s)/iResolution.y*g,g-s);
      p.yz=mul(rotate2D(-.8), p.yz);
      p.y*=2.5;
      p.z+=iTime*1.3;
      e=p.y;
      f=p.y;
      for(;s<50.;)
      {
        s/=.66;
        p.xz=mul(rotate2D(s), p.xz);
        e+=abs(dot(sin(p*s)/s,p-p+0.6));
        f+=abs(dot(sin(p.xz*s*.33+(iTime*0.5))/s,iResolution.xy/iResolution.xy));
      }

      if(f>k*k)
        o+=e*o*k;
      else
        o+=-exp(-f*f)*o*k;


    }
    fragColor = float4(o*float4(0.33,0.7,0.85,1.0));
	return fragColor;
}

