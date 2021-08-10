/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

Texture2D Texture;

#define MIP_LEVELS_MAX 7

SamplerState g_samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
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

struct PSInQuad
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

struct PSOut
{
    float4 color : SV_Target;
};

uint    g_StackDepth=1;
float   g_ScreenAspectRatio=4.0/3.0;

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
    
    //output.texCoord.z = primitive;
    
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

PSOut PSQuad(PSInQuad input)
{
    PSOut output;
    
    float width = 0.995 - input.texCoord.x;
    width = saturate( width * 50.0 );
    
    output.color.xyz = Texture.Sample( g_samLinear, input.texCoord ).xyz;
    output.color.w = 1.0;           
    
    return output;
}

BlendState NoBlending
{
    BlendEnable[0] = FALSE;
};

[techniques]
<!-- Textures>
	    <Texture sampler="Texture" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures -->
<Techniques>
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

