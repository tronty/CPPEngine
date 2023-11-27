//IndieLib HLSLShaders texSHD2N
varying vec2 texCoord;
varying vec4 color;
varying vec4 normal;

[Vertex shader]
uniform mat4 worldViewProj;
void main( )
{
	vec4 v=vec4(gl_Vertex.xyz, 1);
	gl_Position = worldViewProj * v;
	texCoord=vec2(gl_MultiTexCoord0.xy);
	color = float4(gl_Color, 1);
	normal = float4(gl_Normal, 1);
}

[Fragment shader]
uniform sampler2D Base;
void main( )
{
	vec4 color_ = color*texture2D( Base, texCoord );//*color;
	gl_FragColor = GammaCorrect4(color_);
}

