//-----------------------------------------------------------------------------
// File: env.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





/*
	This is a simple environment shader looking up a cubemap.
*/
[Vertex shader]

varying vec3 dir;

void main(){
	gl_Position = ftransform();

	dir = gl_Vertex.xyz;
}


[Fragment shader]

uniform samplerCube Env;

varying vec3 dir;

void main(){
	gl_FragColor = textureCube(Env, dir);
}
