//----------------------------------------------------------------------------------
// File:   Clipmaps.fx
// Author: Evgeny Makarov
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

Texture2D PyramidTexture;
Texture2D PyramidTextureHM;
Texture2DArray StackTexture;

#define MAX_ANISOTROPY 16
#define MIP_LEVELS_MAX 7

SamplerState samplerLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samplerAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = MAX_ANISOTROPY;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samplerPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samplerStackLinear
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState RStateMSAA
{
    MultisampleEnable = TRUE;
};

struct VSIn
{
    uint index : SV_VertexID;
};

struct PSIn
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
    float3 viewVectorTangent : TEXCOORD1;
    float3 lightVectorTangent : TEXCOORD2;
};

struct PSInQuad
{
    float4 position : SV_Position;
    float3 texCoord : TEXCOORD0;
};

struct PSOut
{
    float4 color : SV_Target;
};

struct PSOutQuad
{
    float4 color : SV_Target;
};

cbuffer cb0
{
    row_major float4x4 g_ModelViewProj;
    float3 g_EyePosition;
    float3 g_LightPosition;
    float3 g_WorldRight;
    float3 g_WorldUp;
};

cbuffer cb1
{
    int2    g_TextureSize;        // Source texture size
    float2  g_StackCenter;        // Stack center position defined by normalized texture coordinates
    uint    g_StackDepth;         // Number of layers in a stack
    float2  g_ScaleFactor;        // SourceImageSize / ClipmapStackSize
    float3  g_MipColors[MIP_LEVELS_MAX];
    uint     g_SphereMeridianSlices;
    uint     g_SphereParallelSlices;
    float   g_ScreenAspectRatio;
}


//--------------------------------------------------------------------------------------
// Calculate local normal using height values from Texture2D
//--------------------------------------------------------------------------------------
float3 GetLocalNormal(Texture2D _texture, SamplerState _sampler, float2 _coordinates)
{
    float3 localNormal;
    
    localNormal.x =  _texture.Sample( _sampler, _coordinates, int2( 1,  0) ).x;
    localNormal.x -= _texture.Sample( _sampler, _coordinates, int2(-1,  0) ).x;
    localNormal.y =  _texture.Sample( _sampler, _coordinates, int2( 0,  1) ).x;
    localNormal.y -= _texture.Sample( _sampler, _coordinates, int2( 0, -1) ).x;
    localNormal.z = sqrt( 1.0 - localNormal.x * localNormal.x - localNormal.y * localNormal.y );
    
    return localNormal;
}


//--------------------------------------------------------------------------------------
// Calculate local normal using height values from Texture2DArray
//--------------------------------------------------------------------------------------
float3 GetLocalNormal_Array(Texture2DArray _texture, SamplerState _sampler, float3 _coordinates)
{
    float3 localNormal;
    
    localNormal.x =  _texture.Sample( _sampler, _coordinates, int2( 1,  0) ).w;
    localNormal.x -= _texture.Sample( _sampler, _coordinates, int2(-1,  0) ).w;
    localNormal.y =  _texture.Sample( _sampler, _coordinates, int2( 0,  1) ).w;
    localNormal.y -= _texture.Sample( _sampler, _coordinates, int2( 0, -1) ).w;
    localNormal.xy *= 5.0 / ( _coordinates.z + 1.0 ); // Scale the normal vector to add relief
    localNormal.z = sqrt( 1.0 - localNormal.x * localNormal.x - localNormal.y * localNormal.y );
     
    return localNormal;
}


//--------------------------------------------------------------------------------------
// Calculate a minimum stack level to fetch from
//--------------------------------------------------------------------------------------
int GetMinimumStackLevel(float2 coordinates)
{
    float2 distance;
    
    distance.x = abs( coordinates.x - g_StackCenter.x );
    distance.x = min( distance.x, 1.0 - distance.x );
    
    distance.y = abs( coordinates.y - g_StackCenter.y );
    distance.y = min( distance.y, 1.0 - distance.y );
            
    return max( log2( distance.x * g_ScaleFactor.x * 4.2 ), log2( distance.y * g_ScaleFactor.y * 4.2 ) );
}


