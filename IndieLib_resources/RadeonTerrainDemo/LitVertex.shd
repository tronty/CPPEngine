[Vertex shader]
//ROW_MAJOR float4x4 proj;
//ROW_MAJOR float4x4 worldView;
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
struct VS_INPUT
{
	float3 position  : POSITION;
	float2 texcoord0 : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float2 texcoord0 : TEXCOORD0;
};
		  /*
sampler2D Sampler;
		*/
VS_OUTPUT mainVS( VS_INPUT IN )
{
	VS_OUTPUT OUT;

	float4 v;
#if 1
	v=float4( IN.position.x,
			       IN.position.y,
					   IN.position.z,
					   1.0f );
#else
	//v = tex2Dlod(Sampler, float4(IN.texcoord0.xy + 0.5, 0.0, 0.0));
	v = tex2Dbias(Sampler, float4(IN.texcoord0.xy + 0.5, 0.0, 0.0));//CG???
#endif

	//worldViewProj=worldView*proj;
   //OUT.hposition = mul( v,worldViewProj );//DX11
    OUT.hposition = mul( worldViewProj ,v);
        
    OUT.texcoord0 = IN.texcoord0;

    return OUT;
}

[Fragment shader]
struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float2 texcoord0 : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

sampler2D Sampler;
float4 RGBAcolor=float4(1,1,1,1);
PS_OUTPUT mainPS( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;

	float4 texcol = tex2D( Sampler, IN.texcoord0 );
	#if 1
	OUT.color = texcol;
	#else
	OUT.color = clamp(texcol, 0.75, 1.0)*RGBAcolor;
	#endif

	OUT.color=GammaCorrect4(OUT.color);
	return OUT;
}

