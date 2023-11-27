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
ROW_MAJOR mat4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, vec4(In.position.x, In.position.y, In.position.z, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]
sampler2D Base;
vec4 color=vec4(0,1,0,1);
vec4 main(VsOut IN): COLOR {
	return texture2D(Base, IN.texCoord)*color;
	//return texture2D(Base, IN.texCoord);//*color
}


