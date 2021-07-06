[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
struct TerrainVS_INPUT
{
    float3 position     : POSITION;
    float3 normal       : NORMAL;
    float2 MultiTexCoord0 : TEXCOORD0;
};
struct TerrainVS_OUTPUT
{
    float4 position : POSITION;
    float2 TexCoord0  : TEXCOORD0;
    float4 TexCoord1  : TEXCOORD1;

};

TerrainVS_OUTPUT main(TerrainVS_INPUT IN)
{
        TerrainVS_OUTPUT OUT;
	OUT.TexCoord0 = IN.MultiTexCoord0;
        OUT.TexCoord1 = float4(IN.normal, 0.0);
        OUT.position = mul(worldViewProj,float4(IN.position,1));
        return OUT;
      }
[Fragment shader]
struct TerrainVS_OUTPUT
{
    float4 position : POSITION;
    float2 TexCoord0  : TEXCOORD0;
    float4 TexCoord1  : TEXCOORD1;

};

struct TerrainPS_OUTPUT
{
        float4 color : COLOR0;
};

      sampler2D weight;
      sampler2D fungus;
      sampler2D dirt;
      sampler2D grass;

float4 main(TerrainVS_OUTPUT IN) :COLOR0
{
        float4 OUT;
        float4 weightSample = tex2D(weight, IN.TexCoord0.xy),
               fungusSample = tex2D(fungus, IN.TexCoord0.xy*25.0),
               grassSample  = tex2D(grass , IN.TexCoord0.xy*20.0),
               dirtSample   = tex2D(dirt  , IN.TexCoord0.xy*20.0);

        float weightScale1  = clamp(weightSample.y * 1.25, 0.0, 1.0);
        float weightScale2  = clamp(weightSample.x, 0.0, 1.0);

        float3  normal       = normalize(IN.TexCoord1.xyz);
	//return float4(normal, 1);
        float dotProduct   = clamp(dot(float3(0.73, 0.73, 0.0), normal), 0.0, 1.0);

        float4  dirtAndGrass = dirtSample   * weightScale1 + (1.0 - weightScale1) * grassSample;
        //return dirtAndGrass;
        float4  color        = weightScale2 * dirtAndGrass + (1.0 - weightScale2) * fungusSample;
        //return color;
        OUT       = color * dotProduct;

	OUT.rgb=GammaCorrect3(OUT.rgb);
	OUT.a=1.0;
	//OUT=float4(1, 0, 0, 1);
        return OUT;
}

