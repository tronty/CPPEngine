[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( float4( objPos, 1.0), WorldViewProjection);
	Out.uv=IN.uv;
	Out.color=IN.color;
	//color.uv=color.uv;
	return Out;
}
[Fragment shader]
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
sampler2D DIFFUSE_SAMPLER;
float4 main(VS_OUTPUT IN) : COLOR
{
	//float4 texcol = tex2D(DIFFUSE_SAMPLER,IN.uv);
	return tex2D(DIFFUSE_SAMPLER,IN.uv);
}
float4 main2(VS_OUTPUT IN) : COLOR
{
	//float4 texcol = tex2D(DIFFUSE_SAMPLER,IN.uv);
	return tex2D(DIFFUSE_SAMPLER,IN.uv)+float4(IN.color, 1);
}

