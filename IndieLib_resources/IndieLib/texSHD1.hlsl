[Vertex shader]
//IndieLib HLSLShaders texSHD1
struct VsIn {
	float3 position: POSITION;
	float4 color: TEXCOORD;
};
struct VsOut {
	float4 position: POSITION;
	float4 color: TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
int bbb=0;
VsOut main(VsIn In){
	VsOut Out;
   if(bbb==0)
	Out.position = mul(  worldViewProj,float4(In.position, 1));
	else
	Out.position = mul(  float4(In.position, 1),worldViewProj);
	Out.color=In.color;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float4 color: TEXCOORD0;
};
float4 main(VsOut IN): COLOR {
	return IN.color;
}

