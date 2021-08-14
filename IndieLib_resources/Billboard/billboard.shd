[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
struct VS_INPUT
{
	float3 position  : POSITION;
	float3 color 	 : TEXCOORD0;
	float2 texcoord0 : TEXCOORD1;
};
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float3 color 	 : TEXCOORD0;
	float2 texcoord0 : TEXCOORD1;
};
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	float4 v=float4( IN.position.x,
			       IN.position.y,
					   IN.position.z,
					   1.0f );
    OUT.hposition = mul( worldViewProj ,v);
    OUT.color = IN.color;
    OUT.texcoord0 = IN.texcoord0;
    return OUT;
}
[Fragment shader]
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float3 color 	 : TEXCOORD0;
	float2 texcoord0 : TEXCOORD1;
};
sampler2D Sampler;
float4 main( VS_OUTPUT IN ) : COLOR
{
	float4 OUT;
	OUT = tex2D( Sampler, IN.texcoord0 )*float4(IN.color, 1);
	float gamma=1.5;
	OUT.r=pow(abs(OUT.r), 1.0 / gamma);
	OUT.g=pow(abs(OUT.g), 1.0 / gamma);
	OUT.b=pow(abs(OUT.b), 1.0 / gamma);
	return OUT;
}


