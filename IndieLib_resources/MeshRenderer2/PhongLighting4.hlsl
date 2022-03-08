#define MAX_LIGHTS 8
#define NUM_LIGHTS 3
float3 LightSource[MAX_LIGHTS];
float4 FrontLightModelProductsceneColor;
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Norm : TEXCOORD0;
	float3 eyeVec : TEXCOORD1;
	float3 lightDir[MAX_LIGHTS] : TEXCOORD2;
};
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection);
	Out.Norm = (mul(float4(IN.Normal,1), WorldViewProjection)).xyz;
	Out.eyeVec = -Out.Pos.xyz;
  	int i;
  	for (i=0; i<NUM_LIGHTS; ++i)
    		lightDir[i] = float3(LightSource[i].position.xyz - Out.Pos.xyz);
	return Out;
}
[Fragment shader]
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
	float3 View : TEXCOORD2;
	float2 uv : TEXCOORD3;
};
float4 main(VS_OUTPUT IN) : COLOR
{
  float4 final_color = FrontLightModelProductsceneColor;
  float3 N = normalize(IN.Norm);
  int i;
  for (i=0; i<NUM_LIGHTS; ++i)
  {  
    float3 L = normalize(lightDir[i]);
    float lambertTerm = dot(N,L);
    if (lambertTerm > 0.0)
    {
      final_color += 
        LightSource[i].diffuse * 
        FrontMaterial.diffuse * 
        lambertTerm;	
      float3 E = normalize(eyeVec);
      float3 R = reflect(-L, N);
      float specular = pow(max(dot(R, E), 0.0), 
                           FrontMaterial.shininess);
      final_color += 
        LightSource[i].specular * 
        FrontMaterial.specular * 
        specular;	
    }
  }
  return final_color;	
}













#version 450 core

layout (location = 0) in float3        in_position;
layout (location = 1) in float2        in_texcoord;
layout (location = 2) in float3        in_normal;
layout (location = 3) in int        in_materialindex;

layout (location = 4) in mat4 in_model;
//layout (location = 5) in use ...
//layout (location = 6) in use ...
//layout (location = 7) in use ...
layout (location = 8) in int in_objectID;

out VS_FS_INTERFACE {
    float3 position;
    float2 texcoord;
    float3 normal;
    flat int materialindex;
    flat int objectID;
} vs_out;

mat4 View = mat4(1);
mat4 Projection = mat4(1);


void main(void)
{
    mat4 MVP = Projection * View * in_model;
    gl_Position = MVP * float4(in_position, 1);

    vs_out.position = (in_model * float4(in_position, 1)).xyz;
    vs_out.texcoord = in_texcoord;
    vs_out.normal = (in_model * float4(in_normal, 0)).xyz;
    vs_out.materialindex = in_materialindex;
    vs_out.objectID = in_objectID;
}


Fragment Shader:  "shader.fs"
#version 450 core

#define MAX_MATERIALS                        1000
#define MAX_POINTLIGHTS                    10
#define MAX_DIRECTIONALLIGHTS        10
#define MAX_SPOTLIGHTS                    10


in VS_FS_INTERFACE {
    float3 position;
    float2 texcoord;
    float3 normal;
    flat int materialindex;
    flat int objectID;
} vs_out;


// here the definition has vec4s instead of float3
// thats because some drivers get the std140 layout for vec3s wrong
struct Material {
    float4 Ka, Kd, Ks;
    float Ns;
    float d;
};

// has constant intensity
struct DirectionalLight {
    float4 Direction;
    float4 Intensity;
};

// has distance-dependent intensity
struct PointLight {
    float4 Position;
    float4 Intensity;
    float AttenuationLinear;
};

// has angle-dependent intensity
struct SpotLight {
    float4 Position;
    float4 Direction;
    float4 Intensity;
    int Exponent;
};


layout (std140, binding = 1) MaterialBlock
{
    Material Materials[MAX_MATERIALS];
};

layout (std140, binding = 2) DirectionalLightBlock
{
    DirectionalLight DirectionalLights[MAX_DIRECTIONALLIGHTS];
};

layout (std140, binding = 3) PointLightBlock
{
    PointLight PointLights[MAX_POINTLIGHTS];
};

layout (std140, binding = 4) SpotLightBlock
{
    SpotLight SpotLights[MAX_SPOTLIGHTS];
};

float3 CameraPosition = float3(0, 0, 0);
float3 Ambient = float3(0, 0, 0);
int DirectionalLightCount = 0;
int PointLightCount = 0;
int SpotLightCount = 0;


