/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]

float4x4 mvp;
float4x4 trans;
float4 LightPos;

struct VsOut {
	float4 Pos  : POSITION;
	float2 tc0  : TEXCOORD0;
	float3 nor  : TEXCOORD1;
	float3 Ldir : TEXCOORD2;
};

#if 0
VsOut main(float4 pos: POSITION, float4 nor: NORMAL, float2 tc0: TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float4 nor: NORMAL, float2 tc0_ : TEXCOORD0){
	float2 tc0=tc0_+0.5;
	float4 pos = SAMPLE2D(DMAP, tc0);
#endif

	VsOut Out;
	float4	tPos;

	tPos = float4(mul(trans, pos));
	tPos.w = 1.0;
	Out.Pos = mul(mvp, tPos);
	Out.tc0 = tc0;
	nor.w = 0.0f;
	Out.nor = mul(trans, nor);
	Out.Ldir = normalize(LightPos.xyz - tPos.xyz);

	return Out;
}

[Fragment shader]

sampler2D diffuseMap;
float4 LightColor;

float4 main(float2 tc0: TEXCOORD0, float3 nor : TEXCOORD1, float3 litdir : TEXCOORD2) : COLOR {	
	return tex2D(diffuseMap, tc0) * max(0.0f, dot(litdir, nor)) * LightColor;
}
