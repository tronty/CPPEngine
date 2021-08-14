[Vertex shader]
struct VsIn {
	float3 Position_b: POSITION;
	float3 Color_b: NORMAL;
};
struct VsOut {
	float4 position: POSITION;
	float3 Color: NORMAL;
};
ROW_MAJOR float4x4 MVP MVPSEMANTIC;
VsOut main(VsIn In)
{
	VsOut Out=(VsOut)0;
	float4 p=float4(In.Position_b, 1);
	Out.position = mul(MVP , p);
	Out.Color = In.Color_b;
	return Out;
}

[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float3 Color: NORMAL;
};
float4 main(VsOut IN): COLOR
{
	return float4(IN.Color, 1);
}

