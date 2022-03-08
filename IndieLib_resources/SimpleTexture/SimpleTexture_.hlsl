/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn {
	float3 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, In.position.z, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]
sampler2D Base;
float4 color=float4(0,1,0,1);
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord)*color;
	//return tex2D(Base, IN.texCoord);//*color
}


