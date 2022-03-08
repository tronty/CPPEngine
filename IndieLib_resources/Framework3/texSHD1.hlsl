[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
struct VS_INPUT
{
	float3 position  : POSITION;
	float2 texcoord0 : TEXCOORD0;
};
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float2 texcoord0 : TEXCOORD0;
};
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	float4 v=float4(IN.position, 1);
	OUT.hposition = mul( worldViewProj ,v);
    OUT.texcoord0 = IN.texcoord0;
    return OUT;
}
[Fragment shader]
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float2 texcoord0 : TEXCOORD0;
};
sampler2D Sampler;
float4 colorRGBA=float4(1,1,1,1);
float4 main(VS_OUTPUT IN) : COLOR
{
	return tex2D(Sampler,IN.texcoord0);
}

float4 main2( VS_OUTPUT IN ) : COLOR0
{
	float4 texcol = tex2D( Sampler, IN.texcoord0 )*colorRGBA;
	texcol.a=1;

	texcol=GammaCorrect4(texcol);
	return texcol;
}

