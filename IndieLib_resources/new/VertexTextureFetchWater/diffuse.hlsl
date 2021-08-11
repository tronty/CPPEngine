
#include "registers.hlsl"

struct Interpolants
{
  float4 position      : POSITION;
  float4 diffuse       : COLOR0;
};


Interpolants vertexShader(float4 position : POSITION,
                          float3 normal : NORMAL)
{
  Interpolants o;
  float3       positionWorld;
  float3       normalWorld;
  float3       eyeWorld;

  o = (Interpolants)0;

  // Output the position in screen space
  o.position = mul(position, vsWorldViewProjectionMatrix);

  // Compute the position in world space
  positionWorld = mul(position, vsWorldMatrix);

  // Transform the normal into world space
  normalWorld = normalize(mul(normal, vsWorldInverseTransposeMatrix));

  // Compute the eye vector
  eyeWorld = normalize(vsEyePositionWorld - positionWorld);

  // Compute the diffuse color
  o.diffuse = saturate(dot(normalWorld, eyeWorld)) * vsMaterialDiffuse * vsLightDiffuse + vsMaterialAmbient * vsLightAmbient;

  return(o);
}

float4 pixelShader(Interpolants i) : COLOR
{
  return(i.diffuse);
}
