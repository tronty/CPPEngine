[Vertex shader]
struct VsIn {
float3 position: POSITION;
float3 normal:   NORMAL;
float4 color: 	 TEXCOORD0;
};
struct VsOut {
float4 position: POSITION;
float4 color: 	 TEXCOORD0;
	float3 Light : TEXCOORD1;
	float3 Norm : NORMAL;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
ROW_MAJOR float4x4 World;
float4 vecLightDir;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
		Out.position = mul(worldViewProj, float4(In.position.xyz, 1));
		Out.position.w= WSIGN 1;
		Out.color=In.color;
	Out.Light = vecLightDir.xyz;
	Out.Norm = normalize(mul(World, float4(In.normal, 1))).xyz;
		return Out;
	}
[Fragment shader]
struct VsOut {
float4 position: POSITION;
float4 color: 	 TEXCOORD0;
	float3 Light : TEXCOORD1;
	float3 Norm : NORMAL;
};
float4 AmbientColor;
float4 DiffuseColor;
float4 main(VsOut IN) : COLOR
{
	return AmbientColor + DiffuseColor * IN.color*saturate(dot(IN.Light, IN.Norm));
}

