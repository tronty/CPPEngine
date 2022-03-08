[Vertex shader]
struct VsIn {
float3 position: POSITION;
float4 color : TEXCOORD0;
};
struct VsOut {
float4 position: POSITION;
float4 color : TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
		Out.position = mul(float4(In.position, 1), worldViewProj);
	Out.color = In.color;
	Out.color.w = 1.0;
		return Out;
	}
[Fragment shader]
struct VsOut {
float4 position: POSITION;
float4 color : TEXCOORD0;
};
float4 main(VsOut In): COLOR{
		return In.color;
	}
