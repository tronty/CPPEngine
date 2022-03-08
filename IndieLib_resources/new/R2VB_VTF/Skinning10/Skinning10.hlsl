#if 0
E:\>fxc Skinning10.fx /EVSSkinnedmain /Tvs_3_0
Microsoft (R) Direct3D Shader Compiler 9.29.952.3111
Copyright (C) Microsoft Corporation 2002-2009. All rights reserved.

E:\Skinning10.fx(159,16): error X4532: cannot map expression to vertex shader instruction set

compilation failed; no code produced

E:\>
#endif
//--------------------------------------------------------------------------------------
// File: Skinning10.fx
//
// The effect file for the Skinning10 sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// defines
//--------------------------------------------------------------------------------------
#define MAX_BONE_MATRICES 255
//define FT_CONSTANTBUFFER 0
//define FT_TEXTUREBUFFER 1
#define FT_TEXTURE 2
//define FT_BUFFER 3

//--------------------------------------------------------------------------------------
// Input/Output structures
//--------------------------------------------------------------------------------------
struct VSSkinnedIn
{
    float3 Pos	: POSITION;			//Position
    float4 Weights : TEXCOORD0;//WEIGHTS;		//Bone weights
    uint4  Bones : TEXCOORD1;//BONES;			//Bone indices
    float3 Norm : NORMAL;			//Normal
    float2 Tex	: TEXCOORD2;		    //Texture coordinate
    float3 Tan : TANGENT;		    //Normalized Tangent vector
};

struct VSStreamOut
{
    float4 Pos	: POSITION;			//Position
    float3 Norm : NORMAL;			//Normal
    float2 Tex	: TEXCOORD;		    //Texture coordinate
    float3 Tangent : TANGENT;		//Normalized Tangent vector
};

struct PSSkinnedIn
{
    float4 Pos	: POSITION;		//Position
    float3 vPos : TEXCOORD0;//POSWORLD;			//world space Pos
    float3 Norm : NORMAL;			//Normal
    float2 Tex	: TEXCOORD1;		    //Texture coordinate
    float3 Tangent : TANGENT;		//Normalized Tangent vector
};

//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------
//cbuffer cb0
//{
    //ROW_MAJOR 
    float4x4 g_mWorldViewProj;// MVPSEMANTIC;
    //ROW_MAJOR 
    float4x4 g_mWorld;// MVPSEMANTIC;
//};

//cbuffer cbUserChange
//{
    float3 g_vLightPos;
    float3 g_vEyePt;
//};

//cbuffer cbImmutable
//{
    float4 g_directional = float4(1.0,1.0,1.0,1.0);
    float4 g_ambient = float4(0.1,0.1,0.1,0.0);
    float4 g_objectspeccolor = float4(1.0,1.0,1.0,1.0);
//};

#if 0
// Constant buffer for bone matrices
//cbuffer cbAnimMatrices
//{
    float4x4 g_mConstBoneWorld[MAX_BONE_MATRICES];
//};

// TBuffer for bone matrices
//tbuffer tbAnimMatrices
//{
    float4x4 g_mTexBoneWorld[MAX_BONE_MATRICES];
//};
#endif

//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------
sampler2D g_txDiffuse;
sampler2D g_txNormal;
// Texture for bone matrices
sampler1D g_txTexBoneWorld;

//--------------------------------------------------------------------------------------
// Buffers (this is the buffer object for bone matrices)
//--------------------------------------------------------------------------------------
//Buffer<float4> g_BufferBoneWorld;

//--------------------------------------------------------------------------------------
// Helper struct for passing back skinned vertex information
//--------------------------------------------------------------------------------------
struct SkinnedInfo
{
    float4 Pos;
    float3 Norm;
    float3 Tan;
};

