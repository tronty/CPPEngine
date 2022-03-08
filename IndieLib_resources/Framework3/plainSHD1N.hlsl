[Vertex shader]
//IndieLib HLSLShaders plainSHD1
float4 scaleBias=float4(0.003,-0.003,-1,1);
//float4 translation=float4(0,0,0,1);
struct VsIn {
float2 position: POSITION;
float4 color: NORMAL;
};
struct VsOut {
float4 position: POSITION;
float4 color: NORMAL;
};
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
		Out.position.xy = In.position.xy * scaleBias.xy + scaleBias.zw;
//	Out.position.z=0;
//		//Out.position.xyz = Out.position.xyz + translation.xyz;
		Out.position.w= WSIGN 1;
	Out.color = In.color;
		return Out;
	}
[Fragment shader]
struct VsOut {
float4 position: POSITION;
float4 color: NORMAL;
};
float4 colorRGBA=float4(1,1,1,1);
float4 main(VsOut In): COLOR{
		return In.color;
	}



