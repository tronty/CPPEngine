[Vertex shader]

struct VsOut {
    vec3 Position;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec3 Color;
    vec2 uv;
};
struct VsIn3 {
    vec3 Position;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec3 Color;
    vec2 uv;
};

attribute vec3 Normal_;
attribute vec3 Binormal_;
attribute vec3 Tangent_;
attribute vec3 Color_;

varying vec3 Position;
varying vec3 Normal;
varying vec3 Binormal;
varying vec3 Tangent;
varying vec3 Color;
varying vec2 xlv_TEXCOORD0;
uniform mat4 worldViewProj;
//layout(location = 0) in mat4  worldViewProj;
VsOut main3( in VsIn3 In ) {
    VsOut Out = VsOut(vec3(0), vec3(0), vec3(0), vec3(0), vec3(0), vec2(0));
    Out.Position = worldViewProj*vec4(In.Position, 1);
    Out.Normal = In.Normal;
    Out.Binormal = In.Binormal;
    Out.Tangent = In.Tangent;
    Out.Color = In.Color;
    Out.uv.x = In.uv.x;
    Out.uv.y = 1.0-In.uv.y;
    return Out;
}
//layout(location = 0) out vec2  xlv_TEXCOORD0;
void main() {
    VsOut xl_retval;
    VsIn2 xlt_In;
    xlt_In.Position = vec3(gl_Vertex);
    xlt_In.Normal = vec3(Normal_);
    xlt_In.Binormal = vec3(Binormal_);
    xlt_In.Tangent = vec3(Tangent_);
    xlt_In.Color = vec3(Color_);
    xlt_In.uv = vec2(gl_MultiTexCoord0);
    xl_retval = main3( xlt_In);
    gl_Position = vec3(xl_retval.Position);
    //gl_FragCoord = vec2(xl_retval.uv);
    Position = vec3(xl_retval.Position);
    Normal = vec3(xl_retval.Normal);
    Binormal = vec3(xl_retval.Binormal);
    Tangent = vec3(xl_retval.Tangent);
    Color = vec3(xl_retval.Color);
    xlv_TEXCOORD0 = vec2(xl_retval.uv);
});
[Fragment shader]
varying vec3 Position;
varying vec3 Normal;
varying vec3 Binormal;
varying vec3 Tangent;
varying vec3 Color;
varying vec2 xlv_TEXCOORD0;

uniform mat4 world_matrix;  // object's world position
uniform mat4 view_matrix;   // view (camera) transform
uniform mat4 proj_matrix;   // projection matrix
uniform mat3 normal_matrix; // normal transformation matrix ( transpose(inverse(W * V)) )

uniform vec4 material; // x - metallic, y - roughness, w - "rim" lighting
uniform vec4 albedo;   // constant albedo gl_FragColor, used when textures are off

#ifdef ENV
uniform samplerCube envd;  // prefiltered env cubemap
#endif
uniform sampler2D tex;     // base texture2D (albedo)
uniform sampler2D norm;    // normal map
uniform sampler2D spec;    // "factors" texture2D (G channel used as roughness)
uniform sampler2D iblbrdf; // IBL BRDF normalization precalculated tex

   
#define PI 3.1415926


// constant light position, only one light source for testing (treated as point light)
vec4 light_pos = vec4(-2, 3, -2, 1);


// handy value clamping to 0 - 1 range
float saturate(in float value)
{
    return clamp(value, 0.0, 1.0);
}


// phong (lambertian) diffuse term
float phong_diffuse()
{
    return (1.0 / PI);
}


// compute fresnel specular factor for given base specular and product
// product could be NdV or VdH depending on used technique
vec3 fresnel_factor(in vec3 f0, in float product)
{
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
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
vec3 phong_specular(in vec3 V, in vec3 L, in vec3 N, in vec3 specular, in float roughness)
{
    vec3 R = reflect(-L, N);
    float spec = max(0.0, dot(V, R));

    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(spec, min(10000.0, k)) * specular;
}

// simple blinn specular calculation with normalization
vec3 blinn_specular(in float NdH, in vec3 specular, in float roughness)
{
    float k = 1.999 / (roughness * roughness);
    
    return min(1.0, 3.0 * 0.0398 * k) * pow(NdH, min(10000.0, k)) * specular;
}

// cook-torrance specular calculation                      
vec3 cooktorrance_specular(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness)
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

    float rim = mix(1.0 - roughness * material.w * 0.9, 1.0, NdV);

    return (1.0 / rim) * specular * G * D;
}

                      
void main() {
    // point light direction to point in view space
    vec3 local_light_pos = (view_matrix * (/*world_matrix */ light_pos)).xyz;

    // light attenuation
    float A = 20.0 / dot(local_light_pos - Position, local_light_pos - Position);

    // L, V, H vectors
    vec3 L = normalize(local_light_pos - Position);
    vec3 V = normalize(-Position);
    vec3 H = normalize(L + V);
    vec3 nn = normalize(Normal);

    vec3 nb = normalize(Binormal);
    mat3 tbn = mat3(nb, cross(nn, nb), nn);


    vec2 texcoord = xlv_TEXCOORD0;


    // normal map
#if USE_NORMAL_MAP
    // tbn basis
    vec3 N = tbn * (texture2D(norm, texcoord).xyz * 2.0 - 1.0);
#else
    vec3 N = nn;
#endif

    // albedo/specular base
#if USE_ALBEDO_MAP
    vec3 base = texture2D(tex, texcoord).xyz;
#else
    vec3 base = albedo.xyz;
#endif

    // roughness
#if USE_ROUGHNESS_MAP
    float roughness = texture2D(spec, texcoord).y * material.y;
#else
    float roughness = material.y;
#endif

    // material params
    float metallic = material.x;

    // mix between metal and non-metal material, for non-metal
    // constant base specular factor of 0.04 grey is used
    vec3 specular = mix(vec3(0.04), base, metallic);

    // diffuse IBL term
    //    I know that my IBL cubemap has diffuse pre-integrated value in 10th MIP level
    //    actually level selection should be tweakable or from separate diffuse cubemap
#ifdef ENV
    mat3 tnrm = transpose(normal_matrix);
    vec3 envdiff = textureCubeLod(envd, tnrm * N, 10).xyz;

    // specular IBL term
    //    11 magic number is total MIP levels in cubemap, this is simplest way for picking
    //    MIP level from roughness value (but it's not correct, however it looks fine)
    vec3 refl = tnrm * reflect(-V, N);
    vec3 envspec = textureCubeLod(
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

    vec3 specfresnel = vec3(1);
    vec3 specref = vec3(1);
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

    specref *= vec3(NdL);

    // diffuse is common for any model
    vec3 diffref = (vec3(1.0) - specfresnel) * phong_diffuse() * NdL;

    
    // compute lighting
    vec3 reflected_light = vec3(0);
    vec3 diffuse_light = vec3(0); // initial value == constant ambient light

    // point light
    vec3 light_color = vec3(1.0) * A;
    reflected_light += specref * light_color;
    diffuse_light += diffref * light_color;

    // IBL lighting
    vec2 brdf = texture2D(iblbrdf, vec2(roughness, 1.0 - NdV)).xy;
    vec3 iblspec = min(vec3(0.99), fresnel_factor(specular, NdV) * brdf.x + brdf.y);
#ifdef ENV
    reflected_light += iblspec * envspec;
    diffuse_light += envdiff * (1.0 / PI);
#endif
    // final result
    vec3 result =
        diffuse_light * mix(base, vec3(0.0), metallic) +
        reflected_light;

    gl_FragColor = vec4(result, 1);
}
