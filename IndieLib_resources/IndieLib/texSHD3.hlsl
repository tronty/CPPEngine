[Vertex shader]
//IndieLib HLSLShaders texSHD3
struct VsIn {
	float3 position: POSITION;
	float2 texCoord: TEXCOORD;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
int bbb=0;
VsOut main(VsIn In){
	VsOut Out;
   if(bbb==0)
	Out.position = mul(  worldViewProj,float4(In.position, 1));
	else
	Out.position = mul(  float4(In.position, 1),worldViewProj);
	#if 0
	Out.texCoord=float2(In.texCoord.x, 1.0-In.texCoord.y);
	#else
	Out.texCoord=In.texCoord;
	#endif
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD;
};
sampler2D Sampler;
float4 colorRGBA=float4(1,1,1,1);
float4 main(VsOut IN): COLOR {
	return tex2D(Sampler, IN.texCoord);//*colorRGBA;
}

