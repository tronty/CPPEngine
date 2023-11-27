struct VS_INPUT
{
	vec3 position  : POSITION;
	vec4 color	 : TEXCOORD0;
	vec2 texcoord : TEXCOORD1;
};
struct VS_OUTPUT
{
	vec4 hposition : POSITION;
	vec4 color	 : TEXCOORD0;
	vec2 texcoord : TEXCOORD1;
};
[Vertex shader]
ROW_MAJOR mat4 worldViewProj MVPSEMANTIC;
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	vec4 v=vec4(IN.position, 1);
	OUT.hposition = mul( worldViewProj ,v);
    	OUT.texcoord = IN.texcoord;
	OUT.color = IN.color;
    	return OUT;
}
[Fragment shader]
struct PS_OUTPUT
{
	vec4 color : COLOR;
};
sampler2D Sampler;
int iSampler=0;
PS_OUTPUT main( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;
	OUT.color = IN.color;
	if(iSampler>0)
		OUT.color += texture2D( Sampler, IN.texcoord );
	OUT.color=GammaCorrect4(OUT.color);
	return OUT;
}

