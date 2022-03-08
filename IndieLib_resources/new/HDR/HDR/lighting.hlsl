//-----------------------------------------------------------------------------
// File: lighting.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





[Vertex shader]

// Define vertex shader outputs
struct VsOut {
	float4 Pos:			POSITION;
	float2 texCoord0:	TEXCOORD0;
	float2 texCoord1:	TEXCOORD1;
};

// ModelViewProjection matrix
float4x4 mvp;	

VsOut main(float4 pos: POSITION, float2 textureCoord0: TEXCOORD0, float2 textureCoord1: TEXCOORD1){
	
	VsOut Out;

	// Transform local space position with mvp
	Out.Pos = mul(mvp, pos);
	
	// Texture coordinates
	Out.texCoord0 = textureCoord0;
	Out.texCoord1 = textureCoord1;
	
	return Out;
}
	

[Fragment shader]

sampler2D	Base;
sampler2D	LightMap;
float		maxValue;

float4 main(float2 texCoord0: TEXCOORD0, float2 texCoord1: TEXCOORD1) : COLOR {
	
	// Sample base texture
	float4 base = tex2D(Base, texCoord0);
	
	// Sample lightmap
	float4 light = tex2D(LightMap, texCoord1);
	
	// Expand fixed point lightmap to floating point
	light.rgb *= light.a * maxValue;
	
	return (light + 0.1) * base;
}
