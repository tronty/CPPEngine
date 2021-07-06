//--------------------------------------------------------------------------------------
// File: POM.hlsl
//
// HLSL file containing shader functions for Parallax Occlusion Mapping.
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
// Structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 inPositionOS  : POSITION;
    float2 inTexCoord    : TEXCOORD0;
    float3 vInNormalOS   : NORMAL;
    float3 vInBinormalOS : BINORMAL;
    float3 vInTangentOS  : TANGENT;
};

struct VS_OUTPUT
{
    float2 texCoord          : TEXCOORD0;
    float3 vLightTS          : TEXCOORD1;   // Light vector in tangent space, denormalized
    float3 vViewTS           : TEXCOORD2;   // View vector in tangent space, denormalized
    float2 vParallaxOffsetTS : TEXCOORD3;   // Parallax offset vector in tangent space
    float3 vNormalWS         : TEXCOORD4;   // Normal vector in world space
    float3 vViewWS           : TEXCOORD5;   // View vector in world space
    
    float4 position          : SV_POSITION; // Output position
};

struct PS_INPUT
{
   float2 texCoord          : TEXCOORD0;
   float3 vLightTS          : TEXCOORD1;   // Light vector in tangent space, denormalized
   float3 vViewTS           : TEXCOORD2;   // View vector in tangent space, denormalized
   float2 vParallaxOffsetTS : TEXCOORD3;   // Parallax offset vector in tangent space
   float3 vNormalWS         : TEXCOORD4;   // Normal vector in world space
   float3 vViewWS           : TEXCOORD5;   // View vector in world space
};


//--------------------------------------------------------------------------------------
// Vertex shader for POM setup
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS( VS_INPUT i )
{
    VS_OUTPUT Out;
        
    // Transform and output input position 
    Out.position = mul( float4( i.inPositionOS.xyz, 1.0 ), g_mWorldViewProjection );
       
    // Propagate texture coordinate through:
    Out.texCoord = i.inTexCoord * g_fBaseTextureRepeat.x;

    // Transform the normal, tangent and binormal vectors 
    // from object space to homogeneous projection space:
    float3 vNormalWS   = mul( i.vInNormalOS,   (float3x3) g_mWorld );
    float3 vBinormalWS = mul( i.vInBinormalOS, (float3x3) g_mWorld );
    float3 vTangentWS  = mul( i.vInTangentOS,  (float3x3) g_mWorld );
    
    // Propagate the world space vertex normal through:   
    Out.vNormalWS = vNormalWS;
    
    // Normalize tangent space vectors
    vNormalWS   = normalize( vNormalWS );
    vBinormalWS = normalize( vBinormalWS );
    vTangentWS  = normalize( vTangentWS );
    
    // Compute position in world space:
    float4 vPositionWS = mul( i.inPositionOS, g_mWorld );
                 
    // Compute and output the world view vector (unnormalized):
    float3 vViewWS = g_vEye - vPositionWS;
    Out.vViewWS = vViewWS;

    // Compute denormalized light vector in world space:
    float3 vLightWS = g_LightPosition.xyz - vPositionWS.xyz;
    // Need to invert Z for correct lighting
    vLightWS.z = -vLightWS.z;
    
    // Normalize the light and view vectors and transform it to the tangent space:
    float3x3 mWorldToTangent = float3x3( vTangentWS, vBinormalWS, vNormalWS );
       
    // Propagate the view and the light vectors (in tangent space):
    Out.vLightTS = mul( mWorldToTangent, vLightWS );
    Out.vViewTS  = mul( mWorldToTangent, vViewWS  );
       
    // Compute the ray direction for intersecting the 
    // height field profile with current view ray
         
    // Compute initial parallax displacement direction:
    float2 vParallaxDirection = normalize(  Out.vViewTS.xy );
       
    // The length of this vector determines the furthest amount of displacement:
    float fLength         = length( Out.vViewTS );
    float fParallaxLength = sqrt( fLength * fLength - Out.vViewTS.z * Out.vViewTS.z ) / Out.vViewTS.z; 
       
    // Compute the actual reverse parallax displacement vector:
    Out.vParallaxOffsetTS = vParallaxDirection * fParallaxLength;
       
    // Need to scale the amount of displacement to account for different height ranges in height maps.
    Out.vParallaxOffsetTS *= g_fPOMHeightMapScale.x;

   return Out;
}   

