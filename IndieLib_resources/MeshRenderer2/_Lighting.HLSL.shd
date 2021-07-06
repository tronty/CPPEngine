float3 PointLight_position;
float4 PointLight_ambientColour;
float4 PointLight_diffuseColour;
float4 PointLight_specularColour;
float PointLight_ambientIntensity;
float PointLight_diffuseIntensity;
float PointLight_specularIntensity;

ROW_MAJOR float4x4 World;
ROW_MAJOR float4x4 View;
ROW_MAJOR float4x4 Projection;
float4 EyePosition;

struct VertexShaderInput
{
	float3 Position : POSITION0;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
    float3 Normal    : TEXCOORD0;
    float4 LightDir  : TEXCOORD1;
    float3 ViewDir   : TEXCOORD2;
};

[Vertex shader]
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    //Multiply position by world, view and projection matrices
    float4 worldPosition = mul(float4(input.Position, 1), World);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);

    //Calculate world space normal from model space normal
    output.Normal = mul(normalize(input.Normal), World);

    //Calculate un-normalized light direction
    output.LightDir = -(float4(PointLight_position, 1.0f) - worldPosition);

    //Calculate un-normalized view direction
    output.ViewDir = EyePosition - worldPosition;

    return output;
}

[Fragment shader]
float4 main(VertexShaderOutput input) : COLOR0
{
       //Calculate local normalized normal vector
       float4 Normal = float4(normalize(input.Normal), 1.0f);

       //Calculate local normalized view direction
       float3 ViewDir = normalize(input.ViewDir);

       //Calculate local normalized light direction
       float3 LightDir = normalize(input.LightDir);

       //Calculate the amount of diffuse light hitting this pixel
       float Diff = saturate(dot(Normal, -LightDir));

       //Calculate reflection vector of the light hitting this pixel
       float4 Reflect = normalize(2 * Diff * Normal - float4(LightDir, 1.0f));

       //Calculate the amount of specular at this pixel
       float4 Specular = pow(saturate(dot(Reflect, ViewDir)), 256);

       //If no light is hitting the pixel cut the specular power
       if (Diff <= 0.0f)
              Specular = 0.0f;

       //Calculate ambient light values
       float4 AmbientColour = PointLight_ambientColour * PointLight_ambientIntensity;

       //Calculate diffuse light values
       float4 DiffuseColour = PointLight_diffuseColour * PointLight_diffuseIntensity * Diff;

       //Calculate specular highlight colour
       float4 SpecularColour = (PointLight_specularColour * PointLight_specularIntensity) *       Specular;

       return AmbientColour + DiffuseColour + SpecularColour;
}

