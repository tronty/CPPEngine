struct VS_OUTPUT
{
	float4 Pos: POSITION;
	float3 Normal : NORMAL;
	float3 Color : TEXCOORD0;
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
	Out.Color = IN.color;
	Out.Normal = mul(float4(IN.Normal,1), WorldViewProjection);
	return Out;
}

[Fragment shader]
float3 LightSourcePos=float3((10.0, 5.0, 0.0);
float4 main(VS_OUTPUT IN) : COLOR
{
	float3 nn = normalize(IN.Normal);
  float3 light_pos = LightSourcePos;
  float3 light_dir = normalize(IN.Pos - light_pos);
  float3 eye_dir = normalize(-IN.Pos);
  float3 reflect_dir = normalize(reflect(light_dir, nn));
	
  float spec = max(dot(reflect_dir, eye_dir), 0.0);
	float diffuse = max(dot(-light_dir, nn), 0.0);

  float intensity = 0.6 * diffuse + 0.4 * spec;

 	if (intensity > 0.9) {
 		intensity = 1.1;
 	}
 	else if (intensity > 0.5) {
 		intensity = 0.7;
 	}
 	else {
 		intensity = 0.5;
  }

	return float4(IN.Color, 1) * intensity;
}

