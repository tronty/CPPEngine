[Vertex shader]
struct VsIn {
	float3 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
		Out.position = mul(worldViewProj, float4(In.position.xyz, 1));
		Out.position.w= WSIGN 1;
		Out.texCoord = In.texCoord;
		return Out;
	}

[Fragment shader]
struct VsOut {
float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
float4 DiffuseColor=float4(1,1,1,1);
sampler2D DIFFUSE_SAMPLER;
float4 main(VsOut IN): COLOR {
	return DiffuseColor*tex2D(DIFFUSE_SAMPLER, IN.texCoord);
};
