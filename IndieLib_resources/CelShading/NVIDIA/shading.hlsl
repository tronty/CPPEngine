/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
struct VS_OUTPUT
{
	float4 Pos: POSITION;
	float3 Normal0 : NORMAL;
	float3 Normal1 : TEXCOORD0;
};
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float3 lightPos = float3(60, 10, 85);

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
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection); // transform Position
	Out.Normal1 = lightPos - IN.Pos;
	Out.Normal0 = IN.Normal;
	return Out;
}

[Fragment shader]
sampler1D CelShade;
float4 main(VS_OUTPUT IN) : COLOR
{
	float3 lightVec = normalize(IN.Normal1);
	float diffuse = dot(lightVec, IN.Normal0);
	return tex1D(CelShade, diffuse);
}
