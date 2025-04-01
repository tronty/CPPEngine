/*
  Copyright (c) 2022 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
// vertex to fragment shader io
varying vec4 Pobj;
varying vec3 N;
varying vec3 I;
varying vec4 Cs;

[Vertex shader]

uniform mat4 ModelViewMatrix=mat4(0.0);

void main()
{
    // position and neighbors in object space
    Pobj = gl_Vertex;

    mat4 NormalMatrix =  ModelViewMatrix;

    // normal transform (transposed model-view inverse)
    N = (NormalMatrix * gl_Normal).xyz;

    // position in eye space
    vec4 P =  ModelViewMatrix * gl_Vertex;

    // incident vector
    I = (P - vec4 (0)).xyz;

    // color
    Cs = gl_Color;
}

[Fragment shader]
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2003, ATI Technologies, Inc., All rights reserved.
//
// Permission to use, copy, modify, and distribute this software and its 
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appear in all copies and derivative
// works and that both the copyright notice and this permission notice appear in
// support documentation, and that the name of ATI Technologies, Inc. not be used
// in advertising or publicity pertaining to distribution of the software without
// specific, written prior permission.
//
///////////////////////////////////////////////////////////////////////////////

/*
 * GLSL port of RenderMan(R) venus test shader
 *
 */

#define TWOPI (2*PI)

#define octaves 8

uniform float Ka=0.0;// range 0 1
uniform float Kd=0.7;// range 0 1
uniform float offset=1.36;// range 0 2
uniform float scale=0.31;// range -5 5
uniform float twist=0.51;// range -1 1
uniform float omega=0.966;// range -1.5 1.5
uniform float texScale=0.0031;// range -0.05 0.05
uniform float intensity1=1.0;
uniform vec4 lightcolor1=vec4(1);
uniform vec3 dir1=vec3(-1);

// ambient Function
vec4 ambient()
{
    vec4 C;
    vec4 Cl1;
    C = vec4 (0,0,0,1);
    Cl1 = intensity1 * lightcolor1;
    C = C + Cl1;
    return C;
}


// diffuse function
vec4 diffuse(vec3 Nn)
{
    vec4 C;
    vec3 L2;
    vec4 Cl2;
    C = vec4 (0,0,0,1);
    L2 = dir1;
    Cl2 = lightcolor1 * intensity1;
    L2 = -L2;
    C = C + Cl2 * max(0, dot(Nn, normalize(L2)));
    return C;
}

void main()
{
  vec4 Ptexture;           /* the shade point in texture space */
  vec3 PtN;                /* normalized version of Ptexture */
  vec3 PP;                 /* Point after rotation by coriolis twist */
  float rsq;                /* Used in calculation of twist */
  float angle;              /* Twist angle */
  float sine, cosine;       /* sin and cos of angle */
  float l, o, a, i;         /* Loop control for fractal sum */
  float value;              /* Fractal sum is stored here */
  vec4 baseColor;

  baseColor = vec4(0.78125, 0.3515625, 0, 1);

  /* Transform to texture coordinates */
	Ptexture = Pobj;

  Ptexture *= texScale;

  /* Calculate Coriolis twist, yielding point PP */
  PtN = vec3(Ptexture);
  PtN = normalize (PtN);
  rsq = PtN.x * PtN.x + PtN.y * PtN.y;
  angle = twist * TWOPI * rsq;
  sine = sin (angle);
  cosine = cos (angle);
  PP = vec3(Ptexture.x*cosine - Ptexture.y*sine,
	      Ptexture.x*sine + Ptexture.y*cosine,
	      Ptexture.z);

  /* Compute VLfBm */
  l = 1;  o = 1;  a = 0;
  for (i = 0;  i < octaves;  i += 1) {
      a += o * noise1(PP * l);
      l *= 2;
      o *= omega;
    }

  value = abs (offset + scale * a);

  vec3 Nf = faceforward(normalize(N), I, N);
	
  /* Shade like matte, but with color scaled by cloud color */
	gl_FragColor = Cs * (value * baseColor) * (Ka * ambient() +
					Kd * diffuse(Nf));
	gl_FragColor = baseColor;
}








