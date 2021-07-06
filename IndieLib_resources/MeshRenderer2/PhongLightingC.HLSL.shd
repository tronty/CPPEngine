// https://www.gamasutra.com/view/feature/2866/implementing_lighting_models_with_.php?print=1
// Implementing Lighting Models With HLSL
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float4x4 World;
float4 vecLightDir = float4( 0.0f, 1.0f, 0.0f, 1.0f);
float4 vecEye = float4( 0.0f, 0.0f, 1.0f, 1.0f);
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : TEXCOORD0;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD1;
	float2 uv : TEXCOORD2;
};
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
	float3 View : TEXCOORD2;
	float2 uv : TEXCOORD3;
	float3 color : TEXCOORD4;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection);
	Out.Light = vecLightDir.xyz;
	float3 PosWorld = normalize(mul(float4(IN.Pos,1), World)).xyz;
	Out.View = vecEye.xyz - PosWorld;
	Out.Norm = normalize(mul(float4(IN.Normal,1), World)).xyz;
	Out.uv=IN.uv;
	Out.color=IN.color;
	return Out;
}
[Fragment shader]
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
	float3 View : TEXCOORD2;
	float2 uv : TEXCOORD3;
	float3 color : TEXCOORD4;
};

float4 main(VS_OUTPUT IN) : COLOR
{
    float4 AmbientColor = float4(IN.color, 1)*0.1;
    float4 DiffuseColor = float4(IN.color, 1);
	float3 Normal = normalize(IN.Norm);
	float3 LightDir = normalize(IN.Light);
	float3 ViewDir = normalize(IN.View);
	float4 diff = saturate(dot(Normal, LightDir)); // diffuse component
	float3 Reflect = normalize(2 * diff * Normal - LightDir);// R = 2 * (N.L) * N - L
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 8); // R.V^n
	float4 color = AmbientColor + DiffuseColor * diff + specular;
	color=GammaCorrect4(color);
	return color;
}