//--------------------------------------------------------------------------------------
// Calculate vertex positions for procedural sphere mesh based on an input index buffer
//--------------------------------------------------------------------------------------
PSIn VSMain(VSIn input)
{
    PSIn output;
    
    float meridianPart = ( input.index % ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereMeridianSlices );
    float parallelPart = ( input.index / ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereParallelSlices );
    
    float angle1 = meridianPart * 3.14159265 * 2.0;
    float angle2 = ( parallelPart - 0.5 ) * 3.14159265;
    
    float cos_angle1 = cos( angle1 );
    float sin_angle1 = sin( angle1 );
    float cos_angle2 = cos( angle2 );
    float sin_angle2 = sin( angle2 );
        
    float3 VertexPosition;
    VertexPosition.z = cos_angle1 * cos_angle2;
    VertexPosition.x = sin_angle1 * cos_angle2;
    VertexPosition.y = sin_angle2;
    
    output.position = mul( float4( VertexPosition, 1.0 ), g_ModelViewProj );
    output.texCoord = float2( 1.0 - meridianPart, 1.0 - parallelPart );
    
    float3 tangent = float3( cos_angle1, 0.0, -sin_angle1 );
    float3 binormal = float3( -sin_angle1 * sin_angle2, cos_angle2, -cos_angle1 * sin_angle2 );
        
    float3 viewVector = normalize(g_EyePosition - VertexPosition);
    
    output.viewVectorTangent.x = dot( viewVector, tangent );
    output.viewVectorTangent.y = dot( viewVector, binormal);
    output.viewVectorTangent.z = dot( viewVector, VertexPosition );
    
    float3 lightVector = normalize( g_LightPosition );
    
    output.lightVectorTangent.x = dot( lightVector, tangent );
    output.lightVectorTangent.y = dot( lightVector, binormal);
    output.lightVectorTangent.z = dot( lightVector, VertexPosition );
    
    return output;
}


PSInQuad VSMainQuad(VSIn input)
{
    PSInQuad output;
    
    // We don't need to do any calculations here because everything
    // is done in the geometry shader.
    output.position = 0;
    output.texCoord = 0;
        
    return output;
}


[maxvertexcount(4)]
void GSMainQuad( point PSInQuad inputPoint[1], inout TriangleStream<PSInQuad> outputQuad, uint primitive : SV_PrimitiveID )
{
    PSInQuad output;
    
    output.position.z = 0.5;
    output.position.w = 1.0;
    
    output.texCoord.z = primitive;
    
    float sizeY = 0.3;
    float sizeX = sizeY * 1.2 / g_ScreenAspectRatio;
    
    float offset = 0.7 - min( 1.2 / g_StackDepth, sizeY ) * primitive;
        
    output.position.x = -0.9 - sizeX * 0.2;
    output.position.y = offset;
    output.texCoord.xy = float2( 0.0, 0.0 );
    outputQuad.Append( output );
    
    output.position.x = -0.9 + sizeX * 0.8;
    output.position.y = offset + sizeY * 0.2;
    output.texCoord.xy = float2( 1.0, 0.0 );
    outputQuad.Append( output );
    
    output.position.x = -0.9;
    output.position.y = offset - sizeY - sizeY * 0.2;
    output.texCoord.xy = float2( 0.0, 1.0 );
    outputQuad.Append( output );
        
    output.position.x = -0.9 + sizeX;
    output.position.y = offset - sizeY;
    output.texCoord.xy = float2( 1.0, 1.0 );
    outputQuad.Append( output );
    
    outputQuad.RestartStrip();
}


