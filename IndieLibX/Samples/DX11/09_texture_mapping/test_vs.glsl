#version 410

//define VS 1

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vt; // per-vertex texture co-ords

#ifdef VS
uniform sampler2D basic_texture;
#endif
uniform mat4 view, proj;

out vec2 texture_coordinates;
#ifdef VS
out vec4 texel;
#endif

void main() {
	texture_coordinates = vt;
	gl_Position = proj * view * vec4 (vertex_position, 1.0);

#ifdef VS
#if 0
	texel = tex2Dlod (basic_texture, texture_coordinates);
#else
	texel = texture (basic_texture, texture_coordinates);
#endif
#endif
}
