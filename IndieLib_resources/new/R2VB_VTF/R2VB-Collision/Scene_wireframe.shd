//-----------------------------------------------------------------------------
// File: Samples\DirectX 9.0\R2VB-Collision\Scene_wireframe.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 WorldTransformMatrix;

struct vertexShaderInput
{
    float4 Position : POSITION0;
};

struct vertexShaderOutput
{
    float4 Position : POSITION0;
};

vertexShaderOutput main( vertexShaderInput Input )
{
    vertexShaderOutput Out;

    // Transform position
    Out.Position = mul( WorldTransformMatrix, Input.Position );

    return Out;
}

[Fragment shader]

float4 main() : COLOR0
{
	return float4(1, 1, 1, 1);
}

