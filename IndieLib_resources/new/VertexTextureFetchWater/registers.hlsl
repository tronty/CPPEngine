#pragma ONCE

//
//  Vertex Shader Constant Variables
//
float4x4   vsWorldMatrix                            : register(c0);
float4x4   vsWorldViewMatrix                        : register(c4);
float4x4   vsWorldViewProjectionMatrix              : register(c8);
float4x4   vsProjectionMatrix                       : register(c12);
float4x4   vsViewProjectionMatrix                   : register(c16);
float4x4   vsWorldInverseTransposeMatrix            : register(c20);
float3     vsLightPositionWorld                     : register(c32);
float3     vsEyePositionWorld                       : register(c33);
float4     vsMaterialDiffuse                        : register(c34);
float4     vsMaterialAmbient                        : register(c35);
float4     vsLightDiffuse                           : register(c36);
float4     vsLightAmbient                           : register(c37);
float4     vsEtaRatio                               : register(c51);
float4     vsRefractionTexcoordScaleOffset          : register(c52);
float4     vsReflectionTexcoordScaleOffset          : register(c53);
float4     vsNormalSign                             : register(c54);
float4     vsScaleBiasConstants                     : register(c94);
float4     vsMathConstants                          : register(c95);




//
//  Pixel Shader Constant Variables
//
float2     psSimulationTexCoordDelta_x0y1           : register(c10);
float2     psSimulationTexCoordDelta_x2y1           : register(c11);
float2     psSimulationTexCoordDelta_x1y0           : register(c12);
float2     psSimulationTexCoordDelta_x1y2           : register(c13);
float2     psSimulationPositionWeighting            : register(c14);
float      psSimulationWaveSpeedSquared             : register(c15);
float      psSimulationOneHalfTimesDeltaTimeSquared : register(c16);
float3     psSimulationGridSize                     : register(c17);
half3      psTotalInternalReflectionSlopeBias       : register(c18);
float      psTime                                   : register(c20);
float      psDeltaTime                              : register(c21);
float      psFresnelR0                              : register(c22);
float3     psEyePositionWorld                       : register(c23);
float      psUnderWater                             : register(c24);




