//IndieLib HLSLShaders plainSHD2

[Vertex shader]
uniform mat4 worldViewProj;
void main( )
{
	gl_Position.xy = worldViewProj * vec4(gl_Vertex.xyz, 1.0);
	gl_Position.w= WSIGN 1;
}

[Fragment shader]
uniform vec4 colorRGBA=vec4(1,1,1,1);
void main( )
{
	gl_FragColor = colorRGBA;
}

