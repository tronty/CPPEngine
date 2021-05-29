[Vertex shader]
//IndieLib HLSLShaders texSHD0
struct VsIn {
	float2 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
float4 scaleBias=float4(0.003,-0.003,-1,1);
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position.xy = In.position.xy * scaleBias.xy + scaleBias.zw;
	Out.position.w= WSIGN 1;
	Out.texCoord=In.texCoord;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
sampler2D Base;
float4 colorRGBA=float4(1,1,1,1);
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord)* colorRGBA;
}

