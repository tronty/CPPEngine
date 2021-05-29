//-----------------------------------------------------------------------------
// File: drop.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float size;
float off;
float2 xy;

struct VsOut {
	float4 Pos: POSITION;
	float2 dir: TEXCOORD0;
	float2 tc0: TEXCOORD1;
	float2 tc1: TEXCOORD2;
	float2 tc2: TEXCOORD3;
	float2 tc3: TEXCOORD4;
	float2 tc4: TEXCOORD5;
};

VsOut main(float2 pos: POSITION){
	VsOut Out;

	// Source geometry is a quad in [(-1, -1), (1, 1)]
	Out.dir = pos;

	// Scale and put it in the right place
	pos *= size;
	pos += xy;
	Out.Pos = float4(pos, 0, 1);

	// Compute texture coordinates for the samples
	float2 tc = pos * float2(0.5, -0.5) + 0.5 + off;
	Out.tc0 = tc;
	Out.tc1 = tc + float2(-off, -off);
	Out.tc2 = tc + float2(-off,  off);
	Out.tc3 = tc + float2( off, -off);
	Out.tc4 = tc + float2( off,  off);
	return Out;
}


[Fragment shader]

sampler2D Tex;

float strength;
float2 intp;

float4 main(float2 dir: TEXCOORD0, float2 tc0: TEXCOORD1, float2 tc1: TEXCOORD2,
            float2 tc2: TEXCOORD3, float2 tc3: TEXCOORD4, float2 tc4: TEXCOORD5) : COLOR {

	float d = saturate(length(dir));
	// Turn the cone-shape into a hill-like shape
	d = smoothstep(0, 1, d);

	float4 currPixel = tex2D(Tex, tc0);
	float4 s1 = tex2D(Tex, tc1);
	float4 s2 = tex2D(Tex, tc2);
	float4 s3 = tex2D(Tex, tc3);
	float4 s4 = tex2D(Tex, tc4);

	float push = strength * (1 - d);
	float target = 0.5 + 10 * push;
	currPixel.w = intp.x * currPixel.w + intp.y * target;
	currPixel.y = -push;

	// Compute Sobel filter like in the wave.shd shader
	currPixel.x = s1.w + s2.w - s3.w - s4.w;
	currPixel.z = s1.w - s2.w + s3.w - s4.w;

	currPixel.xyz += 0.5;

	return currPixel;
}

