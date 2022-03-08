//--------------------------------------------------------------------------------------
// File: VerticalButterfly.shd
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

float fPassNum;     // fPassNum corresponding row in butterfly map

sampler2D sampButterfly;
sampler2D sampSourceImage;

//--------------------------------------------------------------------------------------
// Pixel shader for vertical butterfly pass
//--------------------------------------------------------------------------------------
float4 main( float2 srcLocation : TEXCOORD0 ) : COLOR
{
    float2 sampleCoord;
    float4 butterflyVal;
    float2 a, b, w;
    float temp;

    sampleCoord.x = srcLocation.y;
    sampleCoord.y = fPassNum;

    butterflyVal = tex2D( sampButterfly, sampleCoord);
    w = butterflyVal.ba;

    // Sample location A
    sampleCoord.y = butterflyVal.y;
    sampleCoord.x = srcLocation.x;
    a = tex2D(sampSourceImage, sampleCoord).ra;

    // Sample location B
    sampleCoord.y = abs(butterflyVal.x);
    sampleCoord.x = srcLocation.x;
    b = tex2D(sampSourceImage, sampleCoord).ra;

    // Multiply w*b (complex numbers) 
    temp = w.x*b.x - w.y*b.y;
    b.y = w.y*b.x + w.x*b.y;
    b.x = temp;

    // Perform a + w*b or a - w*b
    a = a + ((butterflyVal.x < 0.0) ? -b : b);

    // Make it a 4 component output for good measure
    return a.xxxy;
}

