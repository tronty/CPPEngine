#version 430

in vec2 vTexCoord;

uniform sampler2D Base;
                                                                                                                                        
out vec4 color;                                                                
                                                                               
void main(void)                                                                
{                                                                              
    color = texture(Base,vTexCoord);                   
}

