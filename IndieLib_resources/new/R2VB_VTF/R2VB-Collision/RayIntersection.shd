//-----------------------------------------------------------------------------
// File: Samples\DirectX 9.0\R2VB-Collision\RayIntersection.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;
struct VsOut {
	float4 Pos: POSITION;
	float2 uv: TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 uv: TEXCOORD0)
{
	VsOut Out;
	Out.Pos = mul(mvp, pos);
	Out.uv = uv;
	return Out;
}

[Fragment shader]

sampler2D	Sampler0;	// Terrain height map

float2		RcpTotalTerrainDimensions;
float4		RayStartingPoint;
float3		RayDirectionVector;
float2		HeightMapSize;
float2		TexelSize;
float2		TerrainUnitSize;

// Given a triangle defined by three dimensional points this function returns 
// the UNNORMALIZED plane normal and height at point of coordinates (X, Z) in the triangle
// Unnormalized normal is returned in .xyz and height in .w.
float4 NormalAndHeightOnPlane(float3 TrianglePoint[3], float X, float Z)
{
	float3 AB, AC;
	float4 NormalAndHeight;

	// Calculate AB and AC two triangle vectors
	AB = TrianglePoint[1] - TrianglePoint[0];
	AC = TrianglePoint[2] - TrianglePoint[0];

	// Calculate plane equation
	NormalAndHeight.xyz = cross(AB, AC);
	NormalAndHeight.w = - dot(NormalAndHeight.xyz, TrianglePoint[0]);

	// Feed point's XZ coordinate into equation to retrieve height
	NormalAndHeight.w = (NormalAndHeight.x * X + NormalAndHeight.z * Z + NormalAndHeight.w) / (-NormalAndHeight.y);

	return NormalAndHeight;
}

float4 SampleNormalAndHeight( float3 Point )
{
	float3 InputTriangle[3];
	float2 FractionalPart;
	float4 NormalAndHeight = {0, 1, 0, 0};
	float4 ParticleTexCoords = {0, 0, 0, 0};

	// Calculate texture coordinates from particle's XZ position on terrain
	ParticleTexCoords.xy = ((Point.xz * RcpTotalTerrainDimensions + 0.5f) * (HeightMapSize-1.0)) / HeightMapSize;

	// Calculate fractional part. This is used to determine which one of the two quad triangles the point is on
	FractionalPart = frac(ParticleTexCoords.xy * HeightMapSize.xy);

	// Accurate height calculation
	// The algorithm below samples the three points forming the triangle onto which the selected
	// point lies, then calculate the height at this point using the triangle's plane equation

	// *--*
	// |A/|
	// |/B|
	// *--*
	// Determine on which triangle the point lies based on the fractional part of the texture coordinates
	// The following code uses tex2dlod to ensure texture instructions are generated inside branching instructions
	// This code could benefit from Fetch4 if the heights were stored separately
	if ( FractionalPart.x < (1.0 - FractionalPart.y) )
	{
		// Point lies on triangle A
		InputTriangle[0].xyz = float3(0.0, tex2Dlod( Sampler0, ParticleTexCoords + float4(0.0, 0.0, 0.0, 0.0)        ).y, 0.0);
		InputTriangle[1].xyz = float3(0.0, tex2Dlod( Sampler0, ParticleTexCoords + float4(0.0, TexelSize.y, 0.0, 0.0)).y, TerrainUnitSize.y);
		InputTriangle[2].xyz = float3(TerrainUnitSize.x, tex2Dlod( Sampler0, ParticleTexCoords + float4(TexelSize.x, 0.0, 0.0, 0.0)).y, 0.0);
	}
	else
	{
		// Point lies on triangle B
		InputTriangle[0].xyz = float3(0.0, tex2Dlod( Sampler0, ParticleTexCoords + float4(0.0, TexelSize.y, 0.0, 0.0)        ).y, TerrainUnitSize.y);
		InputTriangle[1].xyz = float3(TerrainUnitSize.x, tex2Dlod( Sampler0, ParticleTexCoords + float4(TexelSize.x, TexelSize.y, 0.0, 0.0)).y, TerrainUnitSize.y);
		InputTriangle[2].xyz = float3(TerrainUnitSize.x, tex2Dlod( Sampler0, ParticleTexCoords + float4(TexelSize.x, 0.0, 0.0, 0.0)        ).y, 0.0);
	}

	// Calculate height of selected point inside triangle
	NormalAndHeight = NormalAndHeightOnPlane(InputTriangle, FractionalPart.x*TerrainUnitSize.x, FractionalPart.y*TerrainUnitSize.y);

	// Return result
	return NormalAndHeight;
}

float4 main(float2 vPos : VPOS) : COLOR0
{
	float3	CurrentPoint = RayStartingPoint.xyz;
	float4	TerrainNormalAndHeight;
	float4	ScreenCoordinates;

	// Use a double "for" loop with a given number of iterations (e.g. 50x50 = 2500)
	// In theory we could clip the starting point to the terrain's bounding cube so that a maximum
	// of TEXTURESIZE * sqrt(2) only iterations are required to guarentee the parsing of all the
	// pixels that need testing with the ray. However since we're running this pixel shader on a
	// single pixel the loop's execution length doesn't really matter.
	for (int i=0; i<50; i++)
	{
		for (int j=0; j<50; j++)
		{
			// Test for collision at current point
			TerrainNormalAndHeight = SampleNormalAndHeight( CurrentPoint );
			
			// Test for collision
			if (CurrentPoint.y < TerrainNormalAndHeight.w)
			{
				// Collision occured, stop loop
				j = 50; i = 50;
			}
			else
			{
				CurrentPoint.xyz += RayDirectionVector.xyz;
			}
		}
	} 

	// Convert point to screen coordinates
	ScreenCoordinates.xy = (CurrentPoint.xz * RcpTotalTerrainDimensions.xy + 0.5)*HeightMapSize;
	ScreenCoordinates.zw = float2(0, 1);

	return ScreenCoordinates;
}

