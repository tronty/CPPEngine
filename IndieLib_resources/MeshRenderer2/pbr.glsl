// https://github.com/SaschaWillems/Vulkan.git
// pbr.vert 
// pbr.frag
varying vec4 _Pos;
varying vec3 _Light;
varying vec3 _Normal;
varying vec3 _View;
varying vec2 _uv;
varying vec3 _PosWorld;

[Vertex shader]
mat4 WorldViewProjection=mat4(0);
mat4 World=mat4(0);
vec4 vecLightDir = vec4( 0.0f, 1.0f, 0.0f, 1.0f);
vec4 vecEye = vec4( 0.0f, 0.0f, 1.0f, 1.0f);
struct VS_INPUT
{
	vec3 Pos;
	vec3 Normal;
	vec3 BiNormal;
	vec3 Tangent;
	vec3 color;
	vec2 uv;
};
void mainVS(VS_INPUT IN)
{
	_Pos = WorldViewProjection * vec4(IN.Pos,1);
	gl_Position = _Pos;
	_Light = vecLightDir.xyz;
	_PosWorld = normalize(World * vec4(IN.Pos,1)).xyz;
	_View = vecEye.xyz - _PosWorld;
	_Normal = normalize(World * vec4(IN.Normal,1)).xyz;
	_uv=IN.uv;
}
void main()
{
    VS_INPUT xlt_In;
    xlt_In.Pos = vec3(gl_Vertex);
    xlt_In.uv = vec2(gl_MultiTexCoord0);
    mainVS( xlt_In);
}

[Fragment shader]
vec3 camPos=vec3(0.0, 0.0, -1.0);
vec4 lights[4];
float roughness=0.1;
float metallic=1.0;
vec3 color=vec3(1.0, 0.765557, 0.336057);
const float PI = 3.14159265359;
//#define ROUGHNESS_PATTERN 1

vec3 materialcolor()
{
	return vec3(color.r, color.g, color.b);
}

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
vec3 F_Schlick(float cosTheta, float ametallic)
{
	vec3 F0 = mix(vec3(0.04), materialcolor(), ametallic); // * material.specular
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
	return F;    
}

// Specular BRDF composition --------------------------------------------

vec3 BRDF(vec3 L, vec3 V, vec3 N, float ametallic, float aroughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, aroughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, aroughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, aroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, ametallic);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

vec4 mainPS()
{		  
	vec3 N = normalize(_Normal);
	vec3 V = normalize(camPos - _PosWorld);

	float roughness_ = roughness;

	// Add striped pattern to roughness based on vertex position
#ifdef ROUGHNESS_PATTERN
	roughness_ = max(roughness_, step(frac(_PosWorld.y * 2.02), 0.5));
#endif

	// Specular contribution
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 4; i++) {
		vec3 L = normalize(lights[i].xyz - _PosWorld);
		Lo += BRDF(L, V, N, metallic, roughness_);
	};

	// Combine with ambient
	vec3 color = materialcolor() * 0.02;
	color += Lo;

	//color=GammaCorrect3(color);

	return vec4(color, 1.0);
}

void main(){gl_FragColor=mainPS();}

