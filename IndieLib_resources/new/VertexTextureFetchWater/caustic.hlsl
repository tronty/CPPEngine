
#include "registers.hlsl"


sampler  waterSampler                        : register(s0);
sampler  causticSampler                      : register(s1);

struct Interpolants
{
  float4 position           : POSITION;
  float2 waterTexCoord      : TEXCOORD0;
};


Interpolants vertexShader(float4 position : POSITION)
{
  Interpolants o;
  float4       positionWorldSpace;
  float        t;

  o = (Interpolants)0;

  // Output the position in screen space
  o.position = mul(position, vsWorldViewProjectionMatrix);

  // Compute the world space position of the vertex
  positionWorldSpace = mul(position, vsWorldMatrix);
  positionWorldSpace.xyz /= positionWorldSpace.w;

  // Compute the water texture coordinate
  o.waterTexCoord = float2(positionWorldSpace.x / 50.0, positionWorldSpace.z / 50.0);
  
  return(o);
}

half4 pixelShader(Interpolants i) : COLOR
{
  half3 normal;
  half4 result;

  // Get the water normal above the fragment
  normal = tex2D(waterSampler, i.waterTexCoord).yzw;
  
  // Compute the caustic. Basically, we just assumt that if the surface of the water
  // is vertical, then we're seeing a lot of the sun.
  result = 1.5 * pow(saturate(dot(normal, half3(0.0, 1.0, 0.0))), 10.0) * half4(0.0, 0.0, 1.0, 0.0);
  result.w = 0.12;

  return(result);
}
