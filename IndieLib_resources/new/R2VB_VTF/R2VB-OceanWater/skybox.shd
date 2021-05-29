//-----------------------------------------------------------------------------
// File: Skybox.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
[Vertex shader]

float4x4 mvp;

struct VsOut {
    float4 Pos: POSITION;
    float3 dir: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader for the sky box.
// Maps the position on the cube to a direction vector for sampling the cubemap
//--------------------------------------------------------------------------------------
VsOut main(float4 pos: POSITION){
    VsOut Out;

    Out.Pos = mul(mvp, pos);
    // we need to swizzle because the cube map is z up, but the scene geometry is y up.
    Out.dir = pos.zxy;

    return Out;
}

[Fragment shader]

samplerCUBE Skybox;

//--------------------------------------------------------------------------------------
// Skybox pixel shader - samples the sky cubemap
//--------------------------------------------------------------------------------------
float4 main(float3 dir: TEXCOORD0) : COLOR {
    return texCUBE(Skybox, dir);
}

