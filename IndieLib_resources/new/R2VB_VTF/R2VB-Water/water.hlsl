//-----------------------------------------------------------------------------
// File: water.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 mvp, mvpProj;
float3 camPos;

struct VsOut {
	float4 Pos: POSITION;
	float3 normal: TEXCOORD0;
	float3 viewVec: TEXCOORD1;
	float4 projCoord: TEXCOORD2;
};

#if 0
VsOut main(float4 pos: POSITION, float4 dmap: TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float3 pos_: POSITION, float4 dmap: TEXCOORD0){
	float2 dmap_=dmap.xy+0.5;
	float4 pos = SAMPLE2D(DMAP, dmap_);
#endif

	VsOut Out;

	float4 p = pos;
	p.y -= 12.0;
	Out.projCoord = mul(mvpProj, p);

	// Make it signed
	dmap -= 0.5;

	// Add displacement
	float d = 1000 * dmap.w;
	pos.y += d;

	Out.Pos = mul(mvp, pos);
	Out.normal = dmap.xyz;
	Out.viewVec = pos.xyz - camPos;

	return Out;
}


[Fragment shader]

sampler2D Reflection;
sampler2D Refraction;

float2x2 rot;

float4 main(float3 normal: TEXCOORD0, float3 viewVec: TEXCOORD1, float4 projCoord: TEXCOORD2) : COLOR {
	// Construct the normal
	normal.y = 0.006;
	normal = normalize(normal);
	viewVec = normalize(viewVec);

	// Compute the refraction
	float d = dot(viewVec, normal);
	float eta = 1.1;
	float k = saturate(1.0 - eta * eta * (1.0 - d * d));

	float4 refl = tex2Dproj(Reflection, projCoord);
	float4 refr = tex2Dproj(Refraction, projCoord);

	// Interpolate between reflection and refraction
	return lerp(refl * float4(0.6, 0.6, 1, 0), refr * float4(0.4, 0.4, 1, 0), k);
}
