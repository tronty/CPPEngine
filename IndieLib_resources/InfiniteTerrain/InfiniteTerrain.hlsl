[Vertex shader]
ROW_MAJOR float4x4 mvp MVPSEMANTIC;
struct VsOut {
	float4 pos:       POSITION;
	float3 cubeCoord: TEXCOORD0;
};
VsOut main(float4 pos: POSITION){
	VsOut Out;
	Out.pos = mul(pos, mvp);
	// Nvidia seems to have some kind of weird precision problem here
	Out.pos.z *= 0.99999;
	Out.cubeCoord = pos.xyz;
	return Out;
}
[Fragment shader]
samplerCUBE Env;
//float4 fogColor;
//float2 scaleBias;
float4 main(float3 cubeCoord: TEXCOORD0) : COLOR {
	float4 sky = texCUBE(Env, cubeCoord);
	//float fog = saturate(normalize(cubeCoord).y * scaleBias.x + scaleBias.y);
	//fog *= fog * (3 - 2 * fog);
	//return lerp(fogColor, sky, fog);
	return sky;
}


