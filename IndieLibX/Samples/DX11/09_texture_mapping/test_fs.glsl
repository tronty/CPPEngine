#version 410

//define VS 1

in vec2 texture_coordinates;
#ifdef VS
in vec4 texel;
#endif
uniform sampler2D basic_texture;
out vec4 frag_colour;

void main() {
#ifndef VS
	vec4 texel_ = texture (basic_texture, texture_coordinates);
	frag_colour = texel_;
#else
	frag_colour = texel;
#endif
}
