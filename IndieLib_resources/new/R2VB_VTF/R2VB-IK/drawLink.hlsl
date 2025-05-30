/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0 : TEXCOORD0;
};

#if 0
VsOut main(float4 pos: POSITION, float2 tc0 : TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float2 tc0_ : TEXCOORD0){
	float2 tc0=tc0_+0.5;
	float4 pos = SAMPLE2D(DMAP, tc0);
#endif

	VsOut Out;

	Out.Pos = mul(mvp, pos);
	Out.tc0 = tc0;

	return Out;
}

[Fragment shader]

sampler2D DiffuseMap;

float4 main(float2 tc0: TEXCOORD0) : COLOR {	
	return tex2D(DiffuseMap, tc0);
}

