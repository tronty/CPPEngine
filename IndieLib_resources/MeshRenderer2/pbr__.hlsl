// https://github.com/SaschaWillems/Vulkan.git
// pbr.vert 
// pbr.frag
float3 camPos=float3(10.0f, 13.0f, 1.8f);
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float4x4 World;
float3 objPos=float3(0,0,0);
float4 vecLightDir = float4( -camPos.x, -camPos.y, -camPos.z, 1.0f);
float4 vecEye = float4( camPos.x, camPos.y, camPos.z, 1.0f);
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
	float3 View : TEXCOORD2;
	float2 uv : TEXCOORD3;
};
VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(WorldViewProjection, float4(IN.Pos,1))+float4(objPos,1);
#if 1
#if 0
    	Out.Light = float3(cosf(iTime*10.0),0,sinf(iTime*10.0))*2.;
    	Out.Light.y = 1.5;
#else
	Out.Light = vecLightDir.xyz;
#endif
	float3 PosWorld = normalize(mul(World, float4(IN.Pos,1))).xyz;
	Out.View = vecEye.xyz - PosWorld;
	Out.Norm = normalize(mul(World, float4(IN.Normal,1))).xyz;
	Out.uv=IN.uv;
#endif
	return Out;
}

[Fragment shader]
struct VS_OUTPUT
{
	float4 inWorldPos : POSITION;
	float3 Light : TEXCOORD0;
	float3 inNormal : TEXCOORD1;
	float3 View : TEXCOORD2;
	float2 uv : TEXCOORD3;
};
static float4 lights[4]=	{	
		                float4(-15.0, -15.0*0.5f, -15.0, 1.0f),
		                float4(-15.0, -15.0*0.5f,  15.0, 1.0f),
		                float4( 15.0, -15.0*0.5f,  15.0, 1.0f),
		                float4( 15.0, -15.0*0.5f, -15.0, 1.0f)
			};
float3 materialcolor=float3(1.0, 0.765557, 0.336057);
float roughness=0.1;
float metallic=1.0;

const float PI = 3.14159265359;
#define ROUGHNESS_PATTERN 1

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float aroughness)
{
	float alpha = aroughness * aroughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom); 
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float aroughness)
{
	float r = (aroughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float cosTheta, float ametallic)
{
	float3 F0 = mix(float3(0.04), materialcolor, ametallic); // * material.specular
	float3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
	return F;    
}

// Specular BRDF composition --------------------------------------------

float3 BRDF(float3 L, float3 V, float3 N, float ametallic, float aroughness)
{
	// Precalculate vectors and dot products	
	float3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	float3 lightColor = float3(1.0);

	float3 color = float3(0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, aroughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, aroughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, aroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		float3 F = F_Schlick(dotNV, ametallic);

		float3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

float4 main(VS_OUTPUT IN) : COLOR
{		  
	float3 N = normalize(IN.inNormal);
	float3 V = normalize(camPos - IN.inWorldPos);

	float roughness_ = roughness;

	// Add striped pattern to roughness based on vertex position
#ifdef ROUGHNESS_PATTERN
	roughness_ = max(roughness_, step(fract(IN.inWorldPos.y * 2.02), 0.5));
#endif

	// Specular contribution
	float3 Lo = float3(0.0);
	for (int i = 0; i < 4; i++) {
		float3 L = normalize(lights[i].xyz - IN.inWorldPos);
		Lo += BRDF(L, V, N, metallic, roughness_);
	};

	// Combine with ambient
	float3 color = materialcolor * 0.02;
	color += Lo;

	//color=GammaCorrect3(color);

	return float4(color, 1.0);
}

