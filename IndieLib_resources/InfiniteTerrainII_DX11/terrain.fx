float4x4 mvp;
float3 camPos;

struct VsIn
{
    float2 pos : POSITION;
};

struct VsOut
{
	float4 pos      : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal   : TEXCOORD1;
	float  fog      : TEXCOORD2;
	float2 scale    : TEXCOORD3;
};

Texture2D Terrain;
SamplerState sam;

VsOut VS(VsIn In)
{
	float4 height = tex2Dlod(Terrain, float4(In.pos.xy + 0.5, 0.0, 0.0));
	float4 vertex = float4(4.0 *In. pos.x, height.w, 4.0 * In.pos.y, 1);

	VsOut Out;
	Out.pos = mul(mvp, vertex);
	Out.texCoord = In.pos * 16.0;
	Out.normal = height.xyz;
	Out.fog = saturate(0.65 * length(vertex.xyz - camPos));
	Out.scale = saturate(height.w * 1.2 - 0.3) * float2(0.6, 0.5);

	return Out;
}

Texture2D Ground;
SamplerState sam_;

float3 lightDir;
float4 fogColor;

float4 PS(VsOut In) : COLOR {
	float diffuse = saturate(dot(lightDir, In.normal) * In.scale.x + In.scale.y);
	float4 base = Ground.Sample( sam, In.texCoord);

	return lerp(diffuse * base, fogColor, In.fog);
}

