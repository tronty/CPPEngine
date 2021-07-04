[Vertex shader]
//float4 translation=float4(0,0,0,1);
struct VsIn {
float3 position: POSITION;
float4 color: NORMAL;
};
struct VsOut {
float4 position: POSITION;
float4 color: NORMAL;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
		Out.position = mul(worldViewProj, float4(In.position, 1));
		Out.position.w= WSIGN 1;
	Out.color = In.color;
		return Out;
	}
[Fragment shader]
struct VsOut {
float4 position: POSITION;
float4 color: NORMAL;
};
//float4 colorRGBA=float4(1,1,1,1);
float4 main(VsOut In): COLOR{
		return In.color;
	}
