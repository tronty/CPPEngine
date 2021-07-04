#include "inoise.fxh"

float4x4 World;
float4x4 View;
float4x4 Projection;

struct VertexShaderInput
{
    float4 Position : POSITION0;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // calc the height displacement using 12 octaves of fBm
    float heightValue = fBm(input.Position.xyz, 12);
    
    // add the height displacement to the vertex position
    input.Position.xyz = input.Position.xyz + heightValue;
    
    // multiply by the WVP matrices
    float4 worldPosition = mul(input.Position, World);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
    // simply output a white pixel
    return float4(1, 1, 1, 1);
}

technique Displacement
{
    pass Pass1
    {
        VertexShader = compile vs_3_0 VertexShaderFunction();
        PixelShader = compile ps_3_0 PixelShaderFunction();
    }
}
