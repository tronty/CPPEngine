struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float3 color : TEXCOORD0;
};
const float3 diffuseMaterial = float3(0.0, 0.0, 1.0);
const float3 specularMaterial = float3(1.0, 1.0, 1.0);
float3 lightVec = float3( -1.0, 0, 0);
[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float4x4 World;
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
	float3 normalVec=IN.Normal;
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(float4(IN.Pos, 1), WorldViewProjection);
	Out.Norm = normalize(mul(float4(IN.Normal, 1), World)).xyz;

    // calculate half angle vector
    float3 eyeVec = float3(0.0, 0.0, 1.0);
    float3 halfVec = normalize(lightVec + eyeVec);
    
    // calculate diffuse component
    float3 diffuse = float3(max(dot(normalVec, lightVec), 0.0)) * diffuseMaterial;

    // calculate specular component
    float3 specular = float3(max(dot(normalVec, halfVec), 0.0));
    specular = pow(specular.x, 32.0) * specularMaterial;
    
    // combine diffuse and specular contributions and output final vertex color
    Out.color.rgb = diffuse + specular;
    //Out.color.a = 1.0;

	return Out;
}

[Fragment shader]
float4 main(VS_OUTPUT IN) : COLOR
{
	float3 normal=IN.Norm;
    float3 normalVec = normalize(normal);

    // calculate half angle vector
    float3 eyeVec = float3(0.0, 0.0, 1.0);
    float3 halfVec = normalize(lightVec + eyeVec);
    
    // calculate diffuse component
    float3 diffuse = float3(max(dot(normalVec, lightVec), 0.0)) * diffuseMaterial;

    // calculate specular component
    float3 specular = float3(max(dot(normalVec, halfVec), 0.0));
    specular = pow(specular.x, 32.0) * specularMaterial;
    
	float4 color;
    // combine diffuse and specular contributions and output final vertex color
    color.rgb = diffuse + specular;
    color.a = 1.0;
	return color;
}