PSOut PS_Trilinear(PSIn input)
{
    PSOut output;
    
    // Calculate texture coordinates gradients.
	float2 pixelCoord = float2( input.texCoord.x * g_TextureSize.x, input.texCoord.y * g_TextureSize.y );
    float2 dx = ddx( pixelCoord );
    float2 dy = ddy( pixelCoord );
    float d = max( length( dx ), length( dy ) );
    
    // Calculate base mip level and fractional blending part for trilinear filtering.
    float mipLevel = max( log2( d ), GetMinimumStackLevel( input.texCoord ) );
    float blendGlobal = saturate(g_StackDepth - mipLevel);
            
    float diffuse = saturate( input.lightVectorTangent.z );
    diffuse = max( diffuse, 0.05 );
    
    float4 color0 = PyramidTexture.Sample( samplerLinear, input.texCoord );
    
    // Make early out for cases where we don't need to fetch from clipmap stack
    if( blendGlobal == 0.0 )
    {
        output.color = color0 * diffuse;
    }
    else
    {
        // This fractional part defines the factor used for blending
        // between two neighbour stack layers
        float blendLayers = modf(mipLevel, mipLevel);
        blendLayers = saturate(blendLayers);
        
        int nextMipLevel = mipLevel + 1;
        nextMipLevel = clamp( nextMipLevel, 0, g_StackDepth - 1 );
        mipLevel = clamp( mipLevel, 0, g_StackDepth - 1 );
            
        // Here we need to perform proper scaling for input texture coordinates.
        // For each layer we multiply input coordinates by g_ScaleFactor / pow( 2, layer ).
        // We add 0.5 to result, because our stack center with coordinates (0.5, 0.5)
        // starts from corner with coordinates (0, 0) of the original image.
        float2 clipTexCoord = input.texCoord / pow( 2, mipLevel );
        clipTexCoord *= g_ScaleFactor;
        float4 color1 = StackTexture.Sample( samplerStackLinear, float3( clipTexCoord + 0.5, mipLevel ) );
            
        clipTexCoord = input.texCoord / pow( 2, nextMipLevel );
        clipTexCoord *= g_ScaleFactor;
        float4 color2 = StackTexture.Sample( samplerStackLinear, float3( clipTexCoord + 0.5, nextMipLevel ) );
        
        output.color = lerp( color0, lerp( color1, color2, blendLayers ), blendGlobal ) * diffuse;
    }
    
    return output;
}


PSOut PS_Trilinear_Parallax(PSIn input)
{
    PSOut output;
    
    // Calculate texture coordinates gradients.
	float2 pixelCoord = float2( input.texCoord.x * g_TextureSize.x, input.texCoord.y * g_TextureSize.y );
    float2 dx = ddx( pixelCoord );
    float2 dy = ddy( pixelCoord );
    float d = max( length( dx ), length( dy ) );
        
    // Calculate base mip level and fractional blending part.
    float mipLevel = max( log2( d ), GetMinimumStackLevel( input.texCoord ) );
    float blendGlobal = saturate( g_StackDepth - mipLevel );
            
    float2 viewVectorTangent = normalize( input.viewVectorTangent ).xy;
    float2 scaledViewVector = viewVectorTangent / g_ScaleFactor;
    
    float3 lightVector = normalize(input.lightVectorTangent);
        
    float2 newCoordinates = input.texCoord - scaledViewVector * ( PyramidTextureHM.Sample( samplerLinear, input.texCoord ).x * 0.02 - 0.01 );
    float3 normal = GetLocalNormal( PyramidTextureHM, samplerLinear, newCoordinates );
    float diffuse = saturate( dot( lightVector, normal ) );
    diffuse = max( diffuse, 0.05 );
        
    float4 color0 = PyramidTexture.Sample( samplerLinear, newCoordinates ) * diffuse;
    
    if( blendGlobal == 0.0 )
    {
        output.color = color0;
    }
    else
    {
        float blendLayers = modf( mipLevel, mipLevel );
        blendLayers = saturate( blendLayers );
        
        int nextMipLevel = mipLevel + 1;
        nextMipLevel = clamp( nextMipLevel, 0, g_StackDepth - 1 );
        mipLevel = clamp( mipLevel, 0, g_StackDepth - 1 );
            
        float scale = pow( 2, mipLevel );
        
        float2 clipTexCoord = input.texCoord / scale;
        clipTexCoord *= g_ScaleFactor;
        
        float height = StackTexture.Sample( samplerStackLinear, float3(clipTexCoord + 0.5, mipLevel) ).w * 0.02 - 0.01;
        
        newCoordinates = clipTexCoord - viewVectorTangent * height / scale + 0.5;
        float4 color1 = StackTexture.Sample( samplerStackLinear, float3( newCoordinates, mipLevel ) );
        
        normal = GetLocalNormal_Array( StackTexture, samplerStackLinear, float3( newCoordinates, mipLevel ) );
        diffuse = saturate( dot( lightVector, normal ) );
        diffuse = max( diffuse, 0.05 );
        color1 *= diffuse;
        
        scale = pow( 2, nextMipLevel );
        
        clipTexCoord = input.texCoord / scale;
        clipTexCoord *= g_ScaleFactor;
        
        height = StackTexture.Sample( samplerStackLinear, float3( clipTexCoord + 0.5, nextMipLevel ) ).w * 0.02 - 0.01;
        
        newCoordinates = clipTexCoord - viewVectorTangent * height / scale + 0.5;
        float4 color2 = StackTexture.Sample( samplerStackLinear, float3( newCoordinates, nextMipLevel ) );
        
        normal = GetLocalNormal_Array( StackTexture, samplerStackLinear, float3( newCoordinates, nextMipLevel ) );
        diffuse = saturate( dot( lightVector, normal ) );
        diffuse = max( diffuse, 0.05 );
        color2 *= diffuse;
        
        output.color = lerp( color0, lerp( color1, color2, blendLayers ), blendGlobal );
    }
        
    return output;
}


