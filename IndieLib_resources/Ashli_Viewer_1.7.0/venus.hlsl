/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
// vertex to fragment shader io
struct VERTEX_OUT {
	float4 Position	: POSITION;
	float3 N	: NORMAL;
	float3 I	: TEXCOORD1;
	float4 Cs	: TEXCOORD2;
	float4 Pobj	: TEXCOORD3;
};

[Vertex shader]

struct VERTEX_IN {
    float3 Position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 UV		: TEXCOORD1;
};
ROW_MAJOR 
float4x4 worldViewProj 
MVPSEMANTIC
;

VERTEX_OUT main(VERTEX_IN IN)
{
	VERTEX_OUT OUT = (VERTEX_OUT)0;
    // position and neighbors in object space
    OUT.Position = float4(IN.Position, 1.0);
    OUT.Pobj = float4(IN.Position, 1.0);

    float3x3 NormalMatrix =  (float3x3)worldViewProj;

    // normal transform (transposed model-view inverse)
    OUT.N = mul(NormalMatrix , IN.Normal);

    // position in eye space
    float4 P =  mul(worldViewProj , OUT.Pobj);

    // incident vector
    OUT.I = (P - float4 (0,0,0,1)).xyz;

    // color
    OUT.Cs = float4(IN.Color, 1);
	return OUT;
}

[Fragment shader]

// random/hash function              
float hash( float n )
{
  return frac(cos(n)*41415.92653);
}

// 3d noise function
float noise( in float3 x )
{
  float3 p  = floor(x);
  float3 f  = smoothstep(0.0, 1.0, frac(x));
  float n = p.x + p.y*57.0 + 113.0*p.z;

  return lerp(lerp(lerp( hash(n+  0.0), hash(n+  1.0),f.x),
    lerp( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
    lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
    lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

#define snoise(x) ((2*noise(x))-1)

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

float PI = 3.14159265359;
float TWOPI = 2*3.14159265359;

#define octaves 8

float Ka=0.0;// range 0 1
float Kd=0.7;// range 0 1
float offset=1.36;// range 0 2
float scale=0.31;// range -5 5
float twist=0.51;// range -1 1
float omega=0.966;// range -1.5 1.5
float texScale=0.0031;// range -0.05 0.05
float intensity1=1.0;
float4 lightcolor1=float4(1,1,1,1);
float3 dir1=float3(-1,-1,-1);

// ambient Function
float4 ambient()
{
    float4 C;
    float4 Cl1;
    C = float4 (0,0,0,1);
    Cl1 = intensity1 * lightcolor1;
    C = C + Cl1;
    return C;
}


// diffuse function
float4 diffuse(float3 Nn)
{
    float4 C;
    float3 L2;
    float4 Cl2;
    C = float4 (0,0,0,1);
    L2 = dir1;
    Cl2 = lightcolor1 * intensity1;
    L2 = -L2;
    C = C + Cl2 * max(0, dot(Nn, normalize(L2)));
    return C;
}

float4 main(VERTEX_OUT IN) : COLOR
{
  float4 Ptexture;           /* the shade point in texture space */
  float3 PtN;                /* normalized version of Ptexture */
  float3 PP;                 /* Point after rotation by coriolis twist */
  float rsq;                /* Used in calculation of twist */
  float angle;              /* Twist angle */
  float sine, cosine;       /* sin and cos of angle */
  float l, o, a, i;         /* Loop control for fractal sum */
  float value;              /* Fractal sum is stored here */
  float4 baseColor;

  baseColor = float4(0.78125, 0.3515625, 0, 1);

  /* Transform to texture coordinates */
	Ptexture = IN.Pobj;

  Ptexture *= texScale;

  /* Calculate Coriolis twist, yielding point PP */
  PtN = Ptexture.xyz;
  PtN = normalize (PtN);
  rsq = PtN.x * PtN.x + PtN.y * PtN.y;
  angle = twist * TWOPI * rsq;
  sine = sin (angle);
  cosine = cos (angle);
  PP = float3(Ptexture.x*cosine - Ptexture.y*sine,
	      Ptexture.x*sine + Ptexture.y*cosine,
	      Ptexture.z);

  /* Compute VLfBm */
  l = 1;  o = 1;  a = 0;
  for (i = 0;  i < octaves;  i += 1) {
  	float3 f1=PP*l;
  	float f2=noise(f1);
      a += o * f2;//snoise(PP * l);
      l *= 2;
      o *= omega;
    }

  value = abs (offset + scale * a);

  float3 Nf = faceforward(normalize(IN.N), IN.I, IN.N);
	
	return baseColor;
  /* Shade like matte, but with color scaled by cloud color */
	return IN.Cs * (value * baseColor) * (Ka * ambient() +
					Kd * diffuse(Nf));
}

