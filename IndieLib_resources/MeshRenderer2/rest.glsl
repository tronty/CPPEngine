varying vec2 uv;
varying vec4 color;

[Vertex shader]
uniform mat4 WorldViewProjection;
void main()
{
	vec3 objPos = gl_Vertex.xyz;
	gl_Position = vec4( objPos, 1.0) * WorldViewProjection;
	uv=vec2(gl_MultiTexCoord0.xy);
	color=gl_Color;
}
void main2()
{
	vec3 objPos = gl_Vertex.xyz;
	gl_Position = vec4( objPos, 1.0) * WorldViewProjection;
	uv=vec2(gl_MultiTexCoord0.xy);
	color=gl_Color;
}
[Fragment shader]

uniform sampler2D DIFFUSE_SAMPLER;
void main()
{
	gl_FragColor = texture2D(DIFFUSE_SAMPLER, uv);
}
void main2()
{
	gl_FragColor = texture2D(DIFFUSE_SAMPLER, uv)+vec4(color.xyz, 1);
}

