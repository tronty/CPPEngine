float4x4 mvp;

struct VsIn
{
    float4 pos : POSITION;
};

struct VsOut
{
    float4 pos : SV_POSITION;
    float3 cubeCoord : TEXCOORD0;
};

VsOut VS(VsIn In){
	VsOut Out;

	Out.pos = mul(mvp, In.pos);
	// Nvidia seems to have some kind of weird precision problem here
	Out.pos.z *= 0.99999;
	Out.cubeCoord = In.pos.xyz;

	return Out;
}

TextureCUBE Env;
SamplerState sam;
float4 fogColor;
float2 scaleBias;

float4 PS(VsOut In) : COLOR {
	float4 sky = Env.Sample( sam, In.cubeCoord);

	float fog = saturate(normalize(In.cubeCoord).y * scaleBias.x + scaleBias.y);

	fog *= fog * (3 - 2 * fog);

	return lerp(fogColor, sky, fog);
}

