/*
  Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
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
SAMPLER2D Base;
VsOut mainVS(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
    	Out.Color=SAMPLE2D(Base, In.texCoord).xyz;
	return Out;
}

[Fragment shader]
float4 mainPS(VsOut IN): COLOR {
	return float4(IN.Color, 1);
}

