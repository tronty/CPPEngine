/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn {
	float2 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
	float3 Color: TEXCOORD1;
};

[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;

#if 1
SAMPLER2D tex;
#else
#if defined(__HLSL4__) || defined(__HLSL5__)
#if 0
Texture2D<float4> tex;
#else
Texture2D tex;
#endif
SamplerState SamplerLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState SamplerPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
#endif
#endif
VsOut mainVS(VsIn In){
	VsOut Out=(VsOut)0;
	//Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.position = float4(In.position.x, In.position.y, 0.0, 1.0);
    	#if 0//defined(__HLSL4__) || defined(__HLSL5__)
    	//Out.Color=tex.SampleLevel(SamplerLinear, In.texCoord, 0).xyz;
    	Out.Color=tex.SampleLevel(SamplerPoint, In.texCoord, 0).xyz; 
	#else
	#if 0
	float2 position2 = 0.5*(In.position.xy+1.0);
    	Out.Color=SAMPLE2D(tex, position2).xyz; 
	#else
    	Out.Color=SAMPLE2D(tex, In.texCoord).xyz;
	#endif
	#endif
/*
VertexShader latest profile = arbvp1:
Cg compiler output...
(30) : error C3004: function "float4 tex2Dbias(sampler2D, float4);" not supported in this profile
(30) : error C3004: function "float4 tex2Dlod(sampler2D, float4);" not supported in this profile
(30) : error C3004: function "float4 tex2D(sampler2D, float2);" not supported in this profile
CG_ERROR
*/
#if 1
	Out.texCoord=In.texCoord;
#else
	#define SIZE_X 64
	#define SIZE_Y 64
	float2 halfPixel=float2(0.5f / SIZE_X, 0.5f / SIZE_Y));
	Out.texCoord = In.position.xy * float2(0.5, -0.5) + 0.5 + halfPixel;
#endif
	return Out;
}

[Fragment shader]
//sampler2D DMAP;
float4 mainPS(VsOut IN): COLOR {
	float4 Out;
	//Out = tex2D(DMAP, IN.texCoord.xy);
	Out = float4(IN.Color, 1);
	return Out;
}

