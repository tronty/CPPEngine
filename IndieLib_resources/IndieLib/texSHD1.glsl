varying vec4 color;
[Vertex shader]
//IndieLib GLSLShaders texSHD1
uniform mat4 worldViewProj;
uniform int bbb=0;
attribute vec3 position;
attribute vec4 color_;
void main(void){
   if(bbb==0)
	gl_Position = worldViewProj * vec4(position, 1);
	else
	gl_Position = vec4(position, 1) * worldViewProj;
	color=color_;
}
[Fragment shader]
void main(void){
	gl_FragColor = color;
}