PSOut PS_Anisotropic(PSIn input)
{
    PSOut output;
    
    // Calculate texture coordinates gradients.
	float2 pixelCoord = float2( input.texCoord.x * g_TextureSize.x, input.texCoord.y * g_TextureSize.y );
    float2 dx = ddx( pixelCoord );
    float2 dy = ddy( pixelCoord );
    
    float squaredLengthX = dx.x * dx.x + dx.y * dx.y;
    float squaredLengthY = dy.x * dy.x + dy.y * dy.y;
    
    float det = abs(dx.x * dy.y - dx.y * dy.x);
    
    float squaredLengthMajor = max( squaredLengthX, squaredLengthY );
    float lengthMajor = sqrt(squaredLengthMajor);
    float ratioOfAnisotropy = squaredLengthMajor / det;
    float lengthMinor = 0;
    
    lengthMinor = ( ratioOfAnisotropy > MAX_ANISOTROPY ) ? lengthMajor / ratioOfAnisotropy : det / lengthMajor;
    lengthMinor = max( lengthMinor, 1.0 );        
    
    // Calculate base mip level and fractional blending part.
    float mipLevel = max( log2( lengthMinor ), GetMinimumStackLevel( input.texCoord ) );
    float blendGlobal = saturate( g_StackDepth - mipLevel );

    float diffuse = saturate( input.lightVectorTangent.z );
    diffuse = max( diffuse, 0.05 );
    
    float4 color0 = PyramidTexture.Sample( samplerAnisotropic, input.texCoord );

    if( blendGlobal == 0.0 )
    {
        output.color = color0 * diffuse;
    }
    else
    {
        float blendLayers = modf( mipLevel, mipLevel );
            
        int nextMipLevel = mipLevel + 1;
        nextMipLevel = clamp( nextMipLevel, 0, g_StackDepth - 1 );
        mipLevel = clamp( mipLevel, 0, g_StackDepth - 1 );
                
        float2 clipTexCoord = input.texCoord / pow( 2, mipLevel );
        clipTexCoord *= g_ScaleFactor;
        float4 color1 = StackTexture.Sample( samplerAnisotropic, float3( clipTexCoord + 0.5, mipLevel ) );
                    
        clipTexCoord = input.texCoord / pow( 2, nextMipLevel );
        clipTexCoord *= g_ScaleFactor;
        float4 color2 = StackTexture.Sample( samplerAnisotropic, float3(clipTexCoord + 0.5, nextMipLevel ) );
        
        output.color = lerp( color0, lerp( color1, color2, blendLayers ), blendGlobal ) * diffuse;
    }
        
    return output;
}


