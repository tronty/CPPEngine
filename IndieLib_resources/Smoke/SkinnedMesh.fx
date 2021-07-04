//----------------------------------------------------------------------------------
// File:   Scene.fx
// Author: Ignacio Llamas and Sarah Tariq
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------

DepthStencilState DisableDepthStencil
{
    DepthEnable = false;
    DepthWriteMask = ZERO;
    StencilEnable = false;
    StencilReadMask = 0x0;
    StencilWriteMask = 0x0;
};

DepthStencilState WriteDepthTest
{
    // depth
    DepthWriteMask = ALL;
    DepthEnable = true;
    DepthFunc = Less;

    //stencil
    StencilEnable = false;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0x00;
};

RasterizerState CullBack
{
    MultiSampleEnable = True;
    CullMode = Back;
};

BlendState NoBlending
{
    BlendEnable[0] = false;
    RenderTargetWriteMask[0] = 0x0F;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samAniso
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
#ifndef MATRIX_PALETTE_SIZE_DEFAULT
#define MATRIX_PALETTE_SIZE_DEFAULT 100
#endif

#ifndef MIN_VALID_WEIGHT
#define MIN_VALID_WEIGHT 0.001f
#endif


float3      EyePos;
float4x4    WorldMatrix;
float4x4    ViewProjectionMatrix;

float4x4    Bones[MATRIX_PALETTE_SIZE_DEFAULT];

float4      g_AmbientLight = float4(0.4,0.4,0.4,0.0);
float       g_PointLightIntensity = 1.0;
float3      g_PointLightPos =  float3(-5,-10,-5);
float       g_Kd = 1;
float       g_Ks = 2;
float       g_specPower = 40;
bool        g_useFire = false;

Texture2D   sceneTextureDiffuse;
Texture2D   sceneTextureSpecular;
Texture2D   sceneTextureNormal;

bool useNormalMap = true;

//--------------------------------------------------------------------------------------
struct VsRigidInput
{
    float3 Pos      : POSITION;
    float3 Norm     : NORMAL;
};


struct VsRigidTCInput
{
    VsRigidInput VsRigidIn;
    float2 Texture  : TEXCOORD;
};

struct VsSkinnedInput
{
    float3 Pos      : POSITION;
    float3 Norm     : NORMAL;
    float2 Texture  : TEXCOORD;
    float4 Weights0 : BLENDWEIGHTS0;
    float4 Weights1 : BLENDWEIGHTS1;
    uint4  Indices0 : BLENDINDICES0;
    uint4  Indices1 : BLENDINDICES1;
};

struct PsInput
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 Texture  : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float4x4 SkinWorld : WORLDMATRIX;
};

struct PsCnDInput
{
    PsInput PsIn;
    float   Depth   : TEXCOORD3;
};

struct PsCnDOutput
{
    float4 Color    : COLOR0;
    float  Depth    : COLOR1;
};
[Vertex shader]
//--------------------------------------------------------------------------------------
// Vertex Shaders
//--------------------------------------------------------------------------------------

matrix GetSkinWorldMatrix( VsSkinnedInput input )
{
    matrix skinMatrix = 0;
    for(int i=0; i<4; i++)
    {
        if(input.Weights0[i] > MIN_VALID_WEIGHT)
            skinMatrix += input.Weights0[i] * Bones[input.Indices0[i]];
        if(input.Weights1[i] > MIN_VALID_WEIGHT)
            skinMatrix += input.Weights1[i] * Bones[input.Indices1[i]];
    }

    return mul(skinMatrix, WorldMatrix);
}

PsCnDInput VS_SKINNED( VsSkinnedInput input )
{
    PsCnDInput output;

    matrix skinWorldMatrix = GetSkinWorldMatrix(input);

    float4 worldPos = mul(float4(input.Pos,1), skinWorldMatrix);

    output.PsIn.Position = mul(worldPos, ViewProjectionMatrix);
    output.PsIn.WorldPos = worldPos;
    output.PsIn.Normal   = normalize(mul(input.Norm, (float3x3)skinWorldMatrix));
    output.PsIn.Texture  = input.Texture;
    output.PsIn.SkinWorld = skinWorldMatrix;
    output.Depth = output.PsIn.Position.w;    

    return output;
}

VsRigidTCInput VS_SKINNED_SO( VsSkinnedInput input )
{
    VsRigidTCInput output;

    matrix skinWorldMatrix = GetSkinWorldMatrix(input);

    output.VsRigidIn.Pos  = mul(float4(input.Pos,1), skinWorldMatrix).xyz;
    output.VsRigidIn.Norm = normalize(mul(input.Norm, (float3x3)skinWorldMatrix).xyz);
    output.Texture  = input.Texture;

    return output;
}


PsCnDInput VS_RIGID( VsRigidInput input )
{
    PsCnDInput output;

    float4 worldPos = mul(float4(input.Pos,1), WorldMatrix);
    output.PsIn.Position = mul(worldPos, ViewProjectionMatrix);
    output.PsIn.WorldPos = worldPos;
    output.PsIn.Normal   = normalize(mul(input.Norm, (float3x3)WorldMatrix));
    output.PsIn.SkinWorld = WorldMatrix;
    output.PsIn.Texture = 0;
    output.Depth = output.PsIn.Position.w;

    return output;
}

