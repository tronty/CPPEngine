[Vertex shader]
ROW_MAJOR mat4 worldViewProj MVPSEMANTIC;
struct VS_INPUT
{
	vec3 position  : POSITION;
	vec2 texcoord0 : TEXCOORD0;
};
struct VS_OUTPUT
{
	vec4 hposition : POSITION;
	vec2 texcoord0 : TEXCOORD0;
};
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	vec4 v=vec4(IN.position, 1);
	OUT.hposition = mul( worldViewProj ,v);
    OUT.texcoord0 = IN.texcoord0;
    return OUT;
}
[Fragment shader]
struct VS_OUTPUT
{
	vec4 hposition : POSITION;
	vec2 texcoord0 : TEXCOORD0;
};
sampler2D Sampler;
vec4 colorRGBA=vec4(1,1,1,1);
vec4 main(VS_OUTPUT IN) : COLOR
{
	return texture2D(Sampler,IN.texcoord0);
}

vec4 main2( VS_OUTPUT IN ) : COLOR0
{
	vec4 texcol = texture2D( Sampler, IN.texcoord0 )*colorRGBA;
	texcol.a=1;

	texcol=GammaCorrect4(texcol);
	return texcol;
}

