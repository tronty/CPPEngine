//--------------------------------------------------------------------------------------
// File: Copy.shd
// 
// Copyright (c) ATI Research 2006
//--------------------------------------------------------------------------------------
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

sampler2D sampSourceImage;

//--------------------------------------------------------------------------------------
// Simple shader to copy texture
//--------------------------------------------------------------------------------------
float4 main( float2 Tex : TEXCOORD0 ) : COLOR0
{
    return tex2D(sampSourceImage, Tex);
}
