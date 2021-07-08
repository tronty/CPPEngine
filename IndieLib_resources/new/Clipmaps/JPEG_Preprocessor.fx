//----------------------------------------------------------------------------------
// File:   JPEG_Preprocessor.fx
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

Texture2D TextureDCT;
Texture2D QuantTexture;
Texture2D RowTexture1;
Texture2D RowTexture2;
Texture2D ColumnTexture1;
Texture2D ColumnTexture2;
Texture2D TargetTexture;

Texture2D TextureY;
Texture2D TextureCb;
Texture2D TextureCr;
Texture2D TextureHeight;

SamplerState samplerPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samplerLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VSIn
{
    uint index : SV_VertexID;
};

struct PSIn
{
    float4 position : SV_Position;
    float3 texCoord : TEXCOORD0;
};

struct PSOut
{
    float4 color : SV_Target;
};

struct PSOutMRT
{
    float4 color0 : SV_Target0;
    float4 color1 : SV_Target1;
};

cbuffer cb0
{
    float g_RowScale;
    float g_ColScale;
};

PSIn VS_Quad(VSIn input)
{
    PSIn output;
     
    output.position = 0;
    output.texCoord = 0;
        
    return output;
}

[maxvertexcount(4)]
void GS_Quad( point PSIn inputPoint[1], inout TriangleStream<PSIn> outputQuad, uint primitive : SV_PrimitiveID )
{
    PSIn output;
    
    output.position.z = 0.5;
    output.position.w = 1.0;
    
    output.texCoord.z = primitive;
    
    output.position.x = -1.0;
    output.position.y = 1.0;
    output.texCoord.xy = float2( 0.0, 0.0 );
    outputQuad.Append( output );
    
    output.position.x = 1.0;
    output.position.y = 1.0;
    output.texCoord.xy = float2( 1.0, 0.0 );
    outputQuad.Append( output );
    
    output.position.x = -1.0;
    output.position.y = -1.0;
    output.texCoord.xy = float2( 0.0, 1.0 );
    outputQuad.Append( output );
        
    output.position.x = 1.0;
    output.position.y = -1.0;
    output.texCoord.xy = float2( 1.0, 1.0 );
    outputQuad.Append( output );
    
    outputQuad.RestartStrip();
}    

/////////////////////////////////////////////////////////////////////////////
// JPEG Decompression
// IDCT based on Independent JPEG Group code
/////////////////////////////////////////////////////////////////////////////

