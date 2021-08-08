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

sampler2D TransformMatrix;
sampler2D VertexTexture;
sampler2D MatrixIndexTexture;
float NumLinks;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	float4 Pos, MatrixIndex;
	float4x4 m;
	
	Pos = tex2D(VertexTexture, t0);
	MatrixIndex = tex2D(MatrixIndexTexture, t0);
	// get transform matrix of this link
	m[0] = tex2D(TransformMatrix, float2((0.125f+MatrixIndex.x)/NumLinks, 0.5f));
	m[1] = tex2D(TransformMatrix, float2((0.275f+MatrixIndex.x)/NumLinks, 0.5f));
	m[2] = tex2D(TransformMatrix, float2((0.625f+MatrixIndex.x)/NumLinks, 0.5f));
	m[3] = tex2D(TransformMatrix, float2((0.875f+MatrixIndex.x)/NumLinks, 0.5f));
	
	return mul(Pos, m);
}

