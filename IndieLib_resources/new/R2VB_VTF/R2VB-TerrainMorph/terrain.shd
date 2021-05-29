//-----------------------------------------------------------------------------
// File: terrain.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 mvp;
float TexFactor;
float HeightFactor;

struct VsOut {
	float4 Pos:      POSITION;
	float2 TexCoord: TEXCOORD0;
	float4 Color:    COLOR;
};

#if 0
VsOut main(float2 pos: POSITION, float4 dmap: TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float4 dmap_: TEXCOORD0){
	float2 dmap=dmap_.xy+0.5;
	float2 pos = SAMPLE2D(DMAP, dmap).xy;
#endif

	VsOut Out;
	float4 tPos;

	tPos = float4(pos.x, dmap.x*HeightFactor, pos.y, 1.0f);
	Out.Pos = mul(mvp, tPos);
	Out.Color.rgba = dmap.x;
	Out.TexCoord = float2(pos.x/TexFactor, pos.y/TexFactor);
	
	return Out;
}


[Fragment shader]

sampler2D base;
sampler2D base2;
float time;

float4 main(float2 texcoord:TEXCOORD0, float4 color : COLOR) : COLOR {
	float4 tex = lerp(tex2D(base, texcoord), tex2D(base2, texcoord), time);
	return float4(color.r, color.g, color.b, 1)* tex;
}

