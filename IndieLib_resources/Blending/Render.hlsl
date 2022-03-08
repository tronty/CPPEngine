[Vertex shader]

struct VS_OUTPUT
{
    float4 hposition : POSITION;
};

struct VS_OUTPUTN
{
    float4 hposition : POSITION;
    float3 normal	   : TEXCOORD0;
};

struct VS_OUTPUTT
{
    float4 hposition : POSITION;
	float2 texCoord  : TEXCOORD0;
};

struct VS_OUTPUTNT
{
    float4 hposition : POSITION;
    float3 normal	   : TEXCOORD1;
	float2 texCoord  : TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;

VS_OUTPUT main( float3 position : POSITION)
{
    VS_OUTPUT OUT;
	OUT.hposition = mul(worldViewProj, float4(position, 1));
	return OUT;
}
VS_OUTPUTN mainN( float3 position : POSITION, float3 normal : TEXCOORD0)
{
    VS_OUTPUTN OUT;
	OUT.hposition = mul( float4(position, 1),worldViewProj);
	OUT.normal = normal;
	return OUT;
}
VS_OUTPUTT mainT( float3 position : POSITION, float2 texcoord: TEXCOORD0)
{
    VS_OUTPUTT OUT;
	OUT.hposition = mul( float4(position, 1),worldViewProj);
	OUT.texCoord = texcoord;
	return OUT;
}
VS_OUTPUTNT mainNT( float3 position : POSITION, float3 normal : TEXCOORD1, float2 texcoord: TEXCOORD0)
{
    VS_OUTPUTNT OUT;
	OUT.hposition = mul( float4(position, 1),worldViewProj);
	OUT.texCoord = texcoord;
	OUT.normal = normal;
	return OUT;
}
ROW_MAJOR float4x4 matWorld;

float4 vecEye=float4(0,0,-1,1);
float4 vecLightDir=float4(0,0,-15,1);
struct VertexToPixel3
{
    float4 Pos : POSITION;
    float3 Light : TEXCOORD0;
    float3 Norm : TEXCOORD1;
    float3 View : TEXCOORD2;
    float2 Tex : TEXCOORD3;
};
VertexToPixel3 specularVS(float3 Pos : POSITION, float3 Normal : NORMAL0, float2 tex : TEXCOORD0)
{
    VertexToPixel3 Out = (VertexToPixel3)0;
    Out.Pos = mul(worldViewProj,float4(Pos,1)); // transform Position
   // Out.Pos = mul(float4(Pos,1),worldViewProj); // transform Position

    Out.Light = -vecLightDir.xyz;	       // L
    float3 PosWorld = normalize(mul(matWorld,float4(Pos,1)).xyz);
    Out.View = vecEye.xyz - PosWorld;	      // V
    Out.Norm = mul(matWorld,float4(Normal,1)).xyz;	// N
    Out.Tex=tex;
    return Out;
}

[Fragment shader]

float4 diffuse;
float4 specular;
float4 ambient;
float4 emission;
float shininess;

struct VS_OUTPUT
{
    float4 hposition : POSITION;
};

struct VS_OUTPUTN
{
    float4 hposition : POSITION;
    float3 normal	   : TEXCOORD0;
};

struct VS_OUTPUTT
{
    float4 hposition : POSITION;
	float2 texCoord  : TEXCOORD0;
};

struct VS_OUTPUTNT
{
    float4 hposition : POSITION;
    float3 normal	   : TEXCOORD1;
	float2 texCoord  : TEXCOORD0;
};



struct PS_OUTPUT
{
	float4 color : COLOR0;
};

sampler2D diffuseSampler;

float4 color=float4(0,1,0,1);
float3 light=float3(0.73, 0.73, 0.0);

PS_OUTPUT main( VS_OUTPUT IN)
{
    PS_OUTPUT OUT;
	OUT.color =  color;
	OUT.color=GammaCorrect4(OUT.color);
    return OUT;
}
PS_OUTPUT mainT( VS_OUTPUTT IN)
{
    PS_OUTPUT OUT;
	 OUT.color =  tex2D( diffuseSampler, IN.texCoord )*color;
	OUT.color=GammaCorrect4(OUT.color);
       return OUT;
}
PS_OUTPUT mainN( VS_OUTPUTN IN)
{
    PS_OUTPUT OUT;
	float3	normal	     = normalize(IN.normal);
	float dotProduct   = clamp(dot(light, normal), 0.0, 1.0);

	 OUT.color =  color;
	OUT.color	=  OUT.color * dotProduct;

	OUT.color=GammaCorrect4(OUT.color);

    return OUT;
}
PS_OUTPUT mainNT( VS_OUTPUTNT IN)
{
    PS_OUTPUT OUT;
	float3	normal	     = normalize(IN.normal);
	float dotProduct   = clamp(dot(light, normal), 0.0, 1.0);

	 OUT.color =  tex2D( diffuseSampler, IN.texCoord )*color;
	OUT.color	=  OUT.color * dotProduct;

	OUT.color=GammaCorrect4(OUT.color);
    return OUT;
}
struct VertexToPixel3
{
    float4 Pos : POSITION;
    float3 Light : TEXCOORD0;
    float3 Norm : TEXCOORD1;
    float3 View : TEXCOORD2;
    float2 Tex : TEXCOORD3;
};
float4 g_diffuse=float4( 1.0f, 0.0f, 0.0f, 1.0f);
float4 g_ambient=float4(0.1, 0.0, 0.0, 1.0);
sampler2D tex;
float4 specularPS(
VertexToPixel3 IN
//float3 Light: TEXCOORD0, float3 Norm : TEXCOORD1,
//	  float3 View : TEXCOORD2
	) : COLOR
{
#if 0
    float4 diffuse = g_diffuse;//{ 1.0f, 0.0f, 0.0f, 1.0f};
    float4 ambient = g_ambient;//{0.1, 0.0, 0.0, 1.0};

    float3 Normal = normalize(IN.Norm);
    float3 LightDir = normalize(IN.Light);
    float3 ViewDir = normalize(IN.View);
    float4 diff = saturate(dot(Normal, LightDir)); // diffuse component

    // R = 2 * (N.L) * N - L
    float3 Reflect = normalize(2 * diff.xyz * Normal - LightDir);
    float4 specular = pow(saturate(dot(Reflect, ViewDir)), 8); // R.V^n

    // I = Acolor + Dcolor * N.L + (R.V)n
    return ambient + diffuse * diff + specular;
  #else
    return tex2D(tex,IN.Tex);//float4(1,0,0,1);
  #endif
}
