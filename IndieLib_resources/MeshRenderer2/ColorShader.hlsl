/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]
struct VsIn
{
	float3 position : POSITION;
#if 0
	float3 Normal : NORMAL;
#else
	float3 Normal : TEXCOORD0;
#endif
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 Color : TEXCOORD1;
	float2 uv : TEXCOORD2;
};
struct VsOut {
	float4 position: POSITION;
	float3 Color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
VsOut main(VsIn In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(WorldViewProjection, float4(In.position.xyz, 1));
	Out.Color = In.Normal;
	//Out.Color = In.Color;
	Out.uv = In.uv;
	return Out;
}
[Fragment shader]
struct VsOut
{
	float4 position: POSITION;
	float3 Color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
//float4 DiffuseColor=float4(1,1,1,1);
//sampler2D DIFFUSE_SAMPLER;
float4 main(VsOut IN): COLOR 
{
	//return float4(0,1,0,1);
	return float4(IN.Color, 1);//*tex2D(DIFFUSE_SAMPLER, IN.uv);
}

