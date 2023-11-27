/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn {
	vec3 position: POSITION;
	vec2 texCoord: TEXCOORD0;
};
struct VsOut {
	vec4 position: POSITION;
	vec2 texCoord: TEXCOORD0;
};
[Vertex shader]
 mat4 worldViewProj ;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	__position = mul(worldViewProj, vec4(_position.x, _position.y, _position.z, 1.0));
	__texCoord = _texCoord;
	return Out;
}
[Fragment shader]
sampler2D Base;
vec4 color=vec4(0,1,0,1);
vec4 main(VsOut IN): COLOR {
	return texture2D(Base, _texCoord)*color;
	//return texture2D(Base, _texCoord);//*color
}


