// https://www.gamasutra.com/view/feature/2866/implementing_lighting_models_with_.php?print=1
// Implementing Lighting Models With HLSL
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
	//color.uv=color.uv;
	Out.uv=IN.uv;
	return Out;
}

[Fragment shader]
sampler2D DIFFUSE_SAMPLER;
float4 AmbientColor = float4( 0.75, 0.75, 0.75, 1.0);
float4 main(VS_OUTPUT IN) : COLOR
{
	float4 texcol = clamp(tex2D(DIFFUSE_SAMPLER,IN.uv), 0.75, 1.0);
	return texcol*AmbientColor;
}

