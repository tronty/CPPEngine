//-----------------------------------------------------------------------------
// File: batchAnimation.shd
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

sampler2D boneAnimation ;
sampler2D trans ;

float4 time_interp;
float XBias;
float iBoneAnimationHeight;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	float4 a0, a1;
	float2 tc0, tc1;
	float4 time;
	float4 BM;

	time = tex2D(trans, float2(0.775f, t0.y));
	tc0 = float2(t0.x + XBias, (time.x)*iBoneAnimationHeight); // get the four animation matrix elements of t0 frame.
	tc1 = float2(tc0.x + XBias, (time.y)*iBoneAnimationHeight); // get the four animation matrix elements of t1 frame.

	a0 = tex2D(boneAnimation, tc0);
	a1 = tex2D(boneAnimation, tc1);
	BM = lerp(a0, a1, time.z); // BM is the four animation matrix elements of current frame.

#if 0
	float3x4 M;
#else
	float4x4 M;
#endif

	M[0] = tex2D(trans, float2(0.025f, t0.y)) * float4(time.w, 1.0f, 1.0f, 1.0f); // get first row of translation matrix.
	M[1] = tex2D(trans, float2(0.275f, t0.y)) * float4(1.0f, time.w, 1.0f, 1.0f); // get second row of translation matrix.
	M[2] = tex2D(trans, float2(0.525f, t0.y)) * float4(1.0f, 1.0f, time.w, 1.0f); // get third row of translation matrix.
	float3 v3=mul(M, BM).xyz;

	return float4(v3, 1.0f); // translate the four animation matrix elements of current frame.
}

