//--------------------------------------------------------------------------------------
// File: WaterSpectrum.shd
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

float g_fTime;

sampler2D amplitudeSamp;
sampler2D freqSamp;

//--------------------------------------------------------------------------------------
// Image-space pixel shader to evaluate the wave amplitude at g_fTime
//--------------------------------------------------------------------------------------
float4 main( float2 Tex : TEXCOORD0 ) : COLOR0
{
    // These float2s are complex numbers with the real part in .x and the imaginary part in .y
    float2 plus, minus;

    float4 amp = tex2D(amplitudeSamp, Tex);   // Amplitude (two complex numbers packed into xy and zw)
    float    w = tex2D(freqSamp, Tex).x;      // Angular frequency

    plus.x  = amp.x * cos( w * g_fTime) - amp.y * sin( w * g_fTime);
    plus.y  = amp.x * sin( w * g_fTime) + amp.y * cos( w * g_fTime);
    minus.x = amp.z * cos(-w * g_fTime) + amp.w * sin(-w * g_fTime);
    minus.y = amp.z * sin(-w * g_fTime) - amp.w * cos(-w * g_fTime);

    // Return the complex sum of the plus and minus waves to get the total wave amplitude at g_fTime
    float2 val = plus + minus;
    return float4(val.x, 0.0f, 0.0f, val.y);
}
