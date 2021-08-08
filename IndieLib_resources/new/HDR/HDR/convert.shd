//-----------------------------------------------------------------------------
// File: convert.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





/*
	This shader does FP16 to I16 conversion. This allows us to use a texture filter in later passes.
	We're using a fairly moderate range, so simple scaling will work fine. In other cases an
	RGBS or RGBE encoding may be needed. We're also downsampling the texture one size while we're at it.
*/

[Vertex shader]

float2 halfPixel;

// Define vertex shader outputs
struct VsOut {
	float4 Pos:			POSITION;
	float2 texCoord0:	TEXCOORD0;
	float2 texCoord1:	TEXCOORD1;
	float2 texCoord2:	TEXCOORD2;
	float2 texCoord3:	TEXCOORD3;
};

VsOut main(float4 pos: POSITION, float2 texCoord: TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;

	// Texture coordinates
	Out.texCoord0 = texCoord + float2(-1.0,  1.0) * halfPixel;
	Out.texCoord1 = texCoord + float2( 1.0,  1.0) * halfPixel;
	Out.texCoord2 = texCoord + float2( 1.0, -1.0) * halfPixel;
	Out.texCoord3 = texCoord + float2(-1.0, -1.0) * halfPixel;
	
	return Out;
}


[Fragment shader]


sampler2D	RT;

float4 main(float2 texCoord0: TEXCOORD0, float2 texCoord1: TEXCOORD1,
			float2 texCoord2: TEXCOORD2, float2 texCoord3: TEXCOORD3) : COLOR {

	float4 rt  = tex2D(RT, texCoord0);
	       rt += tex2D(RT, texCoord1);
	       rt += tex2D(RT, texCoord2);
	       rt += tex2D(RT, texCoord3);

	rt.rgb /= (4.0 * 128.0);

	return rt;
}
