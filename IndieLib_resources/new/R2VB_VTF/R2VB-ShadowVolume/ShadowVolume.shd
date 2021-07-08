[Vertex shader]

float4x4 mvp;
float4 LightPos;

struct VsOut {
	float4 Pos: POSITION;
};

VsOut main(float4 pos: POSITION, float4 nor: NORMAL0){
	VsOut Out;
	float3 L;
	float Dot;
	float4 TPos;
	
	L = LightPos.xyz - pos.xyz;
	L = normalize(L);
	Dot = dot(L, nor);
	if (Dot < 0)
	{
		TPos.xyz = pos.xyz - 50.0f*L;
	}
	else
	{
		TPos.xyz = pos.xyz;
	}
	TPos.w = 1.0f;		
	Out.Pos = mul(mvp, TPos);
	return Out;
}

[Fragment shader]

float4 main() : COLOR {
	return float4(0, 1, 0, 1.0f);
}

