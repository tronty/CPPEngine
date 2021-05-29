[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float4x4 World;
float4 AmbientColor = float4( 0.075, 0.075, 0.075, 1.0);
float4 DiffuseColor = float4( 0.75, 0.75, 0.75, 1.0);
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
	Out.Pos = mul(float4(IN.Pos,1), WorldViewProjection);
	Out.Light = vecLightDir.xyz;
	float3 PosWorld = normalize(mul(float4(IN.Pos,1), World)).xyz;
	Out.View = vecEye.xyz - PosWorld;
	Out.Norm = normalize(mul(float4(IN.Normal,1), World)).xyz;
	Out.uv=IN.uv;
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
float3 GGX_Specular( TextureCube SpecularEnvmap, float3 normal, float3 viewVector, float roughness, float3 F0, out float3 kS )
{
    float3 reflectionVector = reflect(-viewVector, normal);
    float3x3 worldFrame = GenerateFrame(reflectionVector);
    float3 radiance = 0;
    float  NoV = saturate(dot(normal, viewVector));

    for(int i = 0; i < SamplesCount; ++i)
    {
        // Generate a sample vector in some local space
        float3 sampleVector = GenerateGGXsampleVector(i, SamplesCount, roughness);
        // Convert the vector in world space
        sampleVector = normalize( mul( sampleVector, worldFrame ) );

        // Calculate the half vector
        float3 halfVector = normalize(sampleVector + viewVector);
        float cosT = saturatedDot( sampleVector, normal );
        float sinT = sqrt( 1 - cosT * cosT);

        // Calculate fresnel
        float3 fresnel = Fresnel_Schlick( saturate(dot( halfVector, viewVector )), F0 );
        // Geometry term
        float geometry = GGX_PartialGeometryTerm(viewVector, normal, halfVector, roughness) * GGX_PartialGeometryTerm(sampleVector, normal, halfVector, roughness);
        // Calculate the Cook-Torrance denominator
        float denominator = saturate( 4 * (NoV * saturate(dot(halfVector, normal)) + 0.05) );
        kS += fresnel;
        // Accumulate the radiance
        radiance += SpecularEnvmap.SampleLevel( trilinearSampler, sampleVector, ( roughness * mipsCount ) ).rgb * geometry * fresnel * sinT / denominator;
    }

    // Scale back for the samples count
    kS = saturate( kS / SamplesCount );
    return radiance / SamplesCount;        
}

float4 main_(VS_OUTPUT IN) : COLOR
{
    float3 surface = tex2D(surfaceMap_Sampler, input.Texcoord).rgb;   
    float ior = 1 + surface.r;
    float roughness = saturate(surface.g - EPSILON) + EPSILON;
    float metallic = surface.b;

    // Calculate colour at normal incidence
    float3 F0 = abs ((1.0 - ior) / (1.0 + ior));
    F0 = F0 * F0;
    F0 = lerp(F0, materialColour.rgb, metallic);
        
    // Calculate the specular contribution
    float3 ks = 0;
    float3 specular = GGX_Specular(specularCubemap, normal, viewVector, roughness, F0, ks );
    float3 kd = (1 - ks) * (1 - metallic);
    // Calculate the diffuse contribution
    float3 irradiance = texCUBE(diffuseCubemap_Sampler, normal ).rgb;
    float3 diffuse = materialColour * irradiance;

    return float4( kd * diffuse + /*ks **/ specular, 1);     
}
