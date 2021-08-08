[Vertex shader]

struct VS_OUTPUT
{
    float4 hposition : POSITION;
	float2 texture0  : TEXCOORD0;
//    float4 color	   : TEXCOORD1;
};

ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;


VS_OUTPUT quad( float2 position : POSITION)
{
    VS_OUTPUT OUT;

//	OUT.hposition = mul( worldViewProj, float4(position,0, 1) );
	OUT.hposition = mul(  float4(position,0, 1),worldViewProj );

  //	  OUT.color = float4( 0.0, 1.0, 0.0, 1.0 );

	float halfPixel=0.5 / 600.0;
	OUT.texture0 = position.xy * float2(0.5, -0.5) + 0.5 + halfPixel;


	return OUT;
}
VS_OUTPUT cube( float3 position : POSITION,
		float2 tex	: TEXCOORD0)
{
    VS_OUTPUT OUT;

	OUT.hposition = mul( worldViewProj, float4(position, 1) );

    //	  OUT.color = float4( 1.0, 1.0, 1.0, 1.0 );

	//float halfPixel=0.5 / 600.0;
	OUT.texture0 = tex;//position.xy * float2(0.5, -0.5) + 0.5 + halfPixel;


	return OUT;
}

[Fragment shader]

struct VS_OUTPUT
{
    float4 hposition : POSITION;
	float2 texture0  : TEXCOORD0;
   // float4 color	   : TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 color : COLOR0;
};

sampler2D Sampler0;

PS_OUTPUT main( VS_OUTPUT IN)
{
    PS_OUTPUT OUT;
	OUT.color =  tex2D( Sampler0, IN.texture0 );
    return OUT;
}

