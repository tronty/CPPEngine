//--------------------------------------------------------------------------------------
// File: Particle.hlsl
//
// HLSL file containing shader function to render front-facing particles.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// File: shader_include.hlsl
//
// Include file for common shader definitions and functions.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------                  
#define ADD_SPECULAR 0
                                          
//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------
Texture2D g_baseTexture : register( t0 );    // Base color texture
Texture2D g_nmhTexture :  register( t1 );    // Normal map and height map texture pair


//--------------------------------------------------------------------------------------
// Samplers
//--------------------------------------------------------------------------------------
SamplerState g_samLinear : register( s0 );
SamplerState g_samPoint  : register( s1 );


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer cbMain : register( b0 )
{
	matrix g_mWorld;                            // World matrix
	matrix g_mView;                             // View matrix
	matrix g_mProjection;                       // Projection matrix
    matrix g_mWorldViewProjection;              // WVP matrix
    matrix g_mViewProjection;                   // VP matrix
    matrix g_mInvView;                          // Inverse of view matrix
    
    float4 g_vMeshColor;                        // Mesh color
    float4 g_vTessellationFactor;               // Edge, inside and minimum tessellation factor
    float4 g_vDetailTessellationHeightScale;    // Height scale for detail tessellation of grid surface
    float4 g_vGridSize;                         // Grid size
    
    float4 g_vDebugColorMultiply;               // Debug colors
    float4 g_vDebugColorAdd;                    // Debug colors
};

cbuffer cbMaterial : register( b1 )
{
	float4 	 g_materialAmbientColor;          	// Material's ambient color
	float4 	 g_materialDiffuseColor;          	// Material's diffuse color
	float4 	 g_materialSpecularColor;         	// Material's specular color
	float4 	 g_fSpecularExponent;             	// Material's specular exponent

	float4 	 g_LightPosition;                 	// Light's position in world space
	float4 	 g_LightDiffuse;                  	// Light's diffuse color
	float4 	 g_LightAmbient;                  	// Light's ambient color

	float4 	 g_vEye;					    	// Camera's location
	float4 	 g_fBaseTextureRepeat;		    	// The tiling factor for base and normal map textures
	float4 	 g_fPOMHeightMapScale;		    	// Describes the useful range of values for the height field
	
	float4   g_fShadowSoftening;		    	// Blurring factor for the soft shadows computation
	
	int      g_nMinSamples;				    	// The minimum number of samples for sampling the height field profile
	int      g_nMaxSamples;				    	// The maximum number of samples for sampling the height field profile
};


//--------------------------------------------------------------------------------------
// Function:    ComputeIllumination
// 
// Description: Computes phong illumination for the given pixel using its attribute 
//              textures and a light vector.
//--------------------------------------------------------------------------------------
float4 ComputeIllumination( float2 texCoord, float3 vLightTS, float3 vViewTS )
{
   // Sample the normal from the normal map for the given texture sample:
   float3 vNormalTS = normalize( g_nmhTexture.Sample(g_samLinear, texCoord) * 2.0 - 1.0 );
   
   // Sample base map
   float4 cBaseColor = g_baseTexture.Sample( g_samLinear, texCoord );
   
   // Compute diffuse color component:
   float4 cDiffuse = saturate( dot( vNormalTS, vLightTS ) ) * g_materialDiffuseColor;
   
   // Compute the specular component if desired:  
   float4 cSpecular = 0;

#if ADD_SPECULAR==1
   
   float3 vReflectionTS = normalize( 2 * dot( vViewTS, vNormalTS ) * vNormalTS - vViewTS );
   float fRdotL = saturate( dot( vReflectionTS, vLightTS ) );
   cSpecular = pow( fRdotL, g_fSpecularExponent.x ) * g_materialSpecularColor;

#endif
   
   // Composite the final color:
   float4 cFinalColor = ( g_materialAmbientColor + cDiffuse ) * cBaseColor + cSpecular; 
   
   return cFinalColor;  
}  

//--------------------------------------------------------------------------------------
// Internal defines
//--------------------------------------------------------------------------------------
#define FIXED_VERTEX_RADIUS 5.0

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct VS_PARTICLE_INPUT
{
    float3 WSPos : POSITION;
};

struct GS_PARTICLE_INPUT
{
    float4 WSPos : POSITION;
};

struct PS_PARTICLE_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader to GS
//--------------------------------------------------------------------------------------
GS_PARTICLE_INPUT VSPassThrough( VS_PARTICLE_INPUT input )
{
    GS_PARTICLE_INPUT output = (GS_PARTICLE_INPUT)0;
    
    // Pass world space position to GS
    output.WSPos = float4( input.WSPos, 1.0 );
    
    return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader to render point sprites
//--------------------------------------------------------------------------------------
[maxvertexcount(4)]
void GSPointSprite(point GS_PARTICLE_INPUT input[1], inout TriangleStream<PS_PARTICLE_INPUT> SpriteStream)
{
    const float3 g_positions[4] =
    {
        float3( -1.0,  1.0, 0.0 ),
        float3(  1.0,  1.0, 0.0 ),
        float3( -1.0, -1.0, 0.0 ),
        float3(  1.0, -1.0, 0.0 ),
    };
    const float2 g_texcoords[4] = 
    { 
        float2( 0.0, 1.0 ), 
        float2( 1.0, 1.0 ),
        float2( 0.0, 0.0 ),
        float2( 1.0, 0.0 ),
    };
    PS_PARTICLE_INPUT output = (PS_PARTICLE_INPUT)0;
    
    // Emit two new triangles
    [unroll]for( int i=0; i<4; ++i )
    {
        float3 position = g_positions[i] * FIXED_VERTEX_RADIUS;
        position = mul( position, (float3x3)g_mInvView ) + input[0].WSPos;
        output.Pos = mul( float4( position, 1.0 ), g_mViewProjection );

        // Pass texture coordinates
        output.Tex = g_texcoords[i];
        
        // Add vertex
        SpriteStream.Append( output );
    }
    SpriteStream.RestartStrip();
}

//--------------------------------------------------------------------------------------
// Pixel Shader to display constant single color
//--------------------------------------------------------------------------------------
float4 PSConstantColor( PS_PARTICLE_INPUT input ) : SV_TARGET
{
    // Sample particle texture
    float4 vColor = g_baseTexture.Sample( g_samLinear, input.Tex ).wwww;
    
    // Clip fully transparent pixels
    clip( vColor.a - 1.0/255.0 );
    
    // Return color
    return vColor;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
[techniques]
<Textures>
	    <Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="Particle">
    <pass name="P0">
    <VertexShader profile="vs_5_0" name="VSPassThrough" />
	<PixelShader profile="ps_5_0" name="PSConstantColor" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    <GeometryShader profile="gs_5_0" name="GSPointSprite" />
    <ComputeShader profile="cs_5_0" name="" />
    </pass>
</technique>
</Techniques>

