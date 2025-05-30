[Vertex shader]

struct VsOut {
	float4 pos: POSITION;
	float2 texCoord0: TEXCOORD0;
	float2 texCoord1: TEXCOORD1;
	float2 texCoord2: TEXCOORD2;
	float2 texCoord3: TEXCOORD3;
	float2 texCoord4: TEXCOORD4;
};

//float2 scale;
float pixel=0.001953;
float halfPixel=0.000977;

VsOut main(float2 pos: POSITION){
	VsOut Out;

	Out.pos = float4(pos.x, pos.y, 0, 1);

	float2 texCoord = pos.xy * float2(0.5, -0.5) + 0.5 + halfPixel;
	Out.texCoord0 = texCoord + float2( pixel, 0.0);
	Out.texCoord1 = texCoord + float2(-pixel, 0.0);
	Out.texCoord2 = texCoord + float2( 0.0,  pixel);
	Out.texCoord3 = texCoord + float2( 0.0, -pixel);
	Out.texCoord4 = texCoord;

	return Out;
}

[Fragment shader]

sampler2D Height;

float4 main(float2 texCoord0: TEXCOORD0, float2 texCoord1: TEXCOORD1,
			float2 texCoord2: TEXCOORD2, float2 texCoord3: TEXCOORD3, float2 texCoord4: TEXCOORD4) : COLOR {

	float height0 = tex2D(Height, texCoord0).x;
	float height1 = tex2D(Height, texCoord1).x;
	float height2 = tex2D(Height, texCoord2).x;
	float height3 = tex2D(Height, texCoord3).x;
	float height4 = tex2D(Height, texCoord4).x;

	float3 tangent  = float3(0.004, height0 - height1, 0.0  );
	float3 binormal = float3(0.0,   height2 - height3, 0.004);
	float3 normal = -normalize(cross(tangent, binormal));

	return float4(normal, height4);
}

