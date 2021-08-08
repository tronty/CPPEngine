//-----------------------------------------------------------------------------
// File: NPatch.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

struct VsOut {
	float4 pos      : POSITION;
	float2 texCoord : TEXCOORD0;
	float2 wCoord   : TEXCOORD1;
};

float2 halfPixel;
float width;

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.pos = pos;
	Out.texCoord = pos.xy * float2(0.5, -0.5) + float2(0.5, 0.5) + halfPixel;
	Out.wCoord = Out.texCoord * width;

	return Out;
}

[Fragment shader]

struct PsOut {
	float4 position : COLOR0;
	float4 normal   : COLOR1;
};

sampler2D Pos0, Pos1, Pos2;
sampler2D Nrm0, Nrm1, Nrm2;
sampler2D Weights;

PsOut main(float2 texCoord: TEXCOORD0, float2 wCoord: TEXCOORD1){
	PsOut Out;

	/*
		We use frac() here to get texture coordinate wrapping. This is done because
		for most tessellation levels we have a NPOT weights texture, and thus texture
		address wrapping is not supported.
	*/
	float4 w = tex2D(Weights, frac(wCoord));

	// Sample the corners
	float4 pos[3], nrm[3];
	pos[0] = tex2D(Pos0, texCoord);
	nrm[0] = tex2D(Nrm0, texCoord);
	pos[1] = tex2D(Pos1, texCoord);
	nrm[1] = tex2D(Nrm1, texCoord);
	pos[2] = tex2D(Pos2, texCoord);
	nrm[2] = tex2D(Nrm2, texCoord);

	/*
		Compute control points for the N-patch. The math behind this can be found in this paper:
		http://www.ati.com/developer/CurvedPNTriangles.pdf

		There's a bit of duplicate work here as these computation will be repeated for each generated
		vertex in the triangle. In the higher tessellation levels it would probably be faster to make
		three intermediate passes that computes the control points and writes them out to a bunch of
		render targets, but on the other hand it's quite a bit of data to write out (actually one control
		point would have to be left out and computed here anyway since we can only sample 16 textures,
		and we have 16 control points but we also need the weights), so for the lower levels of tessellation
		the overhead of these passes and the cost of sampling these textures here would probably be higher
		than the cost of redoing these computations here. Generally it doesn't make much sense to tessellate
		more than a couple of levels anyway.
	*/
#define P(i, j) (2.0 * pos[i].xyz + pos[j].xyz)
#define W(i, j) dot(pos[j].xyz - pos[i].xyz, nrm[i].xyz) * nrm[i].xyz
	// Corner control points
	float3 b300 = pos[0].xyz;
	float3 b030 = pos[1].xyz;
	float3 b003 = pos[2].xyz;
	// Edge 1/3 and 2/3 control points
	float3 b210 = (P(0, 1) - W(0, 1)) / 3.0;
	float3 b120 = (P(1, 0) - W(1, 0)) / 3.0;
	float3 b021 = (P(1, 2) - W(1, 2)) / 3.0;
	float3 b012 = (P(2, 1) - W(2, 1)) / 3.0;
	float3 b102 = (P(2, 0) - W(2, 0)) / 3.0;
	float3 b201 = (P(0, 2) - W(0, 2)) / 3.0;
	// Center control point
	float3 E = (b210 + b120 + b021 + b012 + b102 + b201) / 6.0;
	float3 V = (pos[0].xyz + pos[1].xyz + pos[2].xyz) / 3.0;
	float3 b111 = E + (E - V) / 2.0;

#define V(i, j) (2.0 * dot(pos[j].xyz - pos[i].xyz, nrm[i].xyz + nrm[j].xyz) / dot(pos[j].xyz - pos[i].xyz, pos[j].xyz - pos[i].xyz)) * (pos[j].xyz - pos[i].xyz)
	// Corner normals
	float3 n200 = nrm[0].xyz;
	float3 n020 = nrm[1].xyz;
	float3 n002 = nrm[2].xyz;
	// Edge mid-point control points
	float3 n110 = normalize(nrm[0].xyz + nrm[1].xyz - V(0, 1));
	float3 n011 = normalize(nrm[1].xyz + nrm[2].xyz - V(1, 2));
	float3 n101 = normalize(nrm[2].xyz + nrm[0].xyz - V(2, 0));



	// Cubic interpolation of the position
	float3 w2 = w * w;
	float3 w3 = w * w2;
	float3 position =
		b300 * w3.x + b030 * w3.y + b003 * w3.z + 
		b210 * 3.0 * w2.x * w.y + b120 * 3.0 * w.x * w2.y + b201 * 3.0 * w2.x * w.z +
		b021 * 3.0 * w2.y * w.z + b102 * 3.0 * w.x * w2.z + b012 * 3.0 * w.y * w2.z +
		b111 * w.w;

	Out.position.xyz = position;

	// Quadratic interpolation of the normal
	float3 normal =
		n200 * w2.x + n020 * w2.y + n002 * w2.z +
		n110 * w.x * w.y + n011 * w.y * w.z + n101 * w.x * w.z;

	Out.normal.xyz = normalize(normal);

	// Linearly interpolate and store texture coordinates in spare .w components
	Out.position.w = w.x * pos[0].w + w.y * pos[1].w + w.z * pos[2].w;
	Out.normal.w   = w.x * nrm[0].w + w.y * nrm[1].w + w.z * nrm[2].w;

	return Out;
}

