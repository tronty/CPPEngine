[Vertex shader]

struct app2vertex {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 Tangent : BINORMAL;
	float3 BiNormal : TANGENT;
};

struct vertex2fragment { 
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 oLightDir : TEXCOORD1;
	float3 oEyeVec : TEXCOORD2;
};

ROW_MAJOR float4x4 ModelViewI;
ROW_MAJOR float4x4 ModelViewIT;
ROW_MAJOR float4x4 ModelViewProj MVPSEMANTIC;
float4 LightPos;
vertex2fragment main (app2vertex IN)
{
	vertex2fragment OUT;
	float4 INPosition=float4(IN.Position, 1);
	
	float3 objLight = mul(ModelViewI, LightPos).xyz;
	objLight -= INPosition;
	
	float3x3 obj2tang;
	obj2tang[0] = IN.Tangent;
	obj2tang[1] = IN.BiNormal;
	obj2tang[2] = IN.Normal;
	
	float3 tangLight = mul (obj2tang, objLight);
	
	float3 eyeVec = ModelViewIT[3] - INPosition;
	float3 tangEyeVec = mul (obj2tang, eyeVec);
	
	OUT.Position = mul (ModelViewProj, INPosition);
	OUT.TexCoord = IN.TexCoord;
	OUT.oLightDir = tangLight;
	OUT.oEyeVec = tangEyeVec;
	
	return OUT;
}

[Fragment shader]
struct vertex2fragment { 
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 LightDir : TEXCOORD1;
	float3 EyeVec : TEXCOORD2;
};

sampler2D TexDiffuse;
sampler2D TexNormal;
sampler2D TexHeight;
float4 lightcolor;
float4 main (vertex2fragment IN) : COLOR
{
	//return float4(0,1,0,1);
	float4 normEye = normalize(float4(IN.EyeVec,1));
	float3 normLight = normalize(IN.LightDir);
	
    float selfOcclusion = saturate(normLight.z*5);
	
	float4 height = 0.04 * tex2D(TexHeight, IN.TexCoord) - 0.02;
	float2 newTexcoord = (height * normEye).xy + IN.TexCoord;
	
	float4 diffuseCol = tex2D(TexDiffuse, newTexcoord);
	float4 bumpNormal = tex2D(TexNormal, newTexcoord);
	bumpNormal *= 2;
	bumpNormal -= 1;
	float3 normBumpNormal = normalize(bumpNormal.xyz);
	
	//lightcolor = (1,1,1,1);
		
	float bump = dot(normLight, normBumpNormal);
	float4 outCol = saturate(lightcolor*bump.xxxx);

	return selfOcclusion*saturate(outCol*diffuseCol);
	
	
}

