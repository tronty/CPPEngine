[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;

struct GrassVS_INPUT
{
    float3 position	: POSITION;
    float3 normal	: NORMAL;
    float3 offset : TEXCOORD0; // offset
    float3 tangent : TANGENT; // tangent
    float3 binormal : BINORMAL; // binormal
    float3 uvJitter : TEXCOORD1; // uvJitter
};
struct GrassVS_OUTPUT
{
    float4 position : POSITION;
    float3 TexCoord  : TEXCOORD0;

};

      float elapsedTime;
 
GrassVS_OUTPUT main(GrassVS_INPUT IN)
{
	GrassVS_OUTPUT OUT;
#if 0
	float4 oldVertex = float4(IN.position,1);

	float4x4 newMatrix = float4x4(float4(IN.tangent.xyz, 0.0),
			      float4(IN.normal, 0.0),
			      float4(IN.binormal.xyz, 0.0),
			      0.0, 0.0, 0.0, 1.0);

	if(IN.uvJitter.y > 0.2)
	{
	  float cosine	 = cos(elapsedTime)*IN.uvJitter.z*10.0;
	  oldVertex.x	+= cosine;
	  oldVertex.z	+= cosine;
	}

	float4 finalVertex	= mul(newMatrix,oldVertex);
	     finalVertex.xyz += IN.offset.xyz;

	OUT.TexCoord = IN.uvJitter;
	OUT.position = mul(worldViewProj, finalVertex);
#else
	float4 oldVertex = float4(IN.position,1);

	if(IN.uvJitter.y > 0.2)
	{
	  float cosine	 = cos(elapsedTime)*IN.uvJitter.z*10.0;
	  oldVertex.x	+= cosine;
	  oldVertex.z	+= cosine;
	}
	float3 v=oldVertex.xyz+IN.offset.xyz;
	OUT.position = mul(worldViewProj, float4(v,1));
	OUT.TexCoord = IN.uvJitter;
#endif
	return OUT;
      }
[Fragment shader]
struct GrassVS_OUTPUT
{
    float4 position : POSITION;
    float3 TexCoord  : TEXCOORD0;

};

struct GrassPS_OUTPUT
{
	float4 color : COLOR;
};
      sampler2D grass;
      float	alphaBooster=1.0;

GrassPS_OUTPUT main(GrassVS_OUTPUT IN)
{
	GrassPS_OUTPUT OUT;
	float2 uv=IN.TexCoord.xy;
	uv.y=1-uv.y;
	float4	grassSample = tex2D(grass, uv);
	float newAlpha	  = grassSample.a*alphaBooster;
	OUT.color      = float4(grassSample.xyz, newAlpha);

	OUT.color.rgb=GammaCorrect3(OUT.color.rgb);
	//OUT.color=float4(0, 1, 0, 1);
	//OUT.color=grassSample;
	return OUT;
}

