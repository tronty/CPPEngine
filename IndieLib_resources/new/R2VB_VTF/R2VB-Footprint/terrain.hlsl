//-----------------------------------------------------------------------------
// File: terrain.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 mvp;
float HeightFactor;

struct VsOut {
	float4 Pos:  POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 Color: COLOR0;
};

#if 0
VsOut main(float pos: POSITION, float2 tc0: TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float2 tc0_: TEXCOORD0){
	float2 tc0=tc0_+0.5;
	float pos = SAMPLE2D(DMAP, tc0).y;
#endif

	VsOut Out;
	float4 tPos;

	tPos.xyz = float3(tc0.x*2.0f, pos*HeightFactor, tc0.y*2.0f);
	tPos.w = 1.0f;
	Out.Pos = mul(mvp, tPos);
	Out.Color.rgba = pos + float4(0.05, 0.05, 0.2, 1);
	Out.TexCoord.x = (tc0.x/(1024.0f*4.0f) + 1.0f)/2.0f;
	Out.TexCoord.y = (-tc0.y/(1024.0f*4.0f) + 1.0f)/2.0f;

	return Out;
}


[Fragment shader]

sampler2D base;
//sampler2D Heightmap;
float Step = 20.0f;

float4 main(float2 tc0:TEXCOORD0, float4 color : COLOR) : COLOR {

#if 0
	float ch, Total;
	float4 Output;


	Total = 1.0f;
	ch = tex2D(Heightmap, tc0).x*1000.0f;
	for (int i=-2; i<=2; i++)
	{
		for (int j=-2; j<=2; j++)
		{
			float H = tex2D(Heightmap, float2(tc0.x+(i*Step/1024.0f), tc0.y+(j*Step/1024.0f))).x*1000.0f;
			Total += min(1.0f, max(0.0f, (H-ch)));
		}
	}
	Total /= (5*5);

	Output.rgb = saturate(float3(0.05f, 0.05f, 0.2f) + (1.0f-Total))*0.2f;
	Output.rg *= 0.5f;
	Output.a = 1.0f;

	return Output+color*0.8f;
#else
	return color;//tex2D(base, tc0)*color;
#endif
}

