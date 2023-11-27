[Vertex shader]
uniform mat4 WorldViewProjection;
attribute vec2 Position_;
attribute vec2 uv_;
uniform  vec3 color_;
varying vec4 _Position;
varying vec3 _color;
varying vec2 _uv;
void main()
{
	vec3 objPos = Position_;
	_Position = vec4( objPos, 1.0) * WorldViewProjection;
	gl_Position = _Position;
	_uv=uv_;
	_color=color_;
}
void main2()
{
	vec3 objPos = Position_;
	_Position = vec4( objPos, 1.0) * WorldViewProjection;
	gl_Position = _Position;
	_uv=_uv_;
	_color=_color_;
}
[Fragment shader]
varying vec4 _Position;
varying vec3 _color;
varying vec2 _uv;
uniform sampler2D DIFFUSE_SAMPLER;
void main()
{
	//vec4 texcol = texture2D(DIFFUSE_SAMPLER,_uv);
	gl_FragColor = texture2D(DIFFUSE_SAMPLER,_uv);
}
void main2()
{
	//vec4 texcol = texture2D(DIFFUSE_SAMPLER,_uv);
	gl_FragColor = texture2D(DIFFUSE_SAMPLER,_uv)+vec4(_color, 1);
}

