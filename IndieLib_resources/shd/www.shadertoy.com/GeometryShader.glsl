[Vertex shader]

#if 0
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
#else
uniform mat4 worldViewProj;
#endif

void main()
{
#if 0
	vec4 world_pos = modelMatrix * gl_Vertex;
	vec4 view_pos = viewMatrix * world_pos;
	gl_Position = projectionMatrix * view_pos;
#else
	gl_Position = worldViewProj * gl_Vertex;
#endif
}

[Fragment shader]

void main(void)
{
	gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}

[Geometry shader]

//#version 330 compatibility
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

void main()
{	
	for(int i=0; i<3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}

