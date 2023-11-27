[Vertex shader]
vec4 scaleBias=vec4(0.003,-0.003,-1,1);
attribute vec2 position;
attribute vec2 texCoord_;
varying vec2 texCoord;
void main(void)
{
	gl_Position.xy = position.xy * scaleBias.xy + scaleBias.zw;
	gl_Position.w= + 1;
	texCoord = texCoord_;
}
[Fragment shader]
uniform vec4 colorRGBA=vec4(1,1,1,1);
uniform sampler2D Base;
varying vec2 texCoord;
void main (void)
{ 
	gl_FragColor = texture2D(Base, texCoord)* colorRGBA;
}

