[Vertex shader]
 mat4 mvp ;
struct VsOut {
	vec4 pos:       POSITION;
	vec3 cubeCoord: TEXCOORD0;
};
VsOut main(vec4 pos: POSITION){
	VsOut Out;
	__pos = mul(pos, mvp);
	// Nvidia seems to have some kind of weird precision problem here
	__pos.z *= 0.99999;
	__cubeCoord = pos.xyz;
	return Out;
}
[Fragment shader]
samplerCUBE Env;
//vec4 fogColor;
//vec2 scaleBias;
vec4 main(vec3 cubeCoord: TEXCOORD0) : COLOR {
	vec4 sky = texCUBE(Env, cubeCoord);
	//float fog = saturate(normalize(cubeCoord).y * scaleBias.x + scaleBias.y);
	//fog *= fog * (3 - 2 * fog);
	//return lerp(fogColor, sky, fog);
	return sky;
}


