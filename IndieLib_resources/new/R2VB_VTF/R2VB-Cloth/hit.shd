//-----------------------------------------------------------------------------
// File: hit.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos      : POSITION;
	float4 texCoord : TEXCOORD0;
};

#if 0
VsOut main(float4 pos: POSITION, float2 texCoord: TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float2 texCoord_: TEXCOORD0){
	float2 texCoord=texCoord_+0.5;
	float4 pos = SAMPLE2D(DMAP, texCoord);
#endif
	VsOut Out;

	pos.w = 1;
	Out.Pos = mul(mvp, pos);
	Out.texCoord = float4(texCoord, 0, 0);

	return Out;
}

[Fragment shader]

float4 main(float4 texCoord: TEXCOORD0) : COLOR {
	return texCoord;
}

