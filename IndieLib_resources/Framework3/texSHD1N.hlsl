[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
struct VS_INPUT
{
	float3 position  : POSITION;
	float3 color: NORMAL;
	float2 texcoord0 : TEXCOORD0;
};
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float4 color: NORMAL;
	float2 texcoord0 : TEXCOORD0;
};
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	float4 v=float4(IN.position, 1);
	OUT.hposition = mul( worldViewProj ,v);
    OUT.texcoord0 = IN.texcoord0;
	OUT.color = float4(IN.color, 1);
    return OUT;
}
[Fragment shader]
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float4 color: NORMAL;
	float2 texcoord0 : TEXCOORD0;
};
struct PS_OUTPUT
{
	float4 color : COLOR;
};
sampler2D Base;
PS_OUTPUT main( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;
	OUT.color = tex2D( Base, IN.texcoord0 );//*IN.color;

	OUT.color=GammaCorrect4(OUT.color);
	return OUT;
}

