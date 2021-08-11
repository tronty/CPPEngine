
#include "registers.hlsl"

sampler  refractionSampler                          : register(s0);     // Refraction map
sampler  reflectionSampler                          : register(s1);     // Reflection map
sampler  refractionNearSampler                      : register(s2);     // Refraction map for objects penetrating water's surface
sampler  fadeSampler                                : register(s4);     // A fade texture to alleviate some border artifacts
sampler  simulationSampler                          : register(s257);

struct Interpolants
{
  float4 positionSS                                 : POSITION;         // Screen space position
  float2 reflectionTexCoord                         : TEXCOORD0;        // texture coordinates for reflection map
  float4 refractionTexCoords                        : TEXCOORD1;        // texture coordinates for refraction maps
  float3 normalWS                                   : TEXCOORD2;        // normal in world space
  float3 eyeVectorWS                                : TEXCOORD3;        // eye vector in world space
  float3 lightVectorWS                              : TEXCOORD4;        // light vector in world space
  float3 halfVectorWS                               : TEXCOORD5;        // half vector in world space
};





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//
//                              Vertex Shader
//
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Vertex shader Function declarations
float2 computeTextureCoord(float4 positionWS, float3 directionWS, float distance, float2 scale, float2 offset);


Interpolants vertexShader(float4 positionMS          : POSITION,   // Model space position
                          float4 color0              : COLOR0,
                          float4 simulationTexCoord  : TEXCOORD0,  // texture coord to look up simulated height and normal
                          float4 perturbation        : TEXCOORD1)  // Describes how much to reflect / refract
{
  Interpolants o = (Interpolants)0;
  float4       simulationSample;          // Sample for simulation that contains the height and normal
  float4       positionWS;        
  float3       normalModelSpace;
  float3       normalWS;
  float3       reflectionVectorWS;
  float3       refractionVectorWS;


  // Look up the simulation result for this vertex
  // simulationSample.x is the height    (simulationSample.y, simulationSample.z, simulationSample.w) is the normal 
  simulationSample = tex2Dlod(simulationSampler, simulationTexCoord);

  // Extract the height and normal out of the sample
  // Stick the height in the position where it belongs
  positionMS.y = simulationSample.x;
  positionMS.w = 1;  // This probably isn't necessary
  normalModelSpace = simulationSample.yzw;
  
  // Output the screen space position
  o.positionSS = mul(positionMS, vsWorldViewProjectionMatrix);

  // Compute the world space position
  positionWS = mul(positionMS, vsWorldMatrix);

  // Compute the world space normal
  normalWS = mul(normalModelSpace, vsWorldMatrix);

  // Flip the normal if we're under water
  normalWS.xyz *= vsNormalSign.x;

  // Output the world space normal
  o.normalWS = normalWS;

  // Output the eye vector in world space
  o.eyeVectorWS = normalize(vsEyePositionWorld - positionWS);

  // Output the light vector in world space
  o.lightVectorWS = normalize(vsLightPositionWorld - positionWS);

  // Output the half vector in world space
  // No need to normalize because it's normalized in the pixel shader
  o.halfVectorWS = o.eyeVectorWS + o.lightVectorWS;



  //////////////////////////////////////////////////////////////////////////////////
  //
  //                   Calculate reflection map coordinates
  //
  //////////////////////////////////////////////////////////////////////////////////

  // Compute the reflection vector in world space
  reflectionVectorWS = reflect(-o.eyeVectorWS, normalWS);

  // Compute the reflection map coordinates
  o.reflectionTexCoord.xy = computeTextureCoord(positionWS, reflectionVectorWS, perturbation.x, vsReflectionTexcoordScaleOffset.xy,
                                                vsReflectionTexcoordScaleOffset.zw);


  //////////////////////////////////////////////////////////////////////////////////
  //  
  //                   Calculate refraction map coordinates
  //
  //////////////////////////////////////////////////////////////////////////////////

  // Compute the refraction vector in world space
  refractionVectorWS = refract(-o.eyeVectorWS, normalWS, vsEtaRatio.x);
 
  // Calculate the refraction map coordinates for objects that are not penetrating the water
  o.refractionTexCoords.xy = computeTextureCoord(positionWS, refractionVectorWS, perturbation.y, vsRefractionTexcoordScaleOffset.xy,
                                                 vsRefractionTexcoordScaleOffset.zw);

  // Calculate the refraction map coordinates for objects that are penetrating the water
  o.refractionTexCoords.zw = computeTextureCoord(positionWS, refractionVectorWS, perturbation.w, vsRefractionTexcoordScaleOffset.xy,
                                                 vsRefractionTexcoordScaleOffset.zw);

  return(o);
}


