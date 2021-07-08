#version 430

uniform sampler2D Base;

out vec4 color;
in vec2 vTexCoord;

void main()
{
    color = texture(Base, vTexCoord);
}

