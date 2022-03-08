/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
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
#if 1
struct VsOutTerrain {
	float4 generateTerrain_pos: POSITION;
	//float4 generateTerrain_pos_: TEXCOORD0;
	float2 generateTerrain_uv: TEXCOORD0;
	#if 0
	float4 tangentSpace_pos: POSITION;
	float2 tangentSpace_uv0: TEXCOORD3;
	float2 tangentSpace_uv1: TEXCOORD4;
	float2 tangentSpace_uv2: TEXCOORD5;
	float2 tangentSpace_uv3: TEXCOORD6;
	float2 tangentSpace_uv4: TEXCOORD7;
	#endif
	#if 0
	//float4 terrain_pos      : POSITION;
	float2 terrain_uv: TEXCOORD2;
	float3 terrain_normal   : TEXCOORD3;
	float  terrain_fog      : TEXCOORD4;
	float2 terrain_scale    : TEXCOORD5;
	#endif
};
#endif
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut mainVS(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}
#if 0
VsOutTerrain mainTerrainVS(VsIn In){
	VsOutTerrain=(VsOutTerrain)0;
#if 1
	Out.generateTerrain_pos = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	//Out.generateTerrain_pos_ = Out.generateTerrain_pos;
	Out.generateTerrain_uv = In.texCoord;
#endif
#if 0
	/*
	//Out.tangentSpace_pos = Out.generateTerrain_pos;
	Out.tangentSpace_uv0 = In.texCoord;
	Out.tangentSpace_uv1 = In.texCoord;
	Out.tangentSpace_uv2 = In.texCoord;
	Out.tangentSpace_uv3 = In.texCoord;
	Out.tangentSpace_uv4 = In.texCoord;
	*/
	//Out.terrain_pos = Out.generateTerrain_pos;
	Out.terrain_uv= In.texCoord;
	#if 0
	Out.terrain_normal   : TEXCOORD3;
	Out.terrain_fog      : TEXCOORD4;
	Out.terrain_scale    : TEXCOORD5;
	#endif	
#endif
	return Out;
}
#endif
[Fragment shader]

sampler2D Base;
float4 mainPS(VsOut IN): COLOR {
	//return float4(1,1,1,1);
	return tex2D(Base, IN.texCoord);
}

float4 mainTerrainPS(VsOutTerrain IN): COLOR {
	//return float4(1,1,1,1);
	return tex2D(Base, IN.generateTerrain_uv);
}
#if 0
float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;	
	float2 p = fragCoord.xy/iResolution.xy-0.5;
    float2 q = fragCoord.xy/iResolution.xy;
	p.x*=iResolution.x/iResolution.y;	
#endif

