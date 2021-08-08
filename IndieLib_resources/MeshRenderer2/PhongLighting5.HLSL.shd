[Vertex shader]
ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
float4x4 World;

float4 vecLightDir = float4( 0.0f, 1.0f, 0.0f, 1.0f);
float4 vecEye = float4( 0.0f, 0.0f, 1.0f, 1.0f);

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
float4 AmbientColor = float4( 0.075, 0.075, 0.075, 1.0);
float4 DiffuseColor = float4( 0.75, 0.75, 0.75, 1.0);
sampler2D DIFFUSE_SAMPLER;
float4 main(VS_OUTPUT IN) : COLOR
{
	float3 Normal = normalize(IN.Norm);
	float3 LightDir = normalize(IN.Light);
	float3 ViewDir = normalize(IN.View);
	float4 diff = saturate(dot(Normal, LightDir)); // diffuse component
	float3 Reflect = normalize(2 * diff * Normal - LightDir);// R = 2 * (N.L) * N - L
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 8); // R.V^n
	float4 texcol = clamp(tex2D(DIFFUSE_SAMPLER,IN.uv), 0.75, 1.0);
	return texcol*(AmbientColor + DiffuseColor * diff + specular);
}

float4 main_(VS_OUTPUT IN) : COLOR
{
    	float3 ambientTerm = AmbientColor.xyz;
	float3 normal = normalize(IN.Norm);
	float3 LightDir = normalize(IN.Light);
	float3 ViewDir = normalize(IN.View);
   	float diffuseIntensity = saturate(dot(normal, LightDir));
    	float3 diffuseTerm = DiffuseColor.xyz * diffuseIntensity;
	float3 Reflect = normalize(2 * diffuseTerm * normal - LightDir);// R = 2 * (N.L) * N - L
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 8); // R.V^n
 
    	float3 specularTerm;
    	if (diffuseIntensity > 0)
    	{
    	    float3 halfway = normalize(LightDir + ViewDir);
    	    float specularFactor = pow(saturate(dot(normal, halfway)), 1.0);
    	    specularTerm = specular.xyz;
    	}
	float4 texcol = clamp(tex2D(DIFFUSE_SAMPLER,IN.uv), 0.75, 1.0);
	return texcol*(float4(ambientTerm + diffuseTerm + specularTerm, 1));
}

float3 Position;
float3 Normal;
float2 TexCoord;

sampler2D BaseTex;
sampler2D AlphaTex;

struct LightInfo {
  float4 Position;  // Light position in eye coords.
  float3 Intensity; // A,D,S intensity
};
LightInfo Light;

struct MaterialInfo {
  float3 Ka;            // Ambient reflectivity
  float3 Kd;            // Diffuse reflectivity
  float3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
};
MaterialInfo Material;

layout( location = 0 ) out float4 FragColor;

float3 phongModel( float3 pos, float3 norm ) {
    float3 s = normalize(float3(Light.Position) - pos);
    float3 v = normalize(-pos.xyz);
    float3 r = reflect( -s, norm );
    float3 ambient = Light.Intensity * Material.Ka;
    float sDotN = max( dot(s,norm), 0.0 );
    float3 diffuse = Light.Intensity * Material.Kd * sDotN;
    float3 spec = float3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
               pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return ambient + diffuse + spec;
}

void main() {
    float4 baseColor = texture( BaseTex, TexCoord );
    float4 alphaMap = texture( AlphaTex, TexCoord );

    if(alphaMap.a < 0.15 )
        discard;
    else {
        FragColor = float4( phongModel(Position,Normal), 1.0 ) *
                baseColor;
    }
}
