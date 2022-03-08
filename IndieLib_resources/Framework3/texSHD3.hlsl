[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
float4x4 World;
float4 vecLightDir;
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
};
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos, 1), worldViewProj);
	Out.Light = vecLightDir.xyz;
	Out.Norm = normalize(mul(float4(IN.Normal, 1), World)).xyz;
	return Out;
}

[Fragment shader]
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
};
float4 AmbientColor;
float4 DiffuseColor;
float4 main(VS_OUTPUT IN) : COLOR
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);//AmbientColor + DiffuseColor * saturate(dot(IN.Light, IN.Norm));
}

