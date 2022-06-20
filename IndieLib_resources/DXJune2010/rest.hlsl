struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
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
struct VS_INPUT2
{
    float3 Position	: POSITION;			//Position
    float4 color : TEXCOORD2;//WEIGHTS;		//Bone weights
    float4  Bones : TEXCOORD1;//BONES;			//Bone indices
    float3 Normal : NORMAL;			//Normal
    float2 uv	: TEXCOORD0;		    //Texture coordinate
    float3 Tangent : TANGENT;		    //Normalized Tangent vector
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
sampler2D txDiffuse;
sampler2D txNormal;
sampler2D txSpecular;
float4 main(VS_OUTPUT IN) : COLOR
{
	//float4 texcol = tex2D(txDiffuse, IN.uv);
	return tex2D(txDiffuse, IN.uv);
}

