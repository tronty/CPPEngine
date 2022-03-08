
struct VsOutNormal {
	float4 position: POSITION;
        float3 normal: NORMAL;
};

	struct PositionNormalVertex
        {
            float3 position: POSITION;
            float3 normal: NORMAL;
            float3 ColorDiffuse: TEXCOORD0;
	};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOutNormal main(PositionNormalVertex In){
	VsOutNormal Out=(VsOutNormal)0;
		Out.position = mul(worldViewProj, float4(In.position.xyz, 1));
		Out.position.w= WSIGN 1;
	Out.normal = In.normal;
		return Out;
	}
[Fragment shader]
float4 DiffuseColor=float4(1,1,1,1);
sampler2D DIFFUSE_SAMPLER;

float4 main(VsOutNormal IN): COLOR {
	return DiffuseColor*float4(IN.normal, 1);
};

