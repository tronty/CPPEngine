/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]
struct VsIn {
	float3 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
		Out.position = mul(worldViewProj, float4(In.position.xyz, 1));
		Out.position.w= WSIGN 1;
	Out.texCoord = In.texCoord;
		return Out;
	}
[Fragment shader]
struct VsOut {
float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
float4 colorRGBA=float4(1,1,1,1);
sampler2D Base;
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord);
}


