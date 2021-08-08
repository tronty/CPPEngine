struct VsIn
{
	float4 pos : POSITION;
};

struct VsOut
{
	float4 pos      : SV_POSITION;
	float2 coord: TEXCOORD0;
};

float2 offset;

VsOut VS(VsIn In){
	VsOut Out;

	Out.pos = In.pos;
	Out.coord = 0.24 * (In.pos.xy + offset);

	return Out;
}

Texture2D Noise;
SamplerState sam;

float turb(float2 c){
	float t = 0.0;

	float k = 1.0;
	for (int i = 0; i < 4; i++){
		float n = Noise.Sample( sam, c).x;

		t += k * n;

		k /= 2.192038172;
		c *= 2.192038172;
	}

	return t;
}

float4 PS(VsOut In) : COLOR {
	float t = turb(In.coord);

	return t * t * 0.45;
}

