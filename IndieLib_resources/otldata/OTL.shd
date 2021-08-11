struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 uv : TEXCOORD0;
};
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos, 1), WorldViewProjection);
	Out.uv=IN.uv;
	return Out;
}

[Fragment shader]
sampler2D DIFFUSE_SAMPLER;
float4 main(VS_OUTPUT IN) : COLOR
{
	return tex2D(DIFFUSE_SAMPLER,IN.uv);
}

