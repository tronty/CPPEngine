[Vertex shader]
struct VsIn {
float3 position: POSITION;
};
struct VsOut {
float4 position: POSITION;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
		Out.position = mul(worldViewProj, float4(In.position.xyz, 1));
		Out.position.w= WSIGN 1;
		return Out;
	}
[Fragment shader]
struct VsOut {
float4 position: POSITION;
};
float4 colorRGBA=float4(1,1,1,1);
float4 main(VsOut In): COLOR{
		return colorRGBA;
	}
