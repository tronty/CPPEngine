/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 Color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
struct VsOut {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 Color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
[Vertex shader]
VsOut mainVS(VsIn IN){
	VsOut Out = (VsOut)0;
	Out = IN;
	float3 objPos = IN.Position;
	Out.Position = mul( float4( objPos, 1.0), worldViewProj);
	//Out.Position = mul( worldViewProj, float4( objPos, 1.0));
	return Out;
}

[Fragment shader]

#define COOK_BLINN 1
#undef COOK_BECKMANN
#undef COOK_GGX
#define PHONG 1
#undef BLINN
#undef COOK
#undef ENV

float4 material = float4(0,0,0,0); // x - metallic, y - roughness, w - "rim" lighting
float4 albedo = float4(0,0,0,0);   // constant albedo gl_FragColor, used when textures are off

#ifdef ENV
float3x3 normal_matrix; // normal transformation matrix ( transpose(inverse(W * V)) )
samplerCube envd;  // prefiltered env cubemap
#endif
sampler2D tex;     // base tex2D (albedo)
sampler2D norm;    // normal map
sampler2D spec;    // "factors" tex2D (G channel used as roughness)
sampler2D iblbrdf; // IBL BRDF normalization precalculated tex
   
#define PI 3.1415926


// constant light position, only one light source for testing (treated as point light)
float4 light_pos = float4(-2, 3, -2, 1);

#if 0
// handy value clamping to 0 - 1 range
float saturate(in float value)
{
    return clamp(value, 0.0, 1.0);
}
#endif


// phong (lambertian) diffuse term
float phong_diffuse()
{
    return (1.0 / PI);
}


// compute fresnel specular factor for given base specular and product
// product could be NdV or VdH depending on used technique
float3 fresnel_factor(in float3 f0, in float product)
{
    return lerp(f0, float3(1.0, 1.0, 1.0), pow(1.01 - product, 5.0));
}


// following functions are copies of UE4
// for computing cook-torrance specular lighting terms

float D_blinn(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
}

float D_beckmann(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float NdH2 = NdH * NdH;
    return exp((NdH2 - 1.0) / (m2 * NdH2)) / (PI * m2 * NdH2 * NdH2);
}

float D_GGX(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (PI * d * d);
}

float G_schlick(in float roughness, in float NdV, in float NdL)
{
    float k = roughness * roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
}


// simple phong specular calculation with normalization
float3 phong_specular(in float3 V, in float3 L, in float3 N, in float3 specular, in float roughness)
{
    float3 R = reflect(-L, N);
    float spec = max(0.0, dot(V, R));

    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(spec, min(10000.0, k)) * specular;
}

// simple blinn specular calculation with normalization
float3 blinn_specular(in float NdH, in float3 specular, in float roughness)
{
    float k = 1.999 / (roughness * roughness);
    
    return min(1.0, 3.0 * 0.0398 * k) * pow(NdH, min(10000.0, k)) * specular;
}

// cook-torrance specular calculation                      
float3 cooktorrance_specular(in float NdL, in float NdV, in float NdH, in float3 specular, in float roughness)
{
	float D = 1.0;
#ifdef COOK_BLINN
    D = D_blinn(roughness, NdH);
#endif

#ifdef COOK_BECKMANN
    D = D_beckmann(roughness, NdH);
#endif

#ifdef COOK_GGX
    D = D_GGX(roughness, NdH);
#endif

    float G = G_schlick(roughness, NdV, NdL);

    float rim = lerp(1.0 - roughness * material.w * 0.9, 1.0, NdV);

    return (1.0 / rim) * specular * G * D;
}
                   
float4 main(VsOut In) : COLOR0 {
    // point light direction to point in view space
    float3 local_light_pos = mul(worldViewProj, float4(light_pos,1)).xyz;

    // light attenuation
    float A = 20.0 / dot(local_light_pos - In.Position, local_light_pos - In.Position);

    // L, V, H vectors
    float3 L = normalize(local_light_pos - In.Position);
    float3 V = normalize(-In.Position);
    float3 H = normalize(L + V);
    float3 nn = normalize(In.Normal);

    float3 nb = normalize(In.Binormal);
    float3x3 tbn = float3x3(nb, cross(nn, nb), nn);


    float2 texcoord = In.uv;


    // normal map
#if USE_NORMAL_MAP
    // tbn basis
    float3 N = tbn * (tex2D(norm, texcoord).xyz * 2.0 - 1.0);
#else
    float3 N = nn;
#endif

    // albedo/specular base
#if USE_ALBEDO_MAP
    float3 base = tex2D(tex, texcoord).xyz;
#else
    float3 base = albedo.xyz;
#endif

    // roughness
#if USE_ROUGHNESS_MAP
    float roughness = tex2D(spec, texcoord).y * material.y;
#else
    float roughness = material.y;
#endif

    // material params
    float metallic = material.x;

    // lerp between metal and non-metal material, for non-metal
    // constant base specular factor of 0.04 grey is used
    float3 specular = lerp(float3(0.04, 0.04, 0.04), base, metallic);

    // diffuse IBL term
    //    I know that my IBL cubemap has diffuse pre-integrated value in 10th MIP level
    //    actually level selection should be tweakable or from separate diffuse cubemap
#ifdef ENV
    float3x3 tnrm = transpose(normal_matrix);
    float3 envdiff = textureCubeLod(envd, tnrm * N, 10).xyz;

    // specular IBL term
    //    11 magic number is total MIP levels in cubemap, this is simplest way for picking
    //    MIP level from roughness value (but it's not correct, however it looks fine)
    float3 refl = tnrm * reflect(-V, N);
    float3 envspec = textureCubeLod(
        envd, refl, max(roughness * 11.0, textureQueryLod(envd, refl).y)
    ).xyz;
#endif
    // compute material reflectance

    float NdL = max(0.0, dot(N, L));
    float NdV = max(0.001, dot(N, V));
    float NdH = max(0.001, dot(N, H));
    float HdV = max(0.001, dot(H, V));
    float LdV = max(0.001, dot(L, V));

    // fresnel term is common for any, except phong
    // so it will be calcuated inside ifdefs

    float3 specfresnel = float3(1, 1, 1);
    float3 specref = float3(1, 1, 1);
#ifdef PHONG
    // specular reflectance with PHONG
    specfresnel = fresnel_factor(specular, NdV);
    specref = phong_specular(V, L, N, specfresnel, roughness);
#endif

#ifdef BLINN
    // specular reflectance with BLINN
    specfresnel = fresnel_factor(specular, HdV);
    specref = blinn_specular(NdH, specfresnel, roughness);
#endif

#ifdef COOK
    // specular reflectance with COOK-TORRANCE
    specfresnel = fresnel_factor(specular, HdV);
    specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);
#endif

    specref *= float3(NdL, NdL, NdL);

    // diffuse is common for any model
    float3 diffref = (float3(1.0, 1.0, 1.0) - specfresnel) * phong_diffuse() * NdL;

    
    // compute lighting
    float3 reflected_light = float3(0,0,0);
    float3 diffuse_light = float3(0,0,0); // initial value == constant ambient light

    // point light
    float3 light_color = float3(1.0, 1.0, 1.0) * A;
    reflected_light += specref * light_color;
    diffuse_light += diffref * light_color;

    // IBL lighting
    float2 brdf = tex2D(iblbrdf, float2(roughness, 1.0 - NdV)).xy;
    float3 iblspec = min(float3(0.99, 0.99, 0.99), fresnel_factor(specular, NdV) * brdf.x + brdf.y);
#ifdef ENV
    reflected_light += iblspec * envspec;
    diffuse_light += envdiff * (1.0 / PI);
#endif
    // final result
    float3 result =
        diffuse_light * lerp(base, float3(0,0,0), metallic) +
        reflected_light;

    return float4(result, 1);
}

