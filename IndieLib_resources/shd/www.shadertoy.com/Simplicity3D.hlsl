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
// https://www.shadertoy.com/view/ls2SDd
// Simplicity 3d
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
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
// 3d flythrough of the fractal in https://www.shadertoy.com/view/lslGWr

const int MAX_RAY_STEPS = 64;

float2 mouse() {
	return iMouse.xy / iResolution.xy - float2(.5, .5);
}

float field(in float3 p) {
	float strength = 7. + .03;
	float accum = 0.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 10; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + float3(-0.5 * (mouse().x + 1.0), -0.4 * (mouse().y + 1.0), -1.5);
		float w = exp(-float(i) / 5.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.3));
		tw += w;
		prev = mag;
	}
	return max(0., accum / tw);
}

float2 rotate2d(float2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return float2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}


float4 main(VsOut IN): COLOR 
{
	//return IN.Color;
	float4 fragColor;
	float2 fragCoord=IN.uv;
	float2 screenPos = (fragCoord.xy / iResolution.xy) * 2.0 - 1.0;
	float3 cameraDir = float3(0.0, 0.0, 1.4);
	float3 cameraPlaneU = float3(1.0, 0.0, 0.0);
	float3 cameraPlaneV = float3(0.0, 1.0, 0.0) * iResolution.y / iResolution.x;
	float3 rayDir = cameraDir + screenPos.x * cameraPlaneU + screenPos.y * cameraPlaneV;
	float3 rayPos = float3(80.0, 12.0 * sin(iTime / 4.7), 0.0);
		
    
    rayDir.y -= .2 * sin(iTime / 4.7);
    rayDir = normalize(rayDir);
    
	rayPos.xz = rotate2d(rayPos.xz, iTime / 2.0);
	rayDir.xz = rotate2d(rayDir.xz, iTime / 2.0 + 3.14 / 2.0);
    
    
    float dis = 0.0;
    
    float3 col = float3(0, 0, 0);
    for(int i=0;i<MAX_RAY_STEPS;i++){
        
        
        float val = field(rayPos * .053);
        float t = max(5.0 * val - .9, 0.0);
        
        col += sqrt(dis) * .1 * float3(0.5 * t * t * t, .6 * t * t, .7 * t);
        
        dis += 1.0 / float(MAX_RAY_STEPS);
        
        rayPos += rayDir * 1.0/ (val + .35);
    }
    
    float2 q = screenPos;
    
    col = min(col, 1.0) - .34 * (log(col + 1.0));
    
    fragColor = float4(sqrt(col.rgb), 1.0);
	return fragColor;
}

