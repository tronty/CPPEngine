float4x4 mWorldViewProjection;
float4x4 mWorld;
float4x4 mView;
float4x4 mProj;
float4x4 mViewProj;
texture g_pTerrainTexture;              // Color texture for mesh

float SceneWidth;
float SceneHeight;

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
#if 0
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_pTerrainTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
#else
sampler2D MeshTextureSampler;
#endif


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float3 Z : TEXCOORD0;   // vertex position 
};


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderDepthVS( float4 vPos : POSITION)
{
    VS_OUTPUT Output;
    
    float4 WVPPosition = mul(vPos, mWorldViewProjection);
    
    Output.Position = WVPPosition;
           
    // store z again so we can access in the ps
    Output.Z = WVPPosition.zzz; 
   
    return Output;    
}


// Just push the Z value straight through into our R32 target
float4 RenderDepthPS( VS_OUTPUT In) : COLOR
{ 
    return In.Z.xxxx;
}
[techniques]
<Textures>
	    <Texture sampler="MeshTextureSampler" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="RenderDepthPass">
	<pass name="P0">
		<VertexShader profile="vs_1_1" name="RenderDepthVS" />
		<PixelShader profile="ps_1_1" name="RenderDepthPS" />
		<VertexFormat>
        		<FormatDesc Type="VERTEX" 	Format="FLOAT4" />
		</VertexFormat>
	</pass>
</technique>
</Techniques>

