struct VS_OUTPUT
{
	float4 Position : POSITION;
	//float3 Color : TEXCOORD0;
	float2 Tex : TEXCOORD1;
};
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 Color : TEXCOORD0;
	float2 Tex : TEXCOORD1;
};
struct VS_Soldier
{
    float3 Position : POSITION;
    float4 Weights : TEXCOORD1;
    uint4 Bones2 : TEXCOORD2;
    uint4 Bones3 : TEXCOORD3;
    uint4 Bones4 : TEXCOORD4;
    uint4 Bones5 : TEXCOORD5;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;

};
struct VS_Warrior
{
    float3 Position          : POSITION;    
    float3 Normal         : NORMAL;  
    float2 Tex          : TEXCOORD0;
    float3 Tan : TANGENT;
    uint4 Bones : TEXCOORD1;
    float4 Weights : TEXCOORD2;
};
struct VS_Tree
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXTURE0;
};
struct VS_Dwarf
{
    float3 Position          : POSITION;    
    float3 Normal         : NORMAL;  
    float2 Tex          : TEXTURE0;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( float4( objPos, 1.0), WorldViewProjection);
	Out.Tex=IN.Tex;
	//Out.Color=IN.Color;
	return Out;
}
VS_OUTPUT main_Soldier(VS_Soldier IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal= IN.Normal;
	Out.Tex=IN.Tex;
	return Out;
}
VS_OUTPUT main_Tree(VS_Tree IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal= IN.Normal;
	Out.Tex=IN.Tex;
	return Out;
}
VS_OUTPUT main_Dwarf(VS_Dwarf IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal= IN.Normal;
	Out.Tex=IN.Tex;
	return Out;
}
VS_OUTPUT main_Warrior(VS_Warrior IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal=IN.Normal;
	Out.Tex=IN.Tex;
	return Out;
}
[Fragment shader]
sampler2D txDiffuse;
sampler2D txNormal;
sampler2D txSpecular;
float4 main(VS_OUTPUT IN) : COLOR
{
	//float4 texcol = tex2D(txDiffuse, IN.Tex);
	return tex2D(txDiffuse, IN.Tex);
}

