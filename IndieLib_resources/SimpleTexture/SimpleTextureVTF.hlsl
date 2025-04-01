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
float pixel=0.001953;
float halfPixel=0.000977;
VsOut mainVS(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
    	Out.Color=SAMPLE2D(Base, In.texCoord).xyz;
	return Out;
}

VsOut mainVS2(float2 position: POSITION){
	VsOut Out=(VsOut)0;
	Out.position = float4(position.x, position.y, 0, 1);
	Out.texCoord = position.xy * float2(0.5, -0.5) + 0.5 + halfPixel;
    	Out.Color=SAMPLE2D(Base, Out.texCoord).xyz;
	return Out;
}

[Fragment shader]
float4 mainPS(VsOut IN): COLOR {
	return float4(IN.Color, 1);
}

