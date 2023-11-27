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
 mat4 worldViewProj ;
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	vec4 v=vec4(_position, 1);
	__hposition = mul( worldViewProj ,v);
    	__texcoord = _texcoord;
	__color = _color;
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
	__color = _color;
	if(iSampler>0)
		__color += texture2D( Sampler, _texcoord );
	__color=GammaCorrect4(__color);
	return OUT;
}

