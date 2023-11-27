//IndieLib HLSLShaders plainSHD2N
varying vec4 color;

[Vertex shader]
uniform vec4 scaleBias=vec4(0.003,-0.003,-1,1);
void main( )
{
	gl_Position.xy = worldViewProj * gl_Vertex.xy;
	gl_Position.w= WSIGN 1;
	color = gl_Normal;
}

[Fragment shader]
void main( )
{
	gl_FragColor = color;
}

