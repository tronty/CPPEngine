//IndieLib HLSLShaders plainSHD1

[Vertex shader]
uniform vec4 scaleBias=vec4(0.003,-0.003,-1,1);
void main( )
{
	gl_Position.xy = gl_Vertex.xy * scaleBias.xy + scaleBias.zw;
	gl_Position.w= WSIGN 1;
}

[Fragment shader]
uniform sampler2D Base;
uniform vec4 colorRGBA=vec4(1,1,1,1);
void main( )
{
	gl_FragColor = colorRGBA;
}



