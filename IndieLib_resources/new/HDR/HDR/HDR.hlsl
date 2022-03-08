//-----------------------------------------------------------------------------
// File: HDR.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

// Define vertex shader outputs
struct VsOut {
	float4 Pos:			POSITION;
	float2 texCoord:	TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 textureCoord: TEXCOORD0){

	VsOut Out;

	Out.Pos = pos;
	Out.texCoord = textureCoord;
	
	return Out;
}

[Fragment shader]

sampler2D Scene;
sampler2D Blur;

#ifdef GPU_EXPOSURE
sampler2D Exposure;
#else
float exposure;
#endif

float4 main(float2 texCoord: TEXCOORD0) : COLOR {
#ifdef GPU_EXPOSURE
	// Sample exposure
	float exposure = tex2D(Exposure, texCoord).x;
#endif

	// Sample the scene
	float4 scene = tex2D(Scene, texCoord);
	float4 blur  = tex2D(Blur,  texCoord);

	// Expand fixed point HDR results to its full range
	blur.rgb *= 128.0;

	// Compose and reduce to low dynamic range for output
	float4 color = scene + blur;
	color.rgb = 1.0 - exp2(-exposure * color.rgb);

	return color;
}
