//-----------------------------------------------------------------------------
// File: particle.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 mvp;
float3 dx, dy;

struct VsOut {
	float4 pos:      POSITION;
	float2 texCoord: TEXCOORD0;
	float2 color:    TEXCOORD1;
};

#if 0
VsOut main(float4 vertex: POSITION, float2 quadCoord: TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 vertex_: POSITION, float2 quadCoord_: TEXCOORD0){
	float2 quadCoord=quadCoord_.xy+0.5;
	float4 vertex = SAMPLE2D(DMAP, quadCoord);
#endif
	VsOut Out;

	Out.texCoord.xy = quadCoord;

	// Contruct quad by extending in the give direction in the provided coordinates
	float2 qc = quadCoord * 2.0 - 1.0;
	vertex.xyz += qc.x * dx + qc.y * dy;

	// Time ranges from 0 to 2, so we scale it down to 0..1
	Out.color = 0.5 * vertex.w;
	vertex.w = 1;

	Out.pos = mul(mvp, vertex);

	return Out;
}

[Fragment shader]

sampler2D Particle;
sampler1D Palette;

float4 main(float2 texCoord: TEXCOORD0, float2 color: TEXCOORD1) : COLOR {
	// Look up a color in a palette depending on the life time of the particle
	return tex2D(Particle, texCoord.xy) * tex1D(Palette, color.x);
}

