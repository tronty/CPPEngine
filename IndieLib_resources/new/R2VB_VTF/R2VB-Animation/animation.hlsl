//-----------------------------------------------------------------------------
// File: animation.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 t0 : TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;
	Out.tc0 = t0;

	return Out;
}

[Fragment shader]

sampler2D boneAnimation;

float4 time_interp;
float XBias;
float iBoneAnimationHeight;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	float4 a0, a1;
	float2 tc0, tc1;
	
	tc0 = float2(t0.x + XBias, (time_interp.x)*iBoneAnimationHeight); // get the four animation matrix elements of t0 frame.
	tc1 = float2(tc0.x + XBias, (time_interp.y)*iBoneAnimationHeight); // get the four animation matrix elements of t1 frame.
	a0 = tex2D(boneAnimation, tc0);
	a1 = tex2D(boneAnimation, tc1);
	
	return lerp(a0, a1, time_interp.z); // the four animation matrix elements of current frame.
}

