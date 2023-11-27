[Vertex shader]
 mat4 worldViewProj ;
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
	vec4 v=vec4(_position, 1);
	__hposition = mul( worldViewProj ,v);
    __texcoord0 = _texcoord0;
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
	return texture2D(Sampler,_texcoord0);
}

vec4 main2( VS_OUTPUT IN ) : COLOR0
{
	vec4 texcol = texture2D( Sampler, _texcoord0 )*colorRGBA;
	texcol.a=1;

	texcol=GammaCorrect4(texcol);
	return texcol;
}

