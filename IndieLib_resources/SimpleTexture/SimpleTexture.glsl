/*
  Copyright (c) 2023 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
varying vec2 texCoord;
[Vertex shader]
uniform vec4 scaleBias=vec4(0.003,-0.003,-1,1);
void main(void)
{
	gl_Position.xy = vec2(gl_Vertex).xy * scaleBias.xy + scaleBias.zw;
	gl_Position.w= + 1;
	texCoord = vec2(gl_MultiTexCoord0.xy);
}

[Fragment shader]
uniform sampler2D Base;
void main() {
	gl_FragColor=
	//gl_Color * 
	texture2D(Base, texCoord.xy);
}

