struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
struct VSMotionBlurIn_Warrior
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};
struct VSSkinnedSceneIn_Warrior
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float3 Tan : TANGENT;
    uint4 Bones : BONES;
    float4 Weights : WEIGHTS;
};
struct VSInstIn_Tree
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXTURE0;
    float4x4 mTransform;
};

struct VSSceneIn_Tree
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXTURE0;
};
struct VSSceneIn_Dwarf
{
    float3 pos          : POSITION;    
    float3 norm         : NORMAL;  
    float2 tex          : TEXTURE0;
};
struct VSSkinnedIn_Soldier
{
    float3 Pos	: POSITION;			//Position
    float4 Weights : WEIGHTS;		//Bone weights
    uint4  Bones : BONES;			//Bone indices
    float3 Norm : NORMAL;			//Normal
    float2 Tex	: TEXCOORD;		    //Texture coordinate
    float3 Tan : TANGENT;		    //Normalized Tangent vector
};
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
VS_OUTPUT main_Soldier(VSSkinnedIn_Soldier IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Pos;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal=IN.norm;
	Out.uv=IN.Tex;
	return Out;
}
VS_OUTPUT main_Tree2(VSInstIn_Tree IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.pos;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal=IN.norm;
	Out.uv=IN.tex;
	return Out;
}
VS_OUTPUT main_Tree(VSSceneIn_Tree IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.pos;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal=IN.norm;
	Out.uv=IN.tex;
	return Out;
}
VS_OUTPUT main_Dwarf(VSSceneIn_Dwarf IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.pos;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal=IN.norm;
	Out.uv=IN.tex;
	return Out;
}
VS_OUTPUT main_Warrior(VSMotionBlurIn_Warrior IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Pos;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal=IN.Normal;
	Out.uv=IN.Tex;
	return Out;
}
VS_OUTPUT main_Warrior2(VSSkinnedSceneIn_Warrior IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Pos;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
	//Out.Normal=IN.Normal;
	Out.uv=IN.Tex;
	return Out;
}
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 objPos = IN.Position;
	Out.Position = mul( WorldViewProjection, float4( objPos, 1.0));
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

