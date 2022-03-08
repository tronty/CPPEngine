[Vertex shader]
struct VsIn {
	float4 position: POSITION;
	float4 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float4 texCoord: TEXCOORD0;
};
ROW_MAJOR float4x4 mvp MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(mvp, In.position);
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float4 texCoord: TEXCOORD0;
};
float4 main(VsOut IN): COLOR {
	return IN.texCoord;
}

	
