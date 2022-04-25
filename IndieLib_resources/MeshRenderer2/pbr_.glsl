// https://github.com/SaschaWillems/Vulkan.git
// pbr.vert 
// pbr.frag
//define ROUGHNESS_PATTERN 1
#if 0
#define ROW_MAJOR
#define MVPSEMANTIC
#endif

mat4 projection=mat4(0);
mat4 model=mat4(0);
mat4 view=mat4(0);
vec3 camPos=vec3(10.0, 13.0, 1.8);

struct VSOutput
{
	vec4 Pos;
	vec3 Normal;
};
varying vec3 WorldPos;
varying vec3 _Normal;

[Vertex shader]
struct VSInput
{
	vec3 Pos;
	vec3 Normal;
	vec3 BiNormal;
	vec3 Tangent;
	vec3 color;
	vec2 uv;
};

vec3 objPos=vec3(0.0, 0.0, 0.0);

VSOutput mainVS(VSInput In)
{
	VSOutput Out;
	vec3 locPos = (model * vec4(In.Pos, 1.0)).xyz;
	WorldPos = locPos + objPos;
	Out.Normal = (model * vec4(In.Normal, 1.0)).xyz;
	Out.Pos = (projection * (view * vec4(WorldPos, 1.0)));
	return Out;
}
void main()
{
    VSOutput xl_retval;
    VSInput xlt_In;
    xlt_In.Pos = vec3(gl_Vertex);
    xlt_In.uv = vec2(gl_MultiTexCoord0);
    xl_retval = mainVS( xlt_In);
    gl_Position = vec4(xl_retval.Pos);
    _Normal = vec3(xl_retval.Normal);
}

[Fragment shader]

vec4 lights[4];
float roughness=0.1;
float metallic=1.0;
float material_r=1.0;
float material_g=0.765557;
float material_b=0.336057;

const float PI = 3.14159265359;

//#define ROUGHNESS_PATTERN 1

vec3 materialcolor()
{
	return vec3(material_r, material_g, material_b);
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
vec3 F_Schlick(float cosTheta, float metallic)
{
	vec3 F0 = mix(vec3(0.04, 0.04, 0.04), materialcolor(), metallic); // * material.specular
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	return F;
}

// Specular BRDF composition --------------------------------------------

vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness)
{
	// Precalculate vectors and dot products
	vec3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	vec3 color = vec3(0.0, 0.0, 0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, metallic);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

// ----------------------------------------------------------------------------
vec4 mainPS(vec3 InNormal, vec3 InWorldPos)
{
	vec3 N = normalize(InNormal);
	vec3 V = normalize(camPos - InWorldPos);

	//float roughness = material.roughness;

	// Add striped pattern to roughness based on vertex position
#ifdef ROUGHNESS_PATTERN
	roughness = max(roughness, step(fract(InWorldPos.y * 2.02), 0.5));
#endif

	// Specular contribution
	vec3 Lo = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < 4; i++) {
		vec3 L = normalize(lights[i].xyz - InWorldPos);
		Lo += BRDF(L, V, N, metallic, roughness);
	};

	// Combine with ambient
	vec3 color = materialcolor() * 0.02;
	color += Lo;

	// Gamma correct
	color = pow(color, vec3(0.4545, 0.4545, 0.4545));

	return vec4(color, 1.0);
}
void main() {gl_FragColor = mainPS(_Normal, WorldPos);}

