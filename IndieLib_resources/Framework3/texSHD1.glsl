//IndieLib HLSLShaders texSHD1
varying vec2 texCoord;

[Vertex shader]
uniform mat4 worldViewProj
void main( )
{
	vec4 v=vec4(gl_Vertex.xyz, 1);
	gl_Position = worldViewProj * v;
	texCoord=vec2(gl_MultiTexCoord0.xy);
}

[Fragment shader]
uniform sampler2D Sampler;
uniform vec4 colorRGBA;
varying vec2 texCoord;
void main( )
{
	gl_FragColor = texture2D( Sampler, texCoord ) * colorRGBA;
}

void main2( )
{
	vec4 texcol = texture2D( Sampler, texCoord ) * colorRGBA;
	texcol.a=1;
	texcol=GammaCorrect4(texcol);
	gl_FragColor = texcol;
}

