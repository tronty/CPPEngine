[Vertex shader]

float4x4 mvp;
float4x4 trans;

struct VsOut {
	float4 Pos:  POSITION;
	float2 t0 : TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 tc0 : TEXCOORD0){
	VsOut Out;
	float4	tPos;

	tPos = mul(pos, trans);
	Out.Pos = mul(mvp, tPos);
	Out.t0 = tc0;
	
	return Out;
}


[Fragment shader]
sampler2D DiffuseMap;

float4 main(float2 texcoord:TEXCOORD0) : COLOR {
	return tex2D(DiffuseMap, texcoord);
}

