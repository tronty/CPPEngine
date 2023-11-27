[Vertex shader]
uniform vec4 scaleBias=vec4(0.003,-0.003,-1,1);
varying vec2 texCoord;
void main(void)
{
	gl_Position.xy = vec2(gl_Vertex).xy * scaleBias.xy + scaleBias.zw;
	gl_Position.w= + 1;
	texCoord = vec2(gl_MultiTexCoord0.xy);
}
[Fragment shader]
uniform vec4 colorRGBA=vec4(1,1,1,1);
uniform sampler2D Base;
varying vec2 texCoord;
void main (void)
{ 
	gl_FragColor = texture2D(Base, texCoord) * colorRGBA * gl_Color;
}

