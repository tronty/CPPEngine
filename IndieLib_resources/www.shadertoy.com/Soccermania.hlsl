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
#if 0
// https://www.shadertoy.com/view/7sK3WK
// Soccermania
float iTime=0;
float2 iResolution=float2(1,1);
float3 iMouse=float3(1,1,1);
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;
#define  ivec2 int2
#define  ivec4 int4
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
// Soccermania by Kastorp

#define tk .004
#define R iResolution
//#define DEBUG
float sdBox( in float2 p, in float2 b )
{
    float2 d = abs(p)-b;
    return length(max(d,0.0))  + min(max(d.x,d.y),0.0);
}
float2 lessThan(float2 v1, float2 v2)
{
	return v1;
	//return  float2(v1.x<v2.x,v1.y<v2.y );
}

// from https://www.shadertoy.com/view/3tyfzV
void drawChar(inout float3 color, in float3 charColor, in float2 p, in float2 pos, in float2 size, in int char) {
    p = (p - pos) / size + 0.5;
    if (all(lessThan(abs(p - 0.5), float2(0.5, 0.5)))) {
        float val = tex2D(iChannel2, p / 16.0 + frac(float2(char, 15 - char / 16) / 16.0) /* , ddx(p / 16.0), ddy(p / 16.0) */ ).r;
        color = lerp(color, charColor, val);
    }
}

float pitch(float2 uv)
{
    float d =1e5;
    uv=abs(uv);
    d=min(d,abs(length(uv) - .18));
    d=min(d,max(uv.x-.56, abs(length(uv-float2(.63,0)) - .18)));
    d=min(d,abs(sdBox(uv-float2(.45,0),float2(.45,.5))));
    d=min(d,abs(sdBox(uv-float2(.73,0),float2(.17,.4))));
    d=min(d,abs(sdBox(uv-float2(.85,0),float2(.05,.15))));
    d=min(d,length(uv-float2(.65,0.)));
    return d;
}
#endif
float4 main( in float2 U : TEXCOORD0 ) :COLOR0
{
#if 1
	return float4(0,0,0,1);
#else
    float2 uv = (U - R.xy*.5) /min(R.y,R.x*.6)*1.05;
    float3 col = float3(0.2,0.5,0.1)*(.9+.1*step(0.1,mod(uv.x,.2)))*(.9+.1*step(0.1,mod(uv.y,.2)));         
    col=lerp(col,float3(1,1,1), smoothstep(tk,.0,pitch(uv)));    
    col=lerp(col,float3(.7,.7,.7), smoothstep(tk,.0,sdBox(abs(uv)-float2(.92,0),float2(.02,.07))));
    
    ivec4 mode= ivec4( tex2D(iChannel0,ivec2(24,0)));
    ivec4 sc= ivec4( tex2D(iChannel0,ivec2(25,0)));
    drawChar(col, float3(0.,.7,0), uv, float2(-0.05,.45), float2(.1,.1), 48+sc.x);
    drawChar(col, float3(0.,.7,0), uv, float2(0.05,.45), float2(.1,.1), 48+sc.y);
    
    for(int i=0;i<=22;i++)
    {
        float2 pos=  tex2D(iChannel0,ivec2(i,0)).xy;
        col=lerp(col, (i==0?float3(1,1,1):(i%11)==1?float3(0,1,0): (i<12? float3(1,0,0):float3(0,.7,1))), smoothstep(tk,.0,-.01+length(uv-pos)));       
    }

    float4 d=tex2D(iChannel1,ivec2(U));
    int j = int(d.x);
    float2 pBall=  tex2D(iChannel0,ivec2(0,0)).xy;         
    float2 cBall =pBall*min(R.y,R.x*.6)/1.05 + R.xy*.5 ;
    float4 zBall=tex2D(iChannel1,ivec2(cBall));
#ifdef DEBUG    
    col=lerp(col, (int(zBall.x)==j || int(zBall.z)==j?3.:1. )*  ((j%11)==1?float3(1,0,1): (j<12? float3(1,0,0):float3(0,0,1))), smoothstep(0.1,.0,d.w-d.y)*.1);
#endif
    if(iMouse.z>0.) {
        col=lerp(col, float3(1,1,0), smoothstep(.001,.0,-.005*R.y+length(U-iMouse.xy)));   
    }
    return float4(col,1.0);'
#endif
}
