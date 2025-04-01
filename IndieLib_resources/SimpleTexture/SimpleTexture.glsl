/*
  Copyright (c) 2022 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

uniform sampler2D Base;
void main() {
	gl_FragColor=
	//vec4(1,1,1,1);
	texture2D(Base, xlv_TEXCOORD0.xy);
}