PSOutMRT PS_IDCT_Rows( PSIn input )
{
    PSOutMRT output;
    float d[8];
    
    // Read row elements
    d[0] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( -4, 0 ) );
    d[1] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( -3, 0 ) );
    d[2] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( -2, 0 ) );
    d[3] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( -1, 0 ) );
    d[4] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( 0, 0 ) );
    d[5] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( 1, 0 ) );
    d[6] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( 2, 0 ) );
    d[7] = 128.0 * TextureDCT.Sample( samplerPoint, input.texCoord, int2( 3, 0 ) );
    
    // Perform dequantization
    d[0] *= QuantTexture.Sample( samplerPoint, float2( 0.0625, input.texCoord.y * g_ColScale ) ) * 256.0f;
    d[1] *= QuantTexture.Sample( samplerPoint, float2( 0.1875, input.texCoord.y * g_ColScale ) ) * 256.0f;
    d[2] *= QuantTexture.Sample( samplerPoint, float2( 0.3125, input.texCoord.y * g_ColScale ) ) * 256.0f;
    d[3] *= QuantTexture.Sample( samplerPoint, float2( 0.4375, input.texCoord.y * g_ColScale ) ) * 256.0f;
    d[4] *= QuantTexture.Sample( samplerPoint, float2( 0.5625, input.texCoord.y * g_ColScale ) ) * 256.0f;
    d[5] *= QuantTexture.Sample( samplerPoint, float2( 0.6875, input.texCoord.y * g_ColScale ) ) * 256.0f;
    d[6] *= QuantTexture.Sample( samplerPoint, float2( 0.8125, input.texCoord.y * g_ColScale ) ) * 256.0f;
    d[7] *= QuantTexture.Sample( samplerPoint, float2( 0.9375, input.texCoord.y * g_ColScale ) ) * 256.0f;
    
    float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    float tmp10, tmp11, tmp12, tmp13;
    float z5, z10, z11, z12, z13;
    
    tmp0 = d[0];
    tmp1 = d[2];
    tmp2 = d[4];
    tmp3 = d[6];

    tmp10 = tmp0 + tmp2;
    tmp11 = tmp0 - tmp2;

    tmp13 = tmp1 + tmp3;
    tmp12 = (tmp1 - tmp3) * 1.414213562 - tmp13;

    tmp0 = tmp10 + tmp13;
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;
    
    tmp4 = d[1];
    tmp5 = d[3];
    tmp6 = d[5];
    tmp7 = d[7];

    z13 = tmp6 + tmp5;
    z10 = tmp6 - tmp5;
    z11 = tmp4 + tmp7;
    z12 = tmp4 - tmp7;

    tmp7 = z11 + z13;
    tmp11 = (z11 - z13) * 1.414213562;

    z5 = (z10 + z12) * 1.847759065;
    tmp10 = 1.082392200 * z12 - z5;
    tmp12 = -2.613125930 * z10 + z5;

    tmp6 = tmp12 - tmp7;
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    output.color0.x = tmp0 + tmp7;
    output.color1.w = tmp0 - tmp7;
    output.color0.y = tmp1 + tmp6;
    output.color1.z = tmp1 - tmp6;
    output.color0.z = tmp2 + tmp5;
    output.color1.y = tmp2 - tmp5;
    output.color1.x = tmp3 + tmp4;
    output.color0.w = tmp3 - tmp4;
                        
    return output;
}

PSOut PS_IDCT_Unpack_Rows( PSIn input )
{
    PSOut output;
    
    // Get eight values storded in 2 textures
    float4 values1 = RowTexture1.Sample( samplerPoint, input.texCoord );
    float4 values2 = RowTexture2.Sample( samplerPoint, input.texCoord );
        
    // Calculate a single non-zero index to define an element to be used
    int index = frac( input.texCoord.x * g_RowScale ) * 8.0;
        
    float4 indexMask1 = ( index == float4( 0, 1, 2, 3 ) );
    float4 indexMask2 = ( index == float4( 4, 5, 6, 7 ) );
    
    output.color = dot( values1, indexMask1 ) + dot( values2, indexMask2 );
    
    return output;
}

PSOutMRT PS_IDCT_Columns( PSIn input )
{
    PSOutMRT output;
    float d[8];
    
    // Read column elements
    d[0] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, -4 ) );
    d[1] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, -3 ) );
    d[2] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, -2 ) );
    d[3] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, -1 ) );
    d[4] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, 0 ) );
    d[5] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, 1 ) );
    d[6] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, 2 ) );
    d[7] = TargetTexture.Sample( samplerPoint, input.texCoord, int2( 0, 3 ) );
    
    float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    float tmp10, tmp11, tmp12, tmp13;
    float z5, z10, z11, z12, z13;
    
    tmp0 = d[0];
    tmp1 = d[2];
    tmp2 = d[4];
    tmp3 = d[6];

    tmp10 = tmp0 + tmp2;
    tmp11 = tmp0 - tmp2;

    tmp13 = tmp1 + tmp3;
    tmp12 = (tmp1 - tmp3) * 1.414213562 - tmp13;

    tmp0 = tmp10 + tmp13;
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;
    
    tmp4 = d[1];
    tmp5 = d[3];
    tmp6 = d[5];
    tmp7 = d[7];

    z13 = tmp6 + tmp5;
    z10 = tmp6 - tmp5;
    z11 = tmp4 + tmp7;
    z12 = tmp4 - tmp7;

    tmp7 = z11 + z13;
    tmp11 = (z11 - z13) * 1.414213562;

    z5 = (z10 + z12) * 1.847759065;
    tmp10 = 1.082392200 * z12 - z5;
    tmp12 = -2.613125930 * z10 + z5;

    tmp6 = tmp12 - tmp7;
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    output.color0.x = tmp0 + tmp7;
    output.color1.w = tmp0 - tmp7;
    output.color0.y = tmp1 + tmp6;
    output.color1.z = tmp1 - tmp6;
    output.color0.z = tmp2 + tmp5;
    output.color1.y = tmp2 - tmp5;
    output.color1.x = tmp3 + tmp4;
    output.color0.w = tmp3 - tmp4;
                        
    return output;
}

