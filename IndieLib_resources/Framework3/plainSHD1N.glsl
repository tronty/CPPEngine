//IndieLib HLSLShaders plainSHD1N
varying vec4 color;

[Vertex shader]
uniform vec4 scaleBias=vec4(0.003,-0.003,-1,1);
void main( )
{
	gl_Position.xy = gl_Vertex.xy * scaleBias.xy + scaleBias.zw;
	gl_Position.w= WSIGN 1;
	color = gl_Normal;
}

[Fragment shader]
void main( )
{
	gl_FragColor = color;
}