layout (location = 0) out    float4       Fragment0;
layout (location = 1) out    int            Fragment1;


void ProcessDirectionalLights(    inout float3 Diffuse, inout float3 Specular, float3 SurfaceNormal, float3 SurfaceToCamera);
void ProcessPointLights(                inout float3 Diffuse, inout float3 Specular, float3 SurfaceNormal, float3 SurfaceToCamera);
void ProcessSpotLights(                inout float3 Diffuse, inout float3 Specular, float3 SurfaceNormal, float3 SurfaceToCamera);


void main(void)
{
    // total intensities
    float3 TotalDiffuse = float3(0, 0, 0);
    float3 TotalSpecular = float3(0, 0, 0);

    float3 SurfaceNormal = normalize(vs_out.normal);
    float3 SurfaceToCamera = normalize(CameraPosition - vs_out.position);

    ProcessDirectionalLights(    TotalDiffuse, TotalSpecular, SurfaceNormal, SurfaceToCamera);
    ProcessPointLights(            TotalDiffuse, TotalSpecular, SurfaceNormal, SurfaceToCamera);
    ProcessSpotLights(                TotalDiffuse, TotalSpecular, SurfaceNormal, SurfaceToCamera);

    float3 Ka = Materials[vs_out.materialindex].Ka.rgb;
    float3 Kd = Materials[vs_out.materialindex].Kd.rgb;
    float3 Ks = Materials[vs_out.materialindex].Ks.rgb;
    float3 Intensity = Ambient * Ka + TotalDiffuse * Kd + TotalSpecular * Ks;
   
    float d = Materials[vs_out.materialindex].d;
    Fragment0 = float4(Intensity, d);
    Fragment1 = vs_out.objectID;
}


void ProcessDirectionalLights(inout float3 Diffuse, inout float3 Specular, float3 SurfaceNormal, float3 SurfaceToCamera)
{
    float Ns = Materials[vs_out.materialindex].Ns;

    for (int i = 0; i < DirectionalLightCount; i++)
    {
        // for specular:
        float3 ReflectedLight = normalize(reflect(DirectionalLights[i].Direction.xyz, SurfaceNormal));

        Diffuse += DirectionalLights[i].Intensity.rgb * max(0, dot(SurfaceNormal, -normalize(DirectionalLights[i].Direction.xyz)));
        Specular += DirectionalLights[i].Intensity.rgb * max(0, pow(dot(ReflectedLight, SurfaceToCamera), max(1, Ns)));
    }
}


void ProcessPointLights(inout float3 Diffuse, inout float3 Specular, float3 SurfaceNormal, float3 SurfaceToCamera)
{
    float Ns = Materials[vs_out.materialindex].Ns;

    for (int i = 0; i < PointLightCount; i++)
    {
        // for diffuse:
        float3 SurfaceToLight = PointLights[i].Position.xyz - vs_out.position;
        float DistanceLightSurface = length(SurfaceToLight);
        SurfaceToLight /= DistanceLightSurface;
        float Attenuation = 1.0f + PointLights[i].AttenuationLinear * DistanceLightSurface;

        // for specular:
        float3 ReflectedLight = normalize(reflect(-SurfaceToLight, SurfaceNormal));

        Diffuse += PointLights[i].Intensity.rgb * max(0, dot(SurfaceNormal, SurfaceToLight)) / Attenuation;
        Specular += PointLights[i].Intensity.rgb * max(0, pow(dot(ReflectedLight, SurfaceToCamera), max(1, Ns))) / Attenuation;
    }
}


void ProcessSpotLights(inout float3 Diffuse, inout float3 Specular, float3 SurfaceNormal, float3 SurfaceToCamera)
{
    float Ns = Materials[vs_out.materialindex].Ns;

    for (int i = 0; i < SpotLightCount; i++)
    {
        // for diffuse:
        float3 SurfaceToLight = normalize(SpotLights[i].Position.xyz - vs_out.position);
        float CosAngle = dot(-SurfaceToLight, normalize(SpotLights[i].Direction.xyz));
        float Multiplier = pow(max(0, CosAngle), max(1, SpotLights[i].Exponent));

        // for specular:
        float3 ReflectedLight = normalize(reflect(-SurfaceToLight, SurfaceNormal));

        Diffuse += SpotLights[i].Intensity.rgb * max(0, dot(SurfaceNormal, SurfaceToLight)) * Multiplier;
        Specular += SpotLights[i].Intensity.rgb * max(0, pow(dot(ReflectedLight, SurfaceToCamera), max(1, Ns))) * Multiplier;
    }
}

