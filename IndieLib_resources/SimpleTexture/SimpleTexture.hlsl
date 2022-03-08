/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
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
};

[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut mainVS(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}

[Fragment shader]

sampler1D Base1;
sampler2D Base;
sampler3D Base3;
int dim=2;
float z=0.5;
float4 main3(VsOut IN): COLOR {
	if(dim==1)
		return tex1D(Base1, IN.texCoord.x);
	else if(dim==3)
		return tex3D(Base3, IN.texCoord.x, IN.texCoord.y, z);
	return tex2D(Base, IN.texCoord);
}
float4 mainPS(VsOut IN): COLOR {
	//return float4(1,1,1,1);
	return tex2D(Base, IN.texCoord);
}

float4 main1D(VsOut IN): COLOR {return tex1D(Base1, IN.texCoord.x);}
float4 main2D(VsOut IN): COLOR {return tex2D(Base,  IN.texCoord.xy);}
float4 main3D(VsOut IN): COLOR {return tex3D(Base3, float3(IN.texCoord.xy, z));}

