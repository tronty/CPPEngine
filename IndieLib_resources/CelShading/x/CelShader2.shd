struct VS_OUTPUT
{
	float4 Pos: POSITION;
	float3 Normal : NORMAL;
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
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection);
	Out.Normal = mul(float4(IN.Normal,1), WorldViewProjection);
	return Out;
}

[Fragment shader]
float3 lightDir=float3((10.0, 5.0, 0.0);
float4 main(VS_OUTPUT IN) : COLOR
{
	float intensity;
	float4 color;
	intensity = dot(lightDir,normalize(normal));

	if (intensity > 0.95)
		color = float4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = float4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = float4(0.4,0.2,0.2,1.0);
	else
		color = float4(0.2,0.1,0.1,1.0);
	return color;
}
