/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

Texture2D    g_txDiffuse : register( t0 );
SamplerState g_samLinear : register( s0 );

struct VS_INPUT
{
    float4 Position     : POSITION;
    float2 TextureUV    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float4 Diffuse      : COLOR0;
    float2 TextureUV    : TEXCOORD0;
};

float4 GetPixelColor_(Texture2D<float4> atex, float2 aTexCoord)
{
	uint width_;
	uint height_;
	atex.GetDimensions(width_, height_);
	float2 dim_;
	dim_.x=float(width_-1);
	dim_.y=float(height_-1);
	return atex.Load(int3(dim_.x*(aTexCoord.x+1.0)*0.5, dim_.y*(aTexCoord.y+1.0)*0.5, 0));
}
#define SAMPLE2D(TEX, TEXCOORD) GetPixelColor_(TEX, TEXCOORD)
#define SAMPLER2D Texture2D<float4>

VS_OUTPUT RenderSceneVS( VS_INPUT input )
{
    VS_OUTPUT Output;
    Output.Position = input.Position;
#if 0
    Output.Diffuse.rgba = float4(0,0,1,1);
#else
    Output.Diffuse=SAMPLE2D(g_txDiffuse, In.TextureUV);
#endif
    Output.TextureUV = input.TextureUV;
    return Output;
}

float4 RenderScenePS( VS_OUTPUT In ) : SV_TARGET
{
#if 0
    return g_txDiffuse.Sample( g_samLinear, In.TextureUV ) * In.Diffuse;
#elif 1
    return In.Diffuse;
#elif 0
	return float4(0,0,1,1);
#endif
}

