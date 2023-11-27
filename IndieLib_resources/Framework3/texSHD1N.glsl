//IndieLib HLSLShaders texSHD1N
varying vec2 texCoord;
varying vec4 color;

[Vertex shader]
uniform mat4 worldViewProj;
void main( )
{
	vec4 v=vec4(gl_Vertex.xyz, 1);
	gl_Position = worldViewProj * v;
	texCoord=vec2(gl_MultiTexCoord0.xy);
	color = vec4(gl_Normal, 1);
}

[Fragment shader]
uniform sampler2D Base;
varying vec4 color;
varying vec2 texCoord;
void main( )
{
	vec4 color = texture2D( Base, texCoord );//*color;
	gl_FragColor = GammaCorrect4(color);
}

