struct SkyVS_OUTPUT
{
    float4 position : POSITION;
    float4 TexCoord : TEXCOORD0;
    float2 intensityintensitySqrd : TEXCOORD1;
};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;

struct SkyVS_INPUT
{
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 BiNormal  	: BINORMAL;
    float3 Tangent  	: TANGENT;
    float3 Color  	: TEXCOORD0;
    float2 Tex  	: TEXCOORD1;
};

#if 1
float4  params=float4(-0.167888, -0.035063, 0.930527, 1.0);
float3  offset=float3(2560.000000, 2560.000000, -5100.000000);
#else
float4  params=float4(0.000000, 0.000000, 1.000333, 1.0);
float3  offset=float3(324.739990, 193.405914, -2237.400879);
#endif

SkyVS_OUTPUT main(SkyVS_INPUT IN)
{
	SkyVS_OUTPUT OUT;
#if 1
        float2 newUV  = float2((IN.position.x - offset.x)/params.x,
			   (IN.position.z - offset.z)/params.z);
	float paramsw=1000.0*params.w;

	OUT.TexCoord.x	= newUV.x + paramsw*0.66;
	OUT.TexCoord.y	= newUV.y + paramsw*0.33;
	OUT.TexCoord.xy *= 2.0;

	OUT.TexCoord.z = newUV.x + paramsw*1.33;
	OUT.TexCoord.w = newUV.y + paramsw*1.66;
	OUT.TexCoord.zw *= 3.0;

	//Get the vertex height and scale it to 0 - 1 range
	OUT.intensityintensitySqrd.x = IN.position.y/params.y;
	OUT.intensityintensitySqrd.x = exp(clamp(OUT.intensityintensitySqrd.x, 0.0, 1.0))/2.8;
	OUT.intensityintensitySqrd.y = OUT.intensityintensitySqrd.x*OUT.intensityintensitySqrd.x;
#else
	OUT.TexCoord=float2(IN.Tex.x+params.w, IN.Tex.y+params.w);
#endif
	OUT.position = mul(worldViewProj,float4(IN.position,1));
	//OUT.position = mul(float4(IN.position,1),worldViewProj);
	return OUT;
      }
[Fragment shader]

struct SkyPS_OUTPUT
{
	float4 color : COLOR;
};

sampler2D noise;
float4 skyInfo=float4(0.25, 0.5, 1.0, 0.0);

SkyPS_OUTPUT main(SkyVS_OUTPUT IN)
{
	SkyPS_OUTPUT OUT;
#if 1
	float4 noise1	  = tex2D(noise, IN.TexCoord.xy),
	     noise2	= tex2D(noise, IN.TexCoord.zw);

	float4 cloudFrag  = (noise1 + noise2) *  IN.intensityintensitySqrd.y,
	     cloudColor = float4((1.0 - IN.intensityintensitySqrd.x)*skyInfo.x,
			       (1.0 - IN.intensityintensitySqrd.x)*skyInfo.y,
			       IN.intensityintensitySqrd.x*skyInfo.z, 0.0);

	OUT.color    = cloudColor * (1.0 - cloudFrag.x) + cloudFrag;
	//OUT.color=(noise1 + noise2);
	//OUT.color=noise2;

	OUT.color.rgb=GammaCorrect3(OUT.color.rgb);
	OUT.color.a=1.0;
#else
	OUT.color=tex2D(noise, IN.TexCoord);
#endif
	//OUT.color=float4(0, 0, 1, 1);
	return OUT;
}

