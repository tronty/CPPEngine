varying vec2 texCoord;
[Vertex shader]
//IndieLib GLSLShaders texSHD3
uniform mat4 worldViewProj;
uniform int bbb=0;
attribute vec3 position;
void main(void){
   if(bbb==0)
	gl_Position = worldViewProj * vec4(position, 1);
	else
	gl_Position = vec4(position, 1) * worldViewProj;
	#if 0
	texCoord=vec2(gl_MultiTexCoord0.x, 1.0-gl_MultiTexCoord0.y);
	#else
	texCoord=vec2(gl_MultiTexCoord0.xy);
	#endif
}
[Fragment shader]
uniform sampler2D Sampler;
uniform vec4 colorRGBA=vec4(1,1,1,1);
void main(void){
	gl_FragColor = texture2D(Sampler, texCoord);//*colorRGBA;
}