//--------------------------------------------------------------------------------------
// Parallax occlusion mapping pixel shader
//--------------------------------------------------------------------------------------
float4 RenderScenePS( PS_INPUT i ) : SV_TARGET
{ 
   //  Normalize the interpolated vectors:
   float3 vViewTS   = normalize( i.vViewTS  );
   float3 vViewWS   = normalize( i.vViewWS  );
   float3 vLightTS  = normalize( i.vLightTS );
   float3 vNormalWS = normalize( i.vNormalWS );
     
   float4 cResultColor = float4( 0, 0, 0, 1 );

   // Compute all the derivatives:
   float2 dx = ddx( i.texCoord );
   float2 dy = ddy( i.texCoord );
                  
   //===============================================//
   // Parallax occlusion mapping offset computation //
   //===============================================//

   // Utilize dynamic flow control to change the number of samples per ray 
   // depending on the viewing angle for the surface. Oblique angles require 
   // smaller step sizes to achieve more accurate precision for computing displacement.
   // We express the sampling rate as a linear function of the angle between 
   // the geometric normal and the view direction ray:
   int nNumSteps = (int)lerp( g_nMaxSamples, g_nMinSamples, dot( vViewWS, vNormalWS ) );

   // Intersect the view ray with the height field profile along the direction of
   // the parallax offset ray (computed in the vertex shader. Note that the code is
   // designed specifically to take advantage of the dynamic flow control constructs
   // in HLSL and is very sensitive to specific syntax. When converting to other examples,
   // if still want to use dynamic flow control in the resulting assembly shader,
   // care must be applied.
   // 
   // In the below steps we approximate the height field profile as piecewise linear
   // curve. We find the pair of endpoints between which the intersection between the 
   // height field profile and the view ray is found and then compute line segment
   // intersection for the view ray and the line segment formed by the two endpoints.
   // This intersection is the displacement offset from the original texture coordinate.
   
   float fCurrHeight = 0.0;
   float fStepSize   = 1.0 / (float) nNumSteps;
   float fPrevHeight = 1.0;
   float fNextHeight = 0.0;

   int    nStepIndex = 0;
   bool   bCondition = true;

   float2 vTexOffsetPerStep = fStepSize * i.vParallaxOffsetTS;
   float2 vTexCurrentOffset = i.texCoord;
   float  fCurrentBound     = 1.0;
   float  fParallaxAmount   = 0.0;

   float2 pt1 = 0;
   float2 pt2 = 0;

   float2 texOffset2 = 0;
   
   while ( nStepIndex < nNumSteps ) 
   {
      vTexCurrentOffset -= vTexOffsetPerStep;

      // Sample height map which in this case is stored in the alpha channel of the normal map:
      fCurrHeight = g_nmhTexture.SampleGrad( g_samLinear, vTexCurrentOffset, dx, dy ).a;

      fCurrentBound -= fStepSize;

      if ( fCurrHeight > fCurrentBound ) 
      {     
         pt1 = float2( fCurrentBound, fCurrHeight );
         pt2 = float2( fCurrentBound + fStepSize, fPrevHeight );

         texOffset2 = vTexCurrentOffset - vTexOffsetPerStep;

         nStepIndex = nNumSteps + 1;
      }
      else
      {
         nStepIndex++;
         fPrevHeight = fCurrHeight;
      }
   }   // End of while ( nStepIndex < nNumSteps )
   
   float fDelta2 = pt2.x - pt2.y;
   float fDelta1 = pt1.x - pt1.y;
   float fDenominator = fDelta2 - fDelta1;
      
   // SM 3.0 and above requires a check for divide by zero since that operation will generate an 'Inf' number instead of 0
   [flatten]if ( fDenominator == 0.0f ) 
   {
       fParallaxAmount = 0.0f;
   }
   else
   {
       fParallaxAmount = ( pt1.x * fDelta2 - pt2.x * fDelta1 ) / fDenominator;
   }
   
   float2 vParallaxOffset = i.vParallaxOffsetTS * ( 1.0 - fParallaxAmount );

   // The computed texture offset for the displaced point on the pseudo-extruded surface:
   float2 texSample = i.texCoord - vParallaxOffset;
   
   // Compute resulting color for the pixel:
   cResultColor = ComputeIllumination( texSample, vLightTS, vViewTS );
   
   return cResultColor;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
[techniques]
<Textures>
	    <Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="RenderScene">
    <pass name="P0">
	<PixelShader profile="ps_5_0" name="RenderScenePS" />
    <VertexShader profile="vs_5_0" name="RenderSceneVS" />
    <GeometryShader profile="gs_5_0" name="" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    </pass>
</technique>
</Techniques>