PSOut PS_IDCT_Unpack_Columns( PSIn input )
{
    PSOut output;
    
    // Get eight values storded in 2 textures
    float4 values1 = ColumnTexture1.Sample( samplerPoint, input.texCoord );
    float4 values2 = ColumnTexture2.Sample( samplerPoint, input.texCoord );
        
    // Calculate a single non-zero index to define an element to be used
    int index = frac( input.texCoord.y * g_ColScale ) * 8.0;
        
    float4 indexMask1 = ( index == float4( 0, 1, 2, 3 ) );
    float4 indexMask2 = ( index == float4( 4, 5, 6, 7 ) );
    
    output.color = clamp( ( dot( values1, indexMask1 ) + dot( values2, indexMask2 ) ) * 0.125 + 128.0, 0.0, 256.0 );
    
    return output;
}

PSOut PS_IDCT_RenderToBuffer( PSIn input )
{
    PSOut output;
    
    float Y = TextureY.Sample( samplerPoint, input.texCoord );
    float Cb = TextureCb.Sample( samplerPoint, input.texCoord );
    float Cr = TextureCr.Sample( samplerPoint, input.texCoord );
        
    // Convert YCbCr -> RGB
    output.color.x = Y + 1.402 * ( Cr - 128.0 );
    output.color.y = Y - 0.34414 * ( Cb - 128.0 ) - 0.71414 * ( Cr - 128.0 );
    output.color.z = Y + 1.772 * ( Cb - 128.0 );
    output.color.w = TextureHeight.Sample( samplerPoint, input.texCoord );
        
    output.color.xyzw *= ( 1.0 / 256.0 );
    
    return output;
}

//--------------------------------------------------------------------------------------
// Compiled shaders used in different techniques
//--------------------------------------------------------------------------------------

VertexShader VS_Quad_Compiled = CompileShader( vs_4_0, VS_Quad() );
GeometryShader GS_Quad_Compiled = CompileShader( gs_4_0, GS_Quad() );

BlendState NoBlending
{
    BlendEnable[0] = FALSE;
};

technique10 JPEG_Decompression
{
    pass p0
    {
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		
		SetVertexShader( VS_Quad_Compiled );
        SetGeometryShader( GS_Quad_Compiled );
        SetPixelShader( CompileShader( ps_4_0, PS_IDCT_Rows() ) );
    }
    pass p1
    {
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		
		SetVertexShader( VS_Quad_Compiled );
        SetGeometryShader( GS_Quad_Compiled );
        SetPixelShader( CompileShader( ps_4_0, PS_IDCT_Unpack_Rows() ) );
    }
    pass p2
    {
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		
		SetVertexShader( VS_Quad_Compiled );
        SetGeometryShader( GS_Quad_Compiled );
        SetPixelShader( CompileShader( ps_4_0, PS_IDCT_Columns() ) );
    }
    pass p3
    {
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		
		SetVertexShader( VS_Quad_Compiled );
        SetGeometryShader( GS_Quad_Compiled );
        SetPixelShader( CompileShader( ps_4_0, PS_IDCT_Unpack_Columns() ) );
    }
    pass p4
    {
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		
		SetVertexShader( VS_Quad_Compiled );
        SetGeometryShader( GS_Quad_Compiled );
        SetPixelShader( CompileShader( ps_4_0, PS_IDCT_RenderToBuffer() ) );
    }
}