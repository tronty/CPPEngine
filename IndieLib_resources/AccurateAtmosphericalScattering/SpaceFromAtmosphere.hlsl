//
// Common.cg
// Author: Sean O'Neil
// Copyright (c) 2004 Sean O'Neil
//

// The output of this vertex shader
struct vertout
{
	float4 pos : POSITION;		// Transformed vertex position
	float4 c0 : COLOR0;			// The Rayleigh color
	float4 c1 : COLOR1;			// The Mie color
	float2 t0 : TEXCOORD0;
};


// The number of sample points taken along the ray
const int nSamples = 5;
const float fSamples = (float)nSamples;

// The scale depth (the altitude at which the average atmospheric density is found)
const float fScaleDepth = 0.25;
const float fInvScaleDepth = 1.0 / fScaleDepth;


// The scale equation calculated by Vernier's Graphical Analysis
float scale(float fCos)
{
	float x = 1.0 - fCos;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

// Calculates the Mie phase function
float getMiePhase(float fCos, float fCos2, float g, float g2)
{
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
}

// Calculates the Rayleigh phase function
float getRayleighPhase(float fCos2)
{
	//return 1.0;
	return 0.75 + 0.75*fCos2;
}

// Returns the near intersection point of a line and a sphere
float getNearIntersection(float3 v3Pos, float3 v3Ray, float fDistance2, float fRadius2)
{
	float B = 2.0 * dot(v3Pos, v3Ray);
	float C = fDistance2 - fRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	return 0.5 * (-B - sqrt(fDet));
}

// Returns the far intersection point of a line and a sphere
float getFarIntersection(float3 v3Pos, float3 v3Ray, float fDistance2, float fRadius2)
{
	float B = 2.0 * dot(v3Pos, v3Ray);
	float C = fDistance2 - fRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	return 0.5 * (-B + sqrt(fDet));
}
struct VsIn {
    float3 Vertex : POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 MultiTexCoord0		: TEXCOORD1;
};
[Vertex shader]
//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//


ROW_MAJOR float4x4 GL_ModelViewProjectionMatrix MVPSEMANTIC;
float3 v3CameraPos;		// The camera's current position
float3 v3LightPos;			// The direction vector to the light source
float3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
float fCameraHeight;		// The camera's current height
float fCameraHeight2;		// fCameraHeight^2
float fOuterRadius;		// The outer (atmosphere) radius
float fOuterRadius2;		// fOuterRadius^2
float fInnerRadius;		// The inner (planetary) radius
float fInnerRadius2;		// fInnerRadius^2
float fKrESun;				// Kr * ESun
float fKmESun;				// Km * ESun
float fKr4PI;				// Kr * 4 * PI
float fKm4PI;				// Km * 4 * PI
float fScale;				// 1 / (fOuterRadius - fInnerRadius)
float fScaleOverScaleDepth;

struct appdata {
    float3 Vertex	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 MultiTexCoord0		: TEXCOORD1;
};
vertout main(appdata IN)// fScale / fScaleDepth
{
	// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	float3 v3Pos = IN.Vertex.xyz;
	float3 v3Ray = v3Pos - v3CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the farther intersection of the ray with the outer atmosphere (which is the far point of the ray passing through the atmosphere)
	fFar = getFarIntersection(v3CameraPos, v3Ray, fCameraHeight2, fOuterRadius2);

	// Calculate attenuation from the camera to the top of the atmosphere toward the vertex
	float3 v3Start = v3CameraPos;
	float fHeight = length(v3Start);
	float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
	float fAngle = dot(v3Ray, v3Start) / fHeight;
	float fScatter = fDepth*scale(fAngle);

	vertout OUT;
	//OUT.pos = mul(GL_ModelViewProjectionMatrix, float4(IN.Vertex,1));
	OUT.pos = mul(float4(IN.Vertex,1), GL_ModelViewProjectionMatrix);
	OUT.c1.rgb = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
	OUT.t0 = IN.MultiTexCoord0;
	return OUT;
}
[Fragment shader]
//
// Atmospheric scattering fragment shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//
//sampler2D s2Test;

float4 main(in float4 Color : COLOR0,
			in float4 SecondaryColor : COLOR1,
			in float2 TexCoord0 : TEXCOORD0) : COLOR

{
	//return float4(0,0,0,1);
	//return  SecondaryColor;
	float4 color = SecondaryColor;// * tex2D(s2Test, TexCoord0);
	//color.a = color.b;
	return color;
}