//--------------------------------------------------------------------------------------
// File: Expand.shd
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

float    g_heightScale;          // wave height scale
float    g_repeatRate;

sampler2D sampSourceImage;

//--------------------------------------------------------------------------------------
// Pixel shader to expand height map values 3D coordinates
//--------------------------------------------------------------------------------------
float4 main( float2 Tex : TEXCOORD0 ) : COLOR0
{
    float4 vertex;
    float2 xyPos;

    xyPos = (Tex * 2.1f) - 1.0f;
    vertex.x = -xyPos.x;
    // scale the height to compensate for the IFFT
    vertex.y = abs(tex2D(sampSourceImage, Tex * g_repeatRate).r) * g_heightScale;
    vertex.z = xyPos.y;
    vertex.w = 1;
    return vertex;
}

