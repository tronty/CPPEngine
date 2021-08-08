/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
struct VS_OUTPUT
{
	float4 Pos: POSITION;
};
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float3 camPos = float3(60, 10, 85);
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal0 : NORMAL;
	float3 Normal1 : TEXCOORD0;
};
float edgeThreshold_ = 71;

#if 1
float outlineThreshold=0.0;
float edgeThreshold=edgeThreshold_ * 0.01 * 2 - 1;
#else
float outlineThreshold=-100000000.0;
float edgeThreshold=-1.0;
#endif

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection); // transform Position

    float3 dir = camPos - IN.Pos;

	Out.Pos.w = float(
		dot(dir, IN.Normal0.xyz) * dot(dir, IN.Normal1.xyz) < outlineThreshold ||
		dot(IN.Normal0.xyz, IN.Normal1.xyz) < edgeThreshold);

	return Out;
}

[Fragment shader]
float4 main(VS_OUTPUT IN) : COLOR
{
	return float4(0,0,0,0);
}