// computeTextureCoord() takes a starting position, direction and distance in world space. 
// It computes a new position by moving the distance along the direction vector. This new
// world space position is projected into screen space. The screen space coordinates are
// massaged to work as texture coordinates.
float2 computeTextureCoord(float4 positionWS, float3 directionWS, float distance, float2 scale, float2 offset)
{
  float4 positionSS;

  // Compute the position after traveling a fixed distance
  positionWS.xyz = positionWS.xyz + directionWS * distance;
  positionWS.w = 1.0;

  // Compute the screen space position of the newly computed position
  positionSS = mul(positionWS, vsViewProjectionMatrix);

  // Do the perspective divide
  positionSS.xy /= positionSS.w;

  // Convert screen space position from [-1,-1]->[1,1] to [0,0]->[1,1]
  // This is done to match the coordinate space of the reflection/refraction map
  positionSS.xy = positionSS.xy * 0.5 + 0.5;

  // Account for the fact that we use a different field of view for the reflection/refraction maps.
  // This overdraw allows us to see stuff in the reflection/refraction maps that is not visible
  // from the normal viewpoint.
  positionSS.xy = positionSS.xy * scale + offset;

  // Flip the t texture coordinate upside down to be consistent with D3D
  positionSS.y = 1 - positionSS.y;

  // Return the screen space position as the result. This will be used as the texture coordinate
  // for the screenspace reflection/refraction maps.
  return(positionSS.xy);
}










////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//
//                              Pixel Shader
//
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


// Pixel shader Function declarations
half computeFresnel(half3 light, half3 normal, half R0);


half4 pixelShader(Interpolants i) : COLOR
{
  half4 refraction;
  half4 reflection;
  half4 refractionNear;
  half4 reflectionNear;
  half2 refractionTexCoord;
  half2 refractionNearTexCoord;
  half2 reflectionNearTexCoord;
  half  refractionVisibility;
  half4 fade;
  half4 specular;
  half  depth;
  half  fresnel;


  // Normalize direction vectors
  i.lightVectorWS = normalize(i.lightVectorWS);
  i.normalWS = normalize(i.normalWS);
  i.halfVectorWS = normalize(i.halfVectorWS);

  // Compute the specular term
  specular.x = pow(max(dot(i.halfVectorWS, i.normalWS), 0), 3);

  // Put a cliff in the specular function
  if(specular.x < 0.5)
  {
    specular.x = 2.0 * specular.x * specular.x;
  }
  specular.xyz = specular.xxx * half3(0.2, 0.2, 0.2);
  specular.w = 0;

  // Do the texture lookup for the reflection
  reflection = tex2D(reflectionSampler, i.reflectionTexCoord.xy);

  // Modulate the reflection texture by a texture to fade out at the edges.
  // This avoids an objectionable artifact when the reflection map
  // doesn't cover enough area.
  fade = tex2D(fadeSampler, i.reflectionTexCoord.xy);
  reflection *= fade;

  // Do the texture lookup for the refraction
  refractionTexCoord = i.refractionTexCoords.xy;
  refractionNearTexCoord = i.refractionTexCoords.zw;

  refraction = tex2D(refractionSampler, refractionTexCoord);
  refractionNear = tex2D(refractionNearSampler, refractionNearTexCoord);

  // Do a very simple compositing of objects in the refraction textures
  if(dot(refractionNear.xyz, refractionNear.xyz) > 0.1)
  {
    refraction.xyz = refractionNear.xyz;
  }

  // Compute the fresnel to blend the refraction and reflection terms together
  fresnel = computeFresnel(i.eyeVectorWS, i.normalWS, psFresnelR0.x);

  // Handle total internal reflection
  refractionVisibility = saturate(dot(i.eyeVectorWS, i.normalWS));
  refractionVisibility = saturate((refractionVisibility - psTotalInternalReflectionSlopeBias.y) * psTotalInternalReflectionSlopeBias.x +
                                  psTotalInternalReflectionSlopeBias.z);
  refraction *= refractionVisibility;

  // Give some blue tint to the refraction
  refraction = lerp(refraction, half4(0, 0, 1, 0), .1);

#ifdef RETURN_REFRACTION
  return(refraction);
#endif
#ifdef RETURN_REFLECTION
  return(reflection);
#endif
#ifdef RETURN_FRESNEL
  return(half4(fresnel, fresnel, fresnel, 1.0));
#endif
#ifdef RETURN_NORMALS
  return(half4(i.normalWS.xyz, 1.0));
#endif

  // Combine the refraction, reflection and specular terms
  return(lerp(refraction, reflection, fresnel) + specular);
}


half computeFresnel(half3 eye, half3 normal, half R0)
{
  // R0 = pow(1.0 - refractionIndexRatio, 2.0) / pow(1.0 + refractionIndexRatio, 2.0);

  half eyeDotNormal;
 
  eyeDotNormal = dot(eye, normal);

  // Make sure eyeDotNormal is positive
  eyeDotNormal = max(eyeDotNormal, -eyeDotNormal);

  return(R0 + (1.0 - R0) * pow(1.0 - eyeDotNormal, 4.5));
}

