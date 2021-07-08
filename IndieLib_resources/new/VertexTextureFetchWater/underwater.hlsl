
#include "registers.hlsl"

sampler textureSampler0                     : register(s0);


float4 pixelShader(float4 position : POSITION,
                   float2 texCoord : TEXCOORD0) : COLOR
{
  float2 perturbation;

  // Calculate a pertubation for the texture coordinates
  perturbation.x = sin(6.28 * frac(psTime / 3) + 20 * texCoord.x);
  perturbation.y = cos(6.28 * frac(psTime / 3) + 20 * texCoord.y);
  perturbation *= 0.005;

  // Perturb the texture coordinates. Fall off to zero on the borders to avoid
  // artifacts on edges
  perturbation.x *= saturate(0.8 - (2.0 * texCoord.x - 1.0) * (2.0 * texCoord.x - 1.0));
  perturbation.y *= saturate(0.8 - (2.0 * texCoord.y - 1.0) * (2.0 * texCoord.y - 1.0));

  // Returned the perturbed texture lookup
  return(tex2D(textureSampler0, texCoord + perturbation));
}
