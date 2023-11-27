//IndieLib HLSLShaders texSHD0
varying vec2 texCoord;

[Vertex shader]
uniform vec4 scaleBias=vec4(0.003,-0.003,-1,1);
void main( )
{
	gl_Position.xy = gl_Vertex.xy * scaleBias.xy + scaleBias.zw;
	gl_Position.w= WSIGN 1;
	texCoord=vec2(gl_MultiTexCoord0.xy);
}

[Fragment shader]
uniform sampler2D Base;
uniform vec4 colorRGBA;
void main( )
{
	gl_FragColor = texture2D( Base, texCoord ) * colorRGBA;
}

