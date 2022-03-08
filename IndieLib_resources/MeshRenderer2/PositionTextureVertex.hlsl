struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};

        struct PositionTextureVertex
        {
            float3 position: POSITION;
            float3 ColorDiffuse: TEXCOORD0;
            float2 texCoord: TEXCOORD1;
	};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(PositionTextureVertex In){
	VsOut Out=(VsOut)0;
		Out.position = mul(worldViewProj, float4(In.position.xyz, 1));
		Out.position.w= WSIGN 1;
	Out.texCoord = In.texCoord;
		return Out;
	}
[Fragment shader]
float4 DiffuseColor=float4(1,1,1,1);
sampler2D DIFFUSE_SAMPLER;
float4 main(VsOut IN): COLOR {
	return DiffuseColor*tex2D(DIFFUSE_SAMPLER, IN.texCoord);
};

