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
	#if 1
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	#else
	Out.uv = In.uv;
	#endif
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
// https://www.shadertoy.com/view/4dSBz3
// Super simple raymarching example
float iTime=0;
const float2 iResolution=float2(1,1);//800,600);
// Super simple raymarching example. Created by Reinder Nijhoff 2017
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// @reindernijhoff
// 
// https://www.shadertoy.com/view/4dSBz3
//
// This is the shader used as example in my ray march tutorial: https://www.shadertoy.com/view/4dSfRc
//
// Created for the Shadertoy Competition 2017 
//

//
// Distance field function for the scene. It combines
// the seperate distance field functions of three spheres
// and a plane using the min-operator.
//
float map(float3 p) {
    float d = distance(p, float3(-1, 0, -5)) - 1.;     // sphere at (-1,0,5) with radius 1
    d = min(d, distance(p, float3(2, 0, -3)) - 1.);    // second sphere
    d = min(d, distance(p, float3(-2, 0, -2)) - 1.);   // and another
    d = min(d, p.y + 1.);                            // horizontal plane at y = -1
    return d;
}

//
// Calculate the normal by taking the central differences on the distance field.
//
float3 calcNormal(in float3 p) {
    float2 e = float2(1.0, -1.0) * 0.0005;
    return normalize(
        e.xyy * map(p + e.xyy) +
        e.yyx * map(p + e.yyx) +
        e.yxy * map(p + e.yxy) +
        e.xxx * map(p + e.xxx));
}

float4 main(float2 fragCoord : TEXCOORD0) : COLOR0 {
	float4 fragColor;
    float3 ro = float3(0, 0, 1);                           // ray origin

    float2 q = (fragCoord.xy - .5 * iResolution.xy ) / iResolution.y;
    float3 rd = normalize(float3(q, 0.) - ro);             // ray direction for fragCoord.xy

    // March the distance field until a surface is hit.
    float h, t = 1.;
    for (int i = 0; i < 256; i++) {
        h = map(ro + rd * t);
        t += h;
        if (h < 0.01) break;
    }

    if (h < 0.01) {
        float3 p = ro + rd * t;
        float3 normal = calcNormal(p);
        float3 light = float3(0, 2, 0);
        
        // Calculate diffuse lighting by taking the dot product of 
        // the light direction (light-p) and the normal.
        float dif = clamp(dot(normal, normalize(light - p)), 0., 1.);
		
        // Multiply by light intensity (5) and divide by the square
        // of the distance to the light.
        dif *= 5. / dot(light - p, light - p);
        
        
        fragColor = float4(float3(pow(dif, 0.4545)), 1);     // Gamma correction
    } else {
        fragColor = float4(0, 0, 0, 1);
    }
	return fragColor;
}

