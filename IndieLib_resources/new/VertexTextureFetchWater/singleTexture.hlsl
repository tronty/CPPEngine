
#include "registers.hlsl"

sampler  textureSampler              : register(s0);

struct Interpolants
{
  float4 position      : POSITION;
  float2 texcoord      : TEXCOORD0;
};


Interpolants vertexShader(float4 position : POSITION,
                          float2 texcoord : TEXCOORD0)
{
  Interpolants o;

  o = (Interpolants)0;

  // Output the position in screen space
  o.position = mul(position, vsWorldViewProjectionMatrix);

  // Output the texcoord
  o.texcoord = texcoord;

  return(o);
}

float4 pixelShader(Interpolants i) : COLOR
{
  return(tex2D(textureSampler, i.texcoord));
}
