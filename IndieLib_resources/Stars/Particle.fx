struct vertex
{
	float3 position : POSITION;
	float4 diffuse	: COLOR0;
	float2 texcoord : TEXCOORD0;
	float2 offset	: TEXCOORD1;
	float4 allowedRotDir : TEXCOORD2;
};

struct fragment
{
	float4 position : POSITION;
	float4 color0	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

uniform float4x4 viewProj;
uniform float4 right;
uniform float4 up;
uniform float4 xCamPos;
struct PS_OUTPUT
{
	float4 color : COLOR;
};
//texture testTexture; // This texture will be loaded by the application
sampler Sampler;/* = sampler_state
{
    Texture   = (testTexture);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};*/
float4 brightness=float4(1,1,1,1);
[Vertex shader]
fragment mainVS( vertex IN)
{
	fragment OUT;

	float4 v = float4( IN.position.x,
			       IN.position.y,
					   IN.position.z,
					   1.0f );


	float4 r1;
	if(IN.allowedRotDir.w==0)
	{
	r1=IN.offset.y*up+IN.offset.x*right+v;		
	}
	else
	{
    float3 center = v.xyz;//mul(inPos, xWorld);
	    
    float3 Forward=normalize(xCamPos.xyz-center);
	float3 upVector = -IN.allowedRotDir.xyz;
	float3 sideVector=normalize(cross(upVector,Forward));
	upVector=normalize(cross(sideVector,Forward));

    float3 finalPosition = center;
	finalPosition+=IN.offset.x*sideVector;
	finalPosition+=IN.offset.y*upVector;
    r1 = float4(finalPosition, 1);    
	}
	OUT.position = mul( viewProj, r1 );
	OUT.color0 = IN.diffuse;
	OUT.texcoord = IN.texcoord;
	return OUT;
}
[Fragment shader]
PS_OUTPUT mainPS( fragment IN )
{
	PS_OUTPUT OUT;
	
	////OUT.color = tex2D( Sampler/*testTexture*/, IN.texcoord0 ) * IN.color0; // Modulate texel color with vertex color
	////OUT.color = tex2D( Sampler/*testTexture*/, IN.texcoord0 ) + IN.color0; // Add texel color to vertex color
	
	OUT.color = tex2D( Sampler/*testTexture*/, IN.texcoord );
	//OUT.color.rgb*=brightness.x;	

	OUT.color=GammaCorrect4(OUT.color);
	return OUT;
}
/*
technique Particle
{
    pass Pass0
    {
		Lighting = FALSE;

		Sampler[0] = (Sampler); // Needed by pixel shader

		VertexShader = compile vs_1_1 mainVS();
		PixelShader  = compile ps_2_0 mainPS();
    }
}
*/

