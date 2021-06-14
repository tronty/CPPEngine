float4 GetPixelColor_(Texture2D<float4> atex, float2 aTexCoord)
{
	uint width_;
	uint height_;
	atex.GetDimensions(width_, height_);
	float2 dim_;
	dim_.x=float(width_-1);
	dim_.y=float(height_-1);
	float4 col=atex.Load(int3(dim_.x*aTexCoord.x, dim_.y*aTexCoord.y, 0));
	col.w=1;
	return col;
}
#define SAMPLE2D(TEX, TEXCOORD) GetPixelColor_(TEX, TEXCOORD)
#define SAMPLER2D Texture2D<float4>

struct VS_Input {
    float2 pos : POS;
    float2 uv : TEX;
};

struct VS_Output {
    float4 pos : SV_POSITION;
    float4 color : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

#if 1
Texture2D    mytexture : register(t0);
SamplerState mysampler : register(s0);
#elif 0
SAMPLER2D mytexture;
#elif 0
Texture2D    mytexture;
SamplerState g_samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};
#endif

VS_Output vs_main(VS_Input input)
{
    VS_Output output;
    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.uv = input.uv;
    #if 0
    output.color = float4(1,0,0,1);
    #elif 0
    output.color = SAMPLE2D(mytexture, input.uv);
    #elif 0
	//float fOffset)   
    	//float tCoord = (fOffset) / 300.0;
    	output.color = mytexture.SampleLevel( g_samPoint, input.uv, 0 );
    #elif 0
    output.color = mytexture.Sample(mysampler, input.uv);
    #endif
    return output;
}

float4 ps_main(VS_Output input) : SV_Target
{
#if 1
    return mytexture.Sample(mysampler, input.uv);
#else
	return input.color;
#endif
}
