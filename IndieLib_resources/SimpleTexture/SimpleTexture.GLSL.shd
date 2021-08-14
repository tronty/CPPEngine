/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn {
	vec2 position;
	vec2 texCoord;
};
struct VsOut {
	vec4 position;
	vec2 texCoord;
};
mat4 worldViewProj;
[Vertex shader]

attribute vec2 VsInposition;
attribute vec2 VsIntexCoord;

varying vec4 VsOutposition;
varying vec2 VsOuttexCoord;

VsOut __main__(VsIn In)
{
	VsOut Out=(VsOut)0;
	Out.position = worldViewProj* vec4(In.position.x, In.position.y, 0.0, 1.0);
	Out.texCoord = In.texCoord;
	return Out;
}

void main()
{
	VsIn In;
	In.position=VsInposition;
	In.texCoord=VsIntexCoord;
	VsOut OUT=__main__(In);
	VsOutposition=OUT.position;
	VsOuttexCoord=OUT.texCoord;
}
[Fragment shader]

varying vec4 VsOutposition;
varying vec2 VsOuttexCoord;
uniform sampler2D Base;

vec4 __main__(VsOut IN)
{
	return texture2D(Base, IN.texCoord);
}

void main()
{
	VsOut IN;
	IN.position=VsOutposition;
	IN.texCoord=VsOuttexCoord;
	glFragColor=__main__(IN);
}

