[Vertex shader]

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 t0 : TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;
	Out.tc0 = t0;

	return Out;
}


[Fragment shader]

sampler2D ParentM;
sampler2D PivotM;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	float4x4 m;
	
	// get parent transform matrix
	m[0] = tex2D(ParentM, float2(0.125f, 0.5f));
	m[1] = tex2D(ParentM, float2(0.275f, 0.5f));
	m[2] = tex2D(ParentM, float2(0.625f, 0.5f));
	m[3] = tex2D(ParentM, float2(0.875f, 0.5f));
	
	// transform matrix = pivot matrix * parent transform matrix
	return mul(tex2D(PivotM, t0), m);
}

