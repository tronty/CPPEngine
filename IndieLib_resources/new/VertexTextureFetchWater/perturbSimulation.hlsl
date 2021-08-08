
#include "registers.hlsl"

sampler  textureSampler              : register(s0);

struct Interpolants
{
  float4 position      : POSITION;
  float4 diffuse       : COLOR0;
  float2 texcoord      : TEXCOORD0;
};

Interpolants vertexShader(float4 position : POSITION,
                          float4 diffuse  : COLOR0,
                          float2 texcoord : TEXCOORD0)
{
  Interpolants o;
  o = (Interpolants)0;

  // Copy the position to the output
  o.position = position;

  // Copy the color to the output
  o.diffuse = diffuse;

  // Output the texcoord
  o.texcoord = texcoord;

  return(o);
}

float4 pixelShader(Interpolants i) : COLOR
{
  return((tex2D(textureSampler, i.texcoord) - 0.5f) * psDeltaTime);
}
