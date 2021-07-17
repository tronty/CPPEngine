struct VsIn
{
    	float4 pos : POSITION;
};

struct VsOut
{
    	float4 pos : SV_POSITION;
	float2 texCoord0: TEXCOORD0;
	float2 texCoord1: TEXCOORD1;
	float2 texCoord2: TEXCOORD2;
	float2 texCoord3: TEXCOORD3;
	float2 texCoord4: TEXCOORD4;
};

float2 scale;
float pixel;
float halfPixel;

VsOut VS(VsIn In){
	VsOut Out;

	Out.pos = In.pos;

	float2 texCoord = In.pos.xy * float2(0.5, -0.5) + 0.5 + halfPixel;
	Out.texCoord0 = texCoord + float2( pixel, 0.0);
	Out.texCoord1 = texCoord + float2(-pixel, 0.0);
	Out.texCoord2 = texCoord + float2( 0.0,  pixel);
	Out.texCoord3 = texCoord + float2( 0.0, -pixel);
	Out.texCoord4 = texCoord;

	return Out;
}

Texture2D Height;
SamplerState sam;

float4 PS(VsOut In) : COLOR {

	float height0 = Height.Sample( sam, In.texCoord0).x;
	float height1 = Height.Sample( sam, In.texCoord1).x;
	float height2 = Height.Sample( sam, In.texCoord2).x;
	float height3 = Height.Sample( sam, In.texCoord3).x;
	float height4 = Height.Sample( sam, In.texCoord4).x;

	float3 tangent  = float3(0.004, height0 - height1, 0.0  );
	float3 binormal = float3(0.0,   height2 - height3, 0.004);
	float3 normal = -normalize(cross(tangent, binormal));

	return float4(normal, height4);
}

