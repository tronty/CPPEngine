
#include "registers.hlsl"

sampler previousHeightSampler               : register(s0);
sampler heightSampler                       : register(s1);
sampler dampeningSampler                    : register(s2);


struct Interpolants
{
  float4 position : POSITION;
  float2 texCoord : TEXCOORD0;
  float4 diffuse  : COLOR0;
};


Interpolants vertexShader(float4 position : POSITION,
                          float4 diffuse  : COLOR0,
                          float2 texCoord : TEXCOORD0)
{
  Interpolants o;

  o.position = position;
  o.diffuse = diffuse;
  o.texCoord = texCoord;

  return(o);
}



half4 pixelShader(Interpolants i) : COLOR
{
  half  acceleration;
  half  height_x1y1;
  half  height_x0y1, height_x2y1;
  half  height_x1y0, height_x1y2;
  half  previousHeight;
  half  newHeight;
  half  dampening;
  half3 normal;
  half3 dydx, dydz;
  half4 result;

  // Look up all the neighbor heights
  height_x1y1 = tex2D(heightSampler, i.texCoord);
  height_x0y1 = tex2D(heightSampler, i.texCoord + psSimulationTexCoordDelta_x0y1);
  height_x2y1 = tex2D(heightSampler, i.texCoord + psSimulationTexCoordDelta_x2y1);
  height_x1y0 = tex2D(heightSampler, i.texCoord + psSimulationTexCoordDelta_x1y0);
  height_x1y2 = tex2D(heightSampler, i.texCoord + psSimulationTexCoordDelta_x1y2);
  previousHeight = tex2D(previousHeightSampler, i.texCoord);
  dampening = tex2D(dampeningSampler, i.texCoord);


  // Compute the acceleration of the point based upon its neighbors
  acceleration = dampening * psSimulationWaveSpeedSquared * (height_x0y1 + height_x2y1 + height_x1y0 + height_x1y2 - 4.0 * height_x1y1);

  // Do Verlet integration
  newHeight = psSimulationPositionWeighting.x * height_x1y1 - 
              psSimulationPositionWeighting.y * previousHeight + 
              psSimulationOneHalfTimesDeltaTimeSquared * acceleration;
 
  // Calculate the normal
  dydx = half3(psSimulationGridSize.x, height_x2y1 - height_x1y1, 0.0);
  dydz = half3(0.0, height_x1y2 - height_x1y1, -psSimulationGridSize.z);
  normal = cross(dydx, dydz);
  normal = normalize(normal);


  // Combine the height and normal
  result = half4(newHeight, normal);

  return(result);
}
