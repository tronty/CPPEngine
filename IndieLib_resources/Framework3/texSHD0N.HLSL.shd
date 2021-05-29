[Vertex shader]
//IndieLib HLSLShaders texSHD0
struct VsIn {
	float2 position: POSITION;
	float4 color: NORMAL;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float4 color: NORMAL;
	float2 texCoord: TEXCOORD0;
};
float4 scaleBias=float4(0.003,-0.003,-1,1);
//ROW_MAJOR float4x4 worldViewProj;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
#if 1
	Out.position.xy = In.position.xy * scaleBias.xy + scaleBias.zw;
#else
	float4 v=float4( In.position.x,
			       In.position.y,
					   In.position.z,
					   1.0f );
	Out.position = mul( worldViewProj ,v);
#endif
	Out.position.w= WSIGN 1;
	Out.texCoord=In.texCoord;
	Out.color = In.color;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
float4 color: NORMAL;
	float2 texCoord: TEXCOORD0;
};
sampler2D Base;
float4 colorRGBA=float4(1,1,1,1);
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord)* IN.color;
}

