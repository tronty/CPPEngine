//--------------------------------------------------------------------------------------
// File: VerticalScramble.shd
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

sampler1D sampScramble;
sampler2D sampSourceImage;

///--------------------------------------------------------------------------------------
// Pixel shader for vertical scramble
//--------------------------------------------------------------------------------------
float4 main( float2 imagePos : TEXCOORD0 ) : COLOR
{
    float2 fromPos = imagePos;

    fromPos.y = tex1D(sampScramble, imagePos.y).x;   // scramble the y coordinate

    return tex2D(sampSourceImage, fromPos);
}