//--------------------------------------------------------------------------------------
// FetchBoneTransform fetches a bone transformation using one of several methods
//
//		FT_CONSTANTBUFFER:
//			With this approach, the bone matrices are stored in a constant buffer.
//			The shader will index into the constant buffer to grab the correct
//			transformation matrices for each vertex.
//		FT_TEXTUREBUFFER:
//			This approach shows the differences between constant buffers and tbuffers.
//			tbuffers are special constant buffers that are accessed like textures.
//			They should give better random access performance.
//		FT_TEXTURE:
//			When FT_TEXTURE is specified, the matrices are loaded into a 1D texture.
//			This is different from a tbuffer in that an actual texture fetch is used
//			instead of a lookup into a constant buffer.
//		FT_BUFFER:
//			This loads the matrices into a buffer that is bound to the shader.  The
//			shader calls Load on the buffer object to load the different matrices from
//			the stream.  This should give better linear access performance.
//--------------------------------------------------------------------------------------
uniform uint iFetchType = FT_TEXTURE;
float4x4 FetchBoneTransform( uint iBone_ )
{
	uint iBone=iBone_;
    float4x4 mret;
    #if 0
    if( FT_CONSTANTBUFFER == iFetchType )
    {
        mret = g_mConstBoneWorld[ iBone ];
    }
    else if ( FT_TEXTUREBUFFER == iFetchType )
    {
        mret = g_mTexBoneWorld[ iBone ];
    }
    else if ( FT_TEXTURE == iFetchType )
    #endif
    {
        //iBone *= 4;
        float4 row1 = tex1D(g_txTexBoneWorld, 4.0*iBone );
        float4 row2 = tex1D(g_txTexBoneWorld, 4.0*iBone + 1 );
        float4 row3 = tex1D(g_txTexBoneWorld, 4.0*iBone + 2 );
        float4 row4 = tex1D(g_txTexBoneWorld, 4.0*iBone + 3 ); // ??? E:\Skinning10.fx(159,16): error X4532: cannot map expression to vertex shader instruction set
        
        mret = float4x4( row1, row2, row3, row4 );
    }
    #if 0
    else if ( FT_BUFFER == iFetchType )
    {
        iBone *= 4;
        float4 row1 = g_BufferBoneWorld.Load( iBone );
        float4 row2 = g_BufferBoneWorld.Load( iBone + 1 );
        float4 row3 = g_BufferBoneWorld.Load( iBone + 2 );
        float4 row4 = g_BufferBoneWorld.Load( iBone + 3 );
        
        mret = float4x4( row1, row2, row3, row4 );
    }
    #endif
    
    return mret;
}

//--------------------------------------------------------------------------------------
// SkinVert skins a single vertex
//--------------------------------------------------------------------------------------
SkinnedInfo SkinVert( VSSkinnedIn Input) //, uniform uint iFetchType )
{
    SkinnedInfo Output = (SkinnedInfo)0;
    
    float4 Pos = float4(Input.Pos,1);
    float3 Norm = Input.Norm;
    float3 Tan = Input.Tan;
    
    //Bone0
    uint iBone = Input.Bones.x;
    float fWeight = Input.Weights.x;
    float4x4 m = FetchBoneTransform( iBone);//, iFetchType );
    Output.Pos += fWeight * mul( m, Pos );
    Output.Norm += fWeight * mul( (float3x3)m, Norm );
    Output.Tan += fWeight * mul( (float3x3)m, Tan );
    
    //Bone1
    iBone = Input.Bones.y;
    fWeight = Input.Weights.y;
    m = FetchBoneTransform( iBone);//, iFetchType );
    Output.Pos += fWeight * mul( m, Pos );
    Output.Norm += fWeight * mul( (float3x3)m, Norm );
    Output.Tan += fWeight * mul( (float3x3)m, Tan );
    
    //Bone2
    iBone = Input.Bones.z;
    fWeight = Input.Weights.z;
    m = FetchBoneTransform( iBone);//, iFetchType );
    Output.Pos += fWeight * mul( m, Pos );
    Output.Norm += fWeight * mul( (float3x3)m, Norm );
    Output.Tan += fWeight * mul( (float3x3)m, Tan );
    
    //Bone3
    iBone = Input.Bones.w;
    fWeight = Input.Weights.w;
    m = FetchBoneTransform( iBone);//, iFetchType );
    Output.Pos += fWeight * mul( m, Pos );
    Output.Norm += fWeight * mul( (float3x3)m, Norm );
    Output.Tan += fWeight * mul( (float3x3)m, Tan );
    
    return Output;
}

//--------------------------------------------------------------------------------------
// Vertex shader used for skinning the mesh for immediate render
//--------------------------------------------------------------------------------------
[Vertex shader]
PSSkinnedIn //VSSkinnedmain
	main(VSSkinnedIn input)//, uniform uint iFetchType )
{
    PSSkinnedIn output;
    
    SkinnedInfo vSkinned = SkinVert( input);//, iFetchType );
    output.Pos = mul( g_mWorldViewProj, vSkinned.Pos );
    output.vPos = mul( (float3x3)g_mWorld, vSkinned.Pos.xyz ); // ???
    output.Norm = normalize( mul( (float3x3)g_mWorld, vSkinned.Norm ));
    output.Tangent = normalize( mul( (float3x3)g_mWorld, vSkinned.Tan ));
    output.Tex = input.Tex;
    
    return output;
}