PsCnDInput VS_RIGID_TC( VsRigidTCInput input )
{
    PsCnDInput output;

    output = VS_RIGID(input.VsRigidIn);
    output.PsIn.Texture = input.Texture;

    return output;
}


VsRigidTCInput VS_RIGID_SO( VsRigidInput input )
{
    VsRigidTCInput output;

    output.VsRigidIn.Pos  = mul(float4(input.Pos,1), WorldMatrix).xyz;
    output.VsRigidIn.Norm = normalize(mul(input.Norm, (float3x3)WorldMatrix).xyz);
    output.Texture = 0;

    return output;
}

VsRigidTCInput VS_RIGID_SO_TC( VsRigidTCInput input )
{
    VsRigidTCInput output;

    output = VS_RIGID_SO(input.VsRigidIn);
    output.Texture = input.Texture;

    return output;
}

[Fragment shader]
//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------

float4 PSComputeColor( PsInput In )
{
    float2 tc = float2(In.Texture.x, 1.0-In.Texture.y);

    float3 N;
    if( useNormalMap )
    {
        float3 sampledNormal = normalize( (sceneTextureNormal.Sample(samLinear, tc).xyz * 2) - 0.5 );
        sampledNormal.z = -sampledNormal.z;
        N = normalize( mul(float4(sampledNormal,0), In.SkinWorld) );
    }
    else
    {
        N = normalize(In.Normal);
    }

    float4 diffuseMap = sceneTextureDiffuse.Sample(samLinear, tc);
    
    // hack to handle non-textured meshes
    float epsilon = 1.0/255;
    if( (diffuseMap.r < epsilon) && (diffuseMap.g < epsilon) && (diffuseMap.b < epsilon) && (diffuseMap.a < epsilon) )
        diffuseMap = float4(0.67, 0.35, 0.13, 1.0);

    float3 L = normalize(g_PointLightPos - In.WorldPos);
    float3 V = normalize(In.WorldPos - EyePos);

    //diffuse light
    float diffuseLighting   = g_PointLightIntensity * g_Kd * saturate(dot(N, L));

    //specular light
    float4 specularMap =  sceneTextureSpecular.Sample(samLinear, tc);
    float3 R = reflect(V, N);
    float specularLighting  = g_PointLightIntensity * g_Ks * 
                              saturate(pow(max(dot(R, L), 0), g_specPower)); 
                              
    float4 lightColor;
    if(g_useFire)
        lightColor = float4(0.968,0.58,0.114,1.0);                          
    else
        lightColor = float4(1.0,1.0,1.0,1.0);  

    return (diffuseMap * (lightColor*diffuseLighting + g_AmbientLight)) + (lightColor * specularMap * specularLighting);
}

PsCnDOutput PS_COLOR_AND_DEPTH( PsCnDInput input )
{
    PsCnDOutput output;
    output.Color = PSComputeColor(input.PsIn);
    output.Depth = input.Depth;
    return output;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique10 SkinnedMesh
{     
    pass
    {
        SetVertexShader( CompileShader( vs_4_0, VS_SKINNED() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_COLOR_AND_DEPTH() ) );

        SetDepthStencilState( WriteDepthTest, 0 );
        SetRasterizerState( CullBack );
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 RigidMesh
{     
    pass
    {
        SetVertexShader( CompileShader( vs_4_0, VS_RIGID() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_COLOR_AND_DEPTH() ) );

        SetDepthStencilState( WriteDepthTest, 0 );
        SetRasterizerState( CullBack );
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 RigidMesh_TC
{     
    pass
    {
        SetVertexShader( CompileShader( vs_4_0, VS_RIGID_TC() ) );
        SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_COLOR_AND_DEPTH() ) );

        SetDepthStencilState( WriteDepthTest, 0 );
        SetRasterizerState( CullBack );
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}


technique10 SkinnedMeshSO
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_SKINNED_SO() ) );
        SetGeometryShader( ConstructGSWithSO( CompileShader( vs_4_0, VS_SKINNED_SO() ), "POSITION.xyz; NORMAL.xyz; TEXCOORD.xy" ) );
        SetPixelShader( NULL );

        SetDepthStencilState( DisableDepthStencil, 0 );
    }
}

technique10 RigidMeshSO
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_RIGID_SO() ) );
        SetGeometryShader( ConstructGSWithSO( CompileShader( vs_4_0, VS_RIGID_SO() ), "POSITION.xyz; NORMAL.xyz; TEXCOORD.xy" ) );
        SetPixelShader( NULL );

        SetDepthStencilState( DisableDepthStencil, 0 );
    }
}

technique10 RigidMeshSO_TC
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_RIGID_SO_TC() ) );
        SetGeometryShader( ConstructGSWithSO( CompileShader( vs_4_0, VS_RIGID_SO() ), "POSITION.xyz; NORMAL.xyz; TEXCOORD.xy" ) );
        SetPixelShader( NULL );

        SetDepthStencilState( DisableDepthStencil, 0 );
    }
}
