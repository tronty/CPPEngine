struct VS_OUTPUT
{
	float4 Position : POSITION;
	//float3 Color : TEXCOORD1;
	float2 Tex : TEXCOORD0;
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
#if 0
struct VS_Soldier52
{
    float3 Position : POSITION;
    float2 Tex : TEXCOORD0;
    uint4 Bones : TEXCOORD1;
    float4 Weights : TEXCOORD2;
};
struct VS_Soldier64
{
    float3 Position : POSITION;  
    float2 Tex : TEXCOORD0;
    float4 Transform1 : TEXCOORD1;
    float4 Transform2 : T3EXCOORD2;
    float3 m3 : TEXCOORD3;
};
#define VS_Soldier VS_Soldier52
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
#else
#define VS_Soldier VS_INPUT
#define VS_Warrior VS_INPUT
#define VS_Tree VS_INPUT
#define VS_Dwarf VS_INPUT
#endif
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));;
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
	//return float4(1,0,0,1);
	float4 diffuse=tex2D(txDiffuse, IN.Tex);
	float4 normal=float4(0,0,0,1);
	float4 specular=float4(0,0,0,1);
	#if 0
	normal=tex2D(txNormal, IN.Tex);
	specular=tex2D(txSpecular, IN.Tex);
	#endif
	return diffuse+normal+specular;
}


