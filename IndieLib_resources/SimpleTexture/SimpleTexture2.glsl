/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

varying vec2 texCoord;

[Vertex shader]

uniform mat4 worldViewProj;

void main()
{
	gl_Position = worldViewProj* vec4(gl_Vertex.x, gl_Vertex.y, 0.0, 1.0);
	texCoord = vec2(gl_MultiTexCoord0.xy);
}
[Fragment shader]

uniform sampler2D Base;

void main()
{
	glFragColor=texture2D(Base, texCoord);
}

