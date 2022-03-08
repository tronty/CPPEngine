// https://www.gamasutra.com/view/feature/2866/implementing_lighting_models_with_.php?print=1
// Implementing Lighting Models With HLSL
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
	float3 color : TEXCOORD2;
	float2 uv : TEXCOORD3;
};
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float4x4 World;
float4 vecLightDir = float4( -1.0, 0, 0, 1.0);
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
	Out.Light = vecLightDir.xyz;
	Out.Norm = normalize(mul(float4(IN.Normal, 1), World)).xyz;
	//color.uv=color.uv;
	Out.uv=IN.uv;
	return Out;
}

[Fragment shader]
sampler2D DIFFUSE_SAMPLER;
float4 AmbientColor = float4( 0.075, 0.075, 0.075, 1.0);
float4 DiffuseColor = float4( 0.75, 0.75, 0.75, 1.0);
float4 main(VS_OUTPUT IN) : COLOR
{
	//return float4(1,0,0,0);
	float4 texcol = clamp(tex2D(DIFFUSE_SAMPLER,IN.uv), 0.75, 1.0);
	return texcol*(AmbientColor + DiffuseColor * saturate(dot(IN.Light, IN.Norm)));
}

