struct VS_INPUT
{
	float3 position  : POSITION;
	float4 color	 : TEXCOORD0;
	float2 texcoord : TEXCOORD1;
};
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float4 color	 : TEXCOORD0;
	float2 texcoord : TEXCOORD1;
};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	float4 v=float4(IN.position, 1);
	OUT.hposition = mul( worldViewProj ,v);
    	OUT.texcoord = IN.texcoord;
	OUT.color = IN.color;
    	return OUT;
}
[Fragment shader]
struct PS_OUTPUT
{
	float4 color : COLOR;
};
sampler2D Sampler;
int iSampler=0;
PS_OUTPUT main( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;
	OUT.color = IN.color;
	if(iSampler>0)
		OUT.color += tex2D( Sampler, IN.texcoord );
	OUT.color=GammaCorrect4(OUT.color);
	return OUT;
}

