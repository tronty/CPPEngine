[Vertex shader]

float4x4 mvp;
float4 LightPos;
float4 CameraPos;

struct VsOut {
	float4 Pos: POSITION;
	float2 t0: TEXCOORD0;
	float3 Ldir: TEXCOORD1;
	float3 viewVec: TEXCOORD2;
};

VsOut main(float4 pos: POSITION, float2 tc0: TEXCOORD0){
	VsOut Out;

	Out.Pos = mul(mvp, pos);
	Out.t0 = tc0;
	float3 lightVec = normalize(LightPos.xyz - pos.xyz);
	float3 viewVec = normalize(CameraPos - pos.xyz);

	Out.Ldir.x = dot(lightVec.xyz, float3(-1, 0,  0));
	Out.Ldir.y = dot(lightVec.xyz, float3( 0, 0, -1));
	Out.Ldir.z = dot(lightVec.xyz, float3( 0, 1,  0));
	
	Out.viewVec.x = dot(viewVec, float3(-1, 0,  0));
	Out.viewVec.y = dot(viewVec, float3( 0, 0, -1));
	Out.viewVec.z = dot(viewVec, float3( 0, 1,  0));

	return Out;
}

[Fragment shader]

sampler2D DiffuseMap;
sampler2D BumpMap;
float4 LightColor;

float4 main(float2 texcoord: TEXCOORD0, float3 litdir: TEXCOORD1, float3 viewVec: TEXCOORD2) : COLOR {
	float3 bump = tex2D(BumpMap, texcoord).xyz * 2.0 - 1.0;
	bump = normalize(bump);

	// Standard Phong lighting
	float diffuse = saturate(dot(litdir, bump));
	float specular = pow(saturate(dot(reflect(-viewVec, bump), litdir)), 16.0);
	
	return tex2D(DiffuseMap, texcoord) * (diffuse + specular) * LightColor;
}

