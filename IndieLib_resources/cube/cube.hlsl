[Vertex shader]
struct VsIn {
	float3 position: POSITION;
	//float3 normal: NORMAL;
	//float4 color:  COLOR0;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	//float4 color:  COLOR0;
	float2 texCoord: TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, In.position.z, 1.0));
	//Out.color = In.color;
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	//float4 color:  COLOR0;
	float2 texCoord: TEXCOORD0;
};
sampler2D Base;
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord);//*IN.color;
}

