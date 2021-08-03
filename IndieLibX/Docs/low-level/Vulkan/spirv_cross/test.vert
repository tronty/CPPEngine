#version 420                                                         

in vec2 aPosition;
in vec2 aTexCoord;

out vec2 vTexCoord;

layout(location = 0) in mat4 worldViewProj;
                                                               
void main(void)                                                                
{                                                                              
	gl_Position = worldViewProj*vec4(aPosition.x, aPosition.y, 0, 1);
	vTexCoord=aTexCoord;                                       
}