PSOut PS_Anisotropic_Parallax(PSIn input)
{
    PSOut output;
    
    // Calculate texture coordinates gradients.
	float2 pixelCoord = float2( input.texCoord.x * g_TextureSize.x, input.texCoord.y * g_TextureSize.y );
    float2 dx = ddx( pixelCoord );
    float2 dy = ddy( pixelCoord );
    
    float squaredLengthX = dx.x * dx.x + dx.y * dx.y;
    float squaredLengthY = dy.x * dy.x + dy.y * dy.y;
    
    float det = abs( dx.x * dy.y - dx.y * dy.x );
    
    float squaredLengthMajor = max( squaredLengthX, squaredLengthY );
    float lengthMajor = sqrt(squaredLengthMajor);
    float ratioOfAnisotropy = squaredLengthMajor / det;
    float lengthMinor = 0;
    
    lengthMinor = ( ratioOfAnisotropy > MAX_ANISOTROPY ) ? lengthMajor / ratioOfAnisotropy : det / lengthMajor;
    lengthMinor = max( lengthMinor, 1.0 );        
    
    // Calculate base mip level and fractional blending part.
    float mipLevel = max( log2( lengthMinor), GetMinimumStackLevel( input.texCoord ) );
    float blendGlobal = saturate( g_StackDepth - mipLevel );

    float2 viewVectorTangent = normalize( input.viewVectorTangent ).xy;
    float2 scaledViewVector = viewVectorTangent / g_ScaleFactor;
    
    float3 lightVector = normalize( input.lightVectorTangent );
        
    float2 newCoordinates = input.texCoord - scaledViewVector * ( PyramidTextureHM.Sample( samplerLinear, input.texCoord ).x * 0.02 - 0.01 );
    float3 normal = GetLocalNormal( PyramidTextureHM, samplerAnisotropic, newCoordinates );
    float diffuse = saturate( dot( lightVector, normal ) );
    diffuse = max( diffuse, 0.05 );
        
    float4 color0 = PyramidTexture.Sample( samplerAnisotropic, newCoordinates ) * diffuse;
    
    if( blendGlobal == 0.0 )
    {
        output.color = color0;
    }
    else
    {
        float blendLayers = modf( mipLevel, mipLevel );
        blendLayers = saturate( blendLayers );
        
        int nextMipLevel = mipLevel + 1;
        nextMipLevel = clamp( nextMipLevel, 0, g_StackDepth - 1 );
        mipLevel = clamp( mipLevel, 0, g_StackDepth - 1 );
            
        float scale = pow( 2, mipLevel );
        
        float2 clipTexCoord = input.texCoord / scale;
        clipTexCoord *= g_ScaleFactor;
        clipTexCoord += 0.5;
        
        float height = StackTexture.Sample( samplerAnisotropic, float3( clipTexCoord, mipLevel ) ).w * 0.02 - 0.01;
        
        newCoordinates = clipTexCoord - viewVectorTangent * height / scale;
        float4 color1 = StackTexture.Sample( samplerAnisotropic, float3( newCoordinates, mipLevel ) );
        
        normal = GetLocalNormal_Array( StackTexture, samplerAnisotropic, float3( newCoordinates, mipLevel ) );
        diffuse = saturate( dot( lightVector, normal ) );
        diffuse = max( diffuse, 0.05 );
        color1 *= diffuse;
        
        scale = pow( 2, nextMipLevel );
        
        clipTexCoord = input.texCoord / scale;
        clipTexCoord *= g_ScaleFactor;
        clipTexCoord += 0.5f;
        
        height = StackTexture.Sample( samplerAnisotropic, float3( clipTexCoord, nextMipLevel ) ).w * 0.02 - 0.01;
        
        newCoordinates = clipTexCoord - viewVectorTangent * height / scale;
        float4 color2 = StackTexture.Sample( samplerAnisotropic, float3( newCoordinates, nextMipLevel ) );
        
        normal = GetLocalNormal_Array( StackTexture, samplerAnisotropic, float3( newCoordinates, nextMipLevel ) );
        diffuse = saturate( dot( lightVector, normal ) );
        diffuse = max( diffuse, 0.05 );
        color2 *= diffuse;
        
        output.color = lerp( color0, lerp( color1, color2, blendLayers ), blendGlobal );
    }
        
    return output;
}


//--------------------------------------------------------------------------------------
// Calculate color values to show
//--------------------------------------------------------------------------------------
PSOut PS_Color(PSIn input)
{
    PSOut output;
    
    // Calculate texture coordinates gradients.
	float2 pixelCoord = float2( input.texCoord.x * g_TextureSize.x, input.texCoord.y * g_TextureSize.y );
    float2 dx = ddx( pixelCoord );
    float2 dy = ddy( pixelCoord );
    float d = max( length( dx ), length( dy ) );
    
    // Calculate base mip level and fractional blending part.
    float mipLevel = log2( d );
    
    float blendLayers = modf( mipLevel, mipLevel );
            
    int mipBoundary = min( g_StackDepth, MIP_LEVELS_MAX ) - 1;
    
    int nextMipLevel = mipLevel + 1;
    nextMipLevel = clamp( nextMipLevel, 0, mipBoundary );
    mipLevel = clamp( mipLevel, 0, mipBoundary );
    
    output.color.xyz = lerp( g_MipColors[mipLevel], g_MipColors[nextMipLevel], blendLayers );
            
    return output;
}

