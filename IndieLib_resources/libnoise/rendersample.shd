[Vertex shader]
struct VsIn {
	float2 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
sampler2D Base;
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord);
}

