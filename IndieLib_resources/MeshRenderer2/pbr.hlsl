// https://github.com/SaschaWillems/Vulkan.git
// pbr.vert 
// pbr.frag
//define ROUGHNESS_PATTERN 1
#if 0
#define ROW_MAJOR
#define MVPSEMANTIC
#endif

ROW_MAJOR float4x4 projection MVPSEMANTIC;
ROW_MAJOR float4x4 model MVPSEMANTIC;
ROW_MAJOR float4x4 view MVPSEMANTIC;
float3 camPos=float3(10.0, 13.0, 1.8);

struct VSOutput
{
	float4 Pos : POSITION;
	float3 WorldPos : TEXCOORD0;
	float3 Normal : NORMAL;
};

[Vertex shader]
struct VSInput
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

float3 objPos=float3(0.0, 0.0, 0.0);

VSOutput mainVS(VSInput input)
{
	VSOutput output = (VSOutput)0;
	float3 locPos = mul(model, float4(input.Pos, 1.0)).xyz;
	output.WorldPos = locPos + objPos;
	output.Normal = mul((float3x3)model, input.Normal);
	output.Pos = mul(projection, mul(view, float4(output.WorldPos, 1.0)));
	return output;
}

[Fragment shader]

float4 lights[4];
float roughness=0.1;
float metallic=1.0;
float material_r=1.0;
float material_g=0.765557;
float material_b=0.336057;

static const float PI = 3.14159265359;

//#define ROUGHNESS_PATTERN 1

float3 materialcolor()
{
	return float3(material_r, material_g, material_b);
}

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom);
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float cosTheta, float metallic)
{
	float3 F0 = lerp(float3(0.04, 0.04, 0.04), materialcolor(), metallic); // * material.specular
	float3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	return F;
}

// Specular BRDF composition --------------------------------------------

float3 BRDF(float3 L, float3 V, float3 N, float metallic, float roughness)
{
	// Precalculate vectors and dot products
	float3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	float3 lightColor = float3(1.0, 1.0, 1.0);

	float3 color = float3(0.0, 0.0, 0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		float3 F = F_Schlick(dotNV, metallic);

		float3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

// ----------------------------------------------------------------------------
float4 main(VSOutput input) : COLOR
{
	float3 N = normalize(input.Normal);
	float3 V = normalize(camPos - input.WorldPos);

	//float roughness = material.roughness;

	// Add striped pattern to roughness based on vertex position
#ifdef ROUGHNESS_PATTERN
	roughness = max(roughness, step(frac(input.WorldPos.y * 2.02), 0.5));
#endif

	// Specular contribution
	float3 Lo = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < 4; i++) {
		float3 L = normalize(lights[i].xyz - input.WorldPos);
		Lo += BRDF(L, V, N, metallic, roughness);
	};

	// Combine with ambient
	float3 color = materialcolor() * 0.02;
	color += Lo;

	// Gamma correct
	color = pow(color, float3(0.4545, 0.4545, 0.4545));

	return float4(color, 1.0);
}