PSOut PSQuad(PSInQuad input)
{
    PSOut output;
    
    float width = 0.995 - input.texCoord.x;
    width = saturate( width * 50.0 );
    
    output.color.xyz = lerp( g_MipColors[input.texCoord.z], StackTexture.Sample( samplerStackLinear, input.texCoord ), width);
    output.color.w = 1.0;           
    
    return output;
}

#if 0
//--------------------------------------------------------------------------------------
// Compiled shaders used in different techniques
//--------------------------------------------------------------------------------------

VertexShader vsCompiled = CompileShader( vs_4_0, VSMain() );
VertexShader vsCompiledQuad = CompileShader( vs_4_0, VSMainQuad() );

GeometryShader gsCompiledQuad = CompileShader( gs_4_0, GSMainQuad() );

PixelShader ps_Trilinear = CompileShader( ps_4_0, PS_Trilinear() );
PixelShader ps_Trilinear_Parallax = CompileShader( ps_4_0, PS_Trilinear_Parallax() );
PixelShader ps_Anisotropic = CompileShader( ps_4_0, PS_Anisotropic() );
PixelShader ps_Anisotropic_Parallax = CompileShader( ps_4_0, PS_Anisotropic_Parallax() );
PixelShader ps_Color = CompileShader( ps_4_0, PS_Color() );
PixelShader psCompiledQuad = CompileShader( ps_4_0, PSQuad() );
#endif

BlendState NoBlending
{
    BlendEnable[0] = FALSE;
};

[techniques]
<Textures>
	    <Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="Trilinear">
    <pass name="p0">
    <VertexShader profile="vs_5_0" name="VSMain" />
	<PixelShader profile="ps_5_0" name="PS_Trilinear" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    <GeometryShader profile="gs_5_0" name="" />
    <ComputeShader profile="cs_5_0" name="" />
        <RasterizerState name="RStateMSAA" />
        <BlendState name="NoBlending" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
    </pass>
    <pass name="p1">
    <VertexShader profile="vs_5_0" name="VSMain" />
	<PixelShader profile="ps_5_0" name="PS_Trilinear_Parallax" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    <GeometryShader profile="gs_5_0" name="" />
    <ComputeShader profile="cs_5_0" name="" />
        <RasterizerState name="RStateMSAA" />
        <BlendState name="NoBlending" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
    </pass>
</technique>
<technique name="Anisotropic">
    <pass name="p0">
    <VertexShader profile="vs_5_0" name="VSMain" />
	<PixelShader profile="ps_5_0" name="PS_Anisotropic" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    <GeometryShader profile="gs_5_0" name="" />
    <ComputeShader profile="cs_5_0" name="" />
        <RasterizerState name="RStateMSAA" />
        <BlendState name="NoBlending" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
    </pass>
    <pass name="p1">
    <VertexShader profile="vs_5_0" name="VSMain" />
	<PixelShader profile="ps_5_0" name="PS_Anisotropic_Parallax" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    <GeometryShader profile="gs_5_0" name="" />
    <ComputeShader profile="cs_5_0" name="" />
        <RasterizerState name="RStateMSAA" />
        <BlendState name="NoBlending" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
    </pass>
</technique>
<technique name="ColoredMips">
    <pass name="p0">
    <VertexShader profile="vs_5_0" name="VSMain" />
	<PixelShader profile="ps_5_0" name="PS_Color" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    <GeometryShader profile="gs_5_0" name="" />
    <ComputeShader profile="cs_5_0" name="" />
        <RasterizerState name="RStateMSAA" />
        <BlendState name="NoBlending" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
    </pass>
</technique>
<technique name="StackDrawPass">
    <pass name="p0">
    <VertexShader profile="vs_5_0" name="VSMainQuad" />
	<PixelShader profile="ps_5_0" name="PSQuad" />
    <HullShader profile="hs_5_0" name="" />
    <DomainShader profile="ds_5_0" name="" />
    <GeometryShader profile="gs_5_0" name="GSMainQuad" />
    <ComputeShader profile="cs_5_0" name="" />
        <RasterizerState name="RStateMSAA" />
        <BlendState name="NoBlending" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
    </pass>
</technique>
</Techniques>

