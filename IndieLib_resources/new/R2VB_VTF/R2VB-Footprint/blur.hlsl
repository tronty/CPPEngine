//-----------------------------------------------------------------------------
// File: blur.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
	float2 tc1: TEXCOORD1;
	float2 tc2: TEXCOORD2;
	float2 tc3: TEXCOORD3;
	float2 tc4: TEXCOORD4;
};

static float2 offset[4] = { float2(0.5f/512.0f, 0.5f/512.0f), float2(-0.5f/512.0f, -0.5f/512.0f), float2(-0.5f/512.0f, 0.5f/512.0f), float2(0.5f/512.0f, -0.5f/512.0f) };

VsOut main(float4 pos: POSITION, float2 t0 : TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;
	Out.tc0 = t0;	
	Out.tc1 = t0+offset[0];
	Out.tc2 = t0+offset[1];
	Out.tc3 = t0+offset[2];
	Out.tc4 = t0+offset[3];

	return Out;
}

[Fragment shader]

sampler2D blur;

float4 main(float2 t0: TEXCOORD0, float2 t1: TEXCOORD1, float2 t2: TEXCOORD2, float2 t3: TEXCOORD3, float2 t4: TEXCOORD4) : COLOR {
	float4 Blur[5];

	Blur[0] = tex2D(blur, t0);
	Blur[1] = tex2D(blur, t1);
	Blur[2] = tex2D(blur, t2);
	Blur[3] = tex2D(blur, t3);
	Blur[4] = tex2D(blur, t4);

	return 0.2f*(Blur[0]+Blur[1]+Blur[2]+Blur[3]+Blur[4]);
}