#if 0
//--------------------------------------------------------------------------------------
// Vertex shader used for skinning and streaming out
//--------------------------------------------------------------------------------------
VSStreamOut VSSkinnedStreamOutmain(VSSkinnedIn input)//, uniform uint iFetchType )
{
    VSStreamOut output = (VSStreamOut)0;
    
    SkinnedInfo vSkinned = SkinVert( input);//, iFetchType );
    output.Pos = vSkinned.Pos;
    output.Norm = vSkinned.Norm;
    output.Tangent = normalize( vSkinned.Tan );
    output.Tex = input.Tex;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Vertex shader used for rendering an already skinned mesh
//--------------------------------------------------------------------------------------
PSSkinnedIn VSRenderPostTransformedmain(VSStreamOut input )
{
    PSSkinnedIn output;
    
    output.Pos = mul( input.Pos, g_mWorldViewProj );
    output.vPos = input.Pos;
    output.Norm = normalize( mul( input.Norm, (float3x3)g_mWorld ) );
    output.Tangent = normalize( mul( input.Tangent, (float3x3)g_mWorld ) );
    output.Tex = input.Tex;
    
    return output;
}
#endif
//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
//--------------------------------------------------------------------------------------
[Fragment shader]
float4 //PSSkinnedmain
	main(PSSkinnedIn input) : COLOR0
{
#if 0
    float4 diffuse = g_txDiffuse.Sample( g_samLinear, input.Tex );
    float3 Norm = g_txNormal.Sample( g_samLinear, input.Tex );
#else // ???
    float4 diffuse = tex2D(g_txDiffuse, input.Tex );
    float3 Norm = tex2D(g_txNormal, input.Tex ).xyz;
#endif
    Norm *= 2.0;
    Norm -= float3(1,1,1);
    
    // Create TBN float4x4
    float3 lightDir = normalize( g_vLightPos - input.vPos );
    float3 viewDir = normalize( g_vEyePt - input.vPos );
    float3 BiNorm = normalize( cross( input.Norm, input.Tangent ) );
    float3x3 BTNMatrix = float3x3( BiNorm, input.Tangent, input.Norm );
    Norm = normalize( mul( Norm, BTNMatrix ) ); //world space bump
    
    //diffuse lighting
    float lightAmt = saturate( dot( lightDir, Norm ) );
    float4 lightColor = lightAmt.xxxx*g_directional + g_ambient;

    // Calculate specular power
    float3 halfAngle = normalize( viewDir + lightDir );
    float4 spec = pow( saturate(dot( halfAngle, Norm )), 64 );
        
    // Return combined lighting
    return lightColor*diffuse + spec*g_objectspeccolor*diffuse.a;
}

#if 0
#if 0
///////////////////////////////////////////////////////////
//This is how we stream out straight from the VS
///////////////////////////////////////////////////////////
VertexShader vsConstantBuffer = CompileShader( vs_4_0, VSSkinnedStreamOutmain( FT_CONSTANTBUFFER ) );
GeometryShader vsConstantBufferSO = ConstructGSWithSO( vsConstantBuffer, "POSITION.xyzw; NORMAL.xyz; TEXCOORD.xy; TANGENT.xyz" );

VertexShader vsTextureBuffer = CompileShader( vs_4_0, VSSkinnedStreamOutmain( FT_TEXTUREBUFFER ) );
GeometryShader vsTextureBufferSO = ConstructGSWithSO( vsTextureBuffer, "POSITION.xyzw; NORMAL.xyz; TEXCOORD.xy; TANGENT.xyz" );

VertexShader vsTexture = CompileShader( vs_4_0, VSSkinnedStreamOutmain( FT_TEXTURE ) );
GeometryShader vsTextureSO = ConstructGSWithSO( vsTexture, "POSITION.xyzw; NORMAL.xyz; TEXCOORD.xy; TANGENT.xyz" );

VertexShader vsBuffer = CompileShader( vs_4_0, VSSkinnedStreamOutmain( FT_BUFFER ) );
GeometryShader vsBufferSO = ConstructGSWithSO( vsBuffer, "POSITION.xyzw; NORMAL.xyz; TEXCOORD.xy; TANGENT.xyz" );
#endif
[techniques]
<!--
//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
SamplerState g_samLinear
{
    Filter = ANISOTROPIC;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState g_samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

//--------------------------------------------------------------------------------------
// State
//--------------------------------------------------------------------------------------
DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};
-->
<Textures>
	    <Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<!--
//--------------------------------------------------------------------------------------
// Render the scene by fetching bone matrices from a constant buffer
//--------------------------------------------------------------------------------------
-->
<!-- technique name="RenderConstantBuffer">
    <pass name="P0">
        <VertexShader profile="vs_4_0" name="VSSkinnedmain">
		<arg type="int" value="0" />
	</VertexShader>
        <GeometryShader name="" />
        <PixelShader profile="ps_4_0" name="PSSkinnedmain" />
        <DepthStencilState name="EnableDepth" X="0" />
    </pass>
</technique -->
<!--
//--------------------------------------------------------------------------------------
// Render the scene by fetching bone matrices from a texture buffer
//--------------------------------------------------------------------------------------
-->
<!-- technique name="RenderTextureBuffer">
    <pass name="P0">
        <VertexShader profile="vs_4_0" name="VSSkinnedmain">
		<arg type="int" value="1" />
	</VertexShader>
        <GeometryShader name="" />
        <PixelShader profile="ps_4_0" name="PSSkinnedmain" />        
        <DepthStencilState name="EnableDepth" X="0" />
    </pass>
</technique -->
<!--
//--------------------------------------------------------------------------------------
// Render the scene by fetching bone matrices from a texture
//--------------------------------------------------------------------------------------
-->
<technique name="RenderTexture">
    <pass name="P0">
        <VertexShader profile="vs_4_0" name="VSSkinnedmain">
		<!-- arg type="int" value="2" / -->
	</VertexShader>
        <GeometryShader name="" />
        <PixelShader profile="ps_4_0" name="PSSkinnedmain" />
        <DepthStencilState name="EnableDepth" X="0" />
    </pass>
</technique>
<!--
//--------------------------------------------------------------------------------------
// Render the scene by fetching bone matrices from a buffer
//--------------------------------------------------------------------------------------
-->
<!-- technique name="RenderBuffer">
    <pass name="P0">
        <VertexShader profile="vs_4_0" name="VSSkinnedmain">
		<arg type="int" value="3" />
	</VertexShader>
        <GeometryShader name="" />
        <PixelShader profile="ps_4_0" name="PSSkinnedmain" />
        <DepthStencilState name="EnableDepth" X="0" />
    </pass>
</technique -->
<!--
//--------------------------------------------------------------------------------------
// Stream out the scene by fetching bone matrices from a constant buffer (StreamOut)
//--------------------------------------------------------------------------------------
-->
<technique name="RenderConstantBuffer_SO">
    <pass name="P0">
        <VertexShader name="vsConstantBuffer" />
        <GeometryShader name="vsConstantBufferSO" />
        <PixelShader name="" />
        <DepthStencilState name="DisableDepth" X="0" />
    </pass>
</technique>
<!--
//--------------------------------------------------------------------------------------
// Stream out the scene by fetching bone matrices from a texture buffer
//--------------------------------------------------------------------------------------
-->
<technique name="RenderTextureBuffer_SO">
    <pass name="P0">
        <VertexShader name="vsTextureBuffer" />
        <GeometryShader name="vsTextureBufferSO" />
        <PixelShader name="" />
        <DepthStencilState name="DisableDepth" X="0" />
    </pass>
</technique>
<!--
//--------------------------------------------------------------------------------------
// Stream out the scene by fetching bone matrices from a texture
//--------------------------------------------------------------------------------------
-->
<technique name="RenderTexture_SO">
    <pass name="P0">	
        <VertexShader name="vsTexture" />
        <GeometryShader name="vsTextureSO" />
        <PixelShader name="" />
        <DepthStencilState name="DisableDepth" X="0" />
    </pass>
</technique>
<!--
//--------------------------------------------------------------------------------------
// Stream out the scene by fetching bone matrices from a buffer
//--------------------------------------------------------------------------------------
-->
<technique name="RenderBuffer_SO">
    <pass name="P0">
        <VertexShader name="vsBuffer" />
        <GeometryShader name="vsBufferSO" />
        <PixelShader name="" />
        <DepthStencilState name="DisableDepth" X="0" />
    </pass>
</technique>
<!--
//--------------------------------------------------------------------------------------
// Render the scene by fetching bone matrices from a buffer
//--------------------------------------------------------------------------------------
-->
<technique name="RenderPostTransformed">
    <pass name="P0">
        <VertexShader profile="vs_4_0" name="VSRenderPostTransformedmain" />
        <GeometryShader name="" />
        <PixelShader profile="ps_4_0" name="PSSkinnedmain" />        
        <DepthStencilState name="EnableDepth" X="0" />
	<VertexFormat>
        	<FormatDesc Type="VERTEX" 	Format="FLOAT2" />
		<FormatDesc Type="TEXCOORD"	Format="FLOAT2" />
	</VertexFormat>
    </pass>
</technique>
</Techniques>
#endif
