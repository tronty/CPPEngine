#define texture(a1,a2) texture2D(a1,a2)
// BRDF approximation sample count (higher is better quality and slower)
const int BRDF_SAMPLE_COUNT = 64;

// reflection convolution sample count (higher is better quality and slower)
const int REFLECTION_SAMPLE_COUNT = 256;

// irradiance convolution step (lower is better quality and slower)
const float sampleDelta = 0.1;


const float PI = 3.14159265358979323846;
float roughness = 0.0;
float metallic = 0.0;

float GeometrySchlickGGX(float NdotV, float roughness) {
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float VanDerCorpus(int n, int base) {
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;

    for(int i = 0; i < 32; ++i)
    {
        if(n > 0)
        {
            denom   = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = int(float(n) / 2.0);
        }
    }

    return result;
}

vec2 Hammersley(int i, int N) {
    return vec2(float(i)/float(N), VanDerCorpus(i, 2));
}  

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

vec3 filtered_reflection(vec3 D, vec3 N) {
 
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);    
   
  	for(int i = 0; i < REFLECTION_SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, REFLECTION_SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(D, H) * H - D);

        float NdotL = max(dot(N, D), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(iChannel0, L).xyz * 2.0 * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
    return prefilteredColor;    
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    for(int i = 0; i < BRDF_SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, BRDF_SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(BRDF_SAMPLE_COUNT);
    B /= float(BRDF_SAMPLE_COUNT);
    return vec2(A, B);
}


vec3 light(vec3 N) {
    vec3 irradiance = vec3(0.0);
        
	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, N);
	up         = cross(N, right);
 
	float nrSamples = 0.0; 
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
    	for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
    	{
        	vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
        	vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 
            
            irradiance += texture(iChannel0, sampleVec).xyz * 2.0 * cos(theta) * sin(theta);
       	 	nrSamples++;
    	}
	}
	irradiance = PI * irradiance * (1.0 / nrSamples);
    return irradiance;
    
}

vec3 surface(vec3 D, vec3 N) {
    vec3 albedo = vec3(1.0, 0.5, 0.0);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    
    vec3 F = fresnelSchlickRoughness(max(dot(N, D), 0.0), F0, roughness);
	vec3 prefilteredColor = filtered_reflection(D, N);
    
    vec3 R = reflect(D, N);
    
 	vec2 brdf = IntegrateBRDF(dot(D, N), roughness);
    
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 diffuse  = light(N) * albedo;
    return (kD * diffuse + specular);
}


float scene(vec3 position) {
    float height = 0.3;
    return length(position)-height;
}

vec3 getNormal(vec3 ray_hit_position, float smoothness) {	
	vec3 n;
	vec2 dn = vec2(smoothness, 0.0);
	n.x	= scene(ray_hit_position + dn.xyy) - scene(ray_hit_position - dn.xyy);
	n.y	= scene(ray_hit_position + dn.yxy) - scene(ray_hit_position - dn.yxy);
	n.z	= scene(ray_hit_position + dn.yyx) - scene(ray_hit_position - dn.yyx);
	return normalize(n);
}

float raymarch(vec3 position, vec3 direction) {
    float total_distance = 0.0;
    for(int i = 0 ; i < 16 ; ++i) {
        float result = scene(position + direction * total_distance);
        if(result < 0.005)
        {
            return total_distance;
        }
        total_distance += result;
    }
    return -1.0;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord.xy / iResolution.y;
    
    roughness = iMouse.x/iResolution.x;
    metallic = iMouse.y/iResolution.y;
    
    uv -= vec2(0.5*iResolution.x/iResolution.y, 0.5);
    vec3 direction = normalize(vec3(uv, 2.5));
    vec3 origin = vec3(0.0, 0.0, -2.5);
    float dist = raymarch(origin, direction);
    if(dist < 0.0) {
		fragColor = vec4(0.0, 0.2, 0.2, 1.0);
    } else{
        vec3 fragPosition = origin+direction*dist;
 		vec3 N = getNormal(fragPosition, 0.01);
        fragColor = mix(vec4(0, .2, .2, 1), vec4(surface(-direction, N), 1), smoothstep(.0, .15, dot(-direction, N) - .1));

    }
}
