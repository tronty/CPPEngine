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

