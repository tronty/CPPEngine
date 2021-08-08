#version 420

layout(location = 0) in mat4 worldViewProj;
in vec2 aPosition;
out vec2 vTexCoord;
in vec2 aTexCoord;

void main()
{
    gl_Position = worldViewProj * vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    vTexCoord = aTexCoord;
}

