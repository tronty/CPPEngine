[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos:  POSITION;
	float2 t0 : TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 tc0 : TEXCOORD0){
	VsOut Out;

	Out.Pos = mul(mvp, pos);
	Out.t0 = tc0;
	
	return Out;
}


[Fragment shader]
sampler2D DiffuseMap;
float4 LightColor;

float4 main(float2 texcoord:TEXCOORD0, float3 litdir : TEXCOORD1) : COLOR {
	return tex2D(DiffuseMap, texcoord)*LightColor;
}
