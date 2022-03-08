[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float3 color	: TEXCOORD0;
	float2 texcoord : TEXCOORD1;
};
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float3 normal	: NORMAL;
	float3 color	: TEXCOORD0;
	float2 texcoord : TEXCOORD1;
};
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	float4 v=float4(IN.position, 1);
	OUT.hposition = mul( worldViewProj ,v);
	OUT.normal = IN.normal;
	OUT.color = IN.color;
    	OUT.texcoord = IN.texcoord;
    return OUT;
}
[Fragment shader]
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float3 normal	: NORMAL;
	float3 color	: TEXCOORD0;
	float2 texcoord : TEXCOORD1;
};
struct PS_OUTPUT
{
	float4 color : COLOR;
};
sampler2D Base;
PS_OUTPUT main( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;
	OUT.color = float4(IN.color, 1)*tex2D( Base, IN.texcoord );

	OUT.color=GammaCorrect4(OUT.color);
	return OUT;
}
