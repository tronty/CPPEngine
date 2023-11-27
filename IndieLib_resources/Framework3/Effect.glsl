/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//
// Noise Shader Library for Unity - https://github.com/keijiro/NoiseShader
//
// Original work (webgl-noise) Copyright (C) 2011 Stefan Gustavson
// Translation and modification was made by Keijiro Takahashi.
//
// This shader is based on the webgl-noise GLSL shader. For further details
// of the original shader, please see the following description from the
// original source code.
//

//
// GLSL textureless classic 3D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-10-11
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/ashima/webgl-noise
//

vec3 mod(vec3 x, vec3 y)
{
  return x - y * floor(x / y);
}

vec3 mod289(vec3 x)
{
  return x - floor(x / 289.0) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x / 289.0) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return (vec4)1.79284291400159 - r * 0.85373472095314;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + (vec3)1.0; // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = frac(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - (vec3)1.0; // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
  vec4 iz0 = (vec4)Pi0.z;
  vec4 iz1 = (vec4)Pi1.z;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
  gx0 = frac(gx0);
  vec4 gz0 = (vec4)0.5 - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, (vec4)0.0);
  gx0 -= sz0 * (step((vec4)0.0, gx0) - 0.5);
  gy0 -= sz0 * (step((vec4)0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
  gx1 = frac(gx1);
  vec4 gz1 = (vec4)0.5 - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, (vec4)0.0);
  gx1 -= sz1 * (step((vec4)0.0, gx1) - 0.5);
  gy1 -= sz1 * (step((vec4)0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;

  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.y, Pf0.z));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.x, Pf1.y, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.x, Pf0.y, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.y, Pf1.z));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = lerp(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

// Classic Perlin noise, periodic variant
float pnoise(vec3 P, vec3 rep)
{
  vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
  vec3 Pi1 = mod(Pi0 + (vec3)1.0, rep); // Integer part + 1, mod period
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = frac(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - (vec3)1.0; // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
  vec4 iz0 = (vec4)Pi0.z;
  vec4 iz1 = (vec4)Pi1.z;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
  gx0 = frac(gx0);
  vec4 gz0 = (vec4)0.5 - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, (vec4)0.0);
  gx0 -= sz0 * (step((vec4)0.0, gx0) - 0.5);
  gy0 -= sz0 * (step((vec4)0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
  gx1 = frac(gx1);
  vec4 gz1 = (vec4)0.5 - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, (vec4)0.0);
  gx1 -= sz1 * (step((vec4)0.0, gx1) - 0.5);
  gy1 -= sz1 * (step((vec4)0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.y, Pf0.z));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.x, Pf1.y, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.x, Pf0.y, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.y, Pf1.z));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = lerp(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

float noise( vec3 a)
{
	return cnoise(a);
}


// https://www.clicktorelease.com/blog/vertex-displacement-noise-3d-webgl-glsl-three-js/
float turbulence( vec3 p )
{
  float w = 100.0;
  float t = -.5;
  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise( vec3( power * p ), vec3( 10.0, 10.0, 10.0 ) ) / power );
  }
  return t;
}

varying vec3 pos;
varying vec3 xlv_Normal_;
varying vec3 xlv_Binormal_;
varying vec3 xlv_Tangent_;
varying vec3 xlv_Color_;
varying vec2 uv;
[Vertex shader]
uniform mat4 worldViewProj;
void main1()
{
	gl_Position=vec4(gl_Vertex.xyz, 1) * worldViewProj;
    	uv=vec2(gl_MultiTexCoord0.xy);
}
void main( )
{
	pos=vec4(gl_Vertex.xyz, 1) * worldViewProj;
	gl_Position=pos;
    	xlv_Normal_=xlv_Normal;
    	xlv_Binormal_=xlv_Binormal;
    	xlv_Tangent_=xlv_Tangent;
    	xlv_Color_=xlv_Color;
    	uv=vec2(gl_MultiTexCoord0.xy);
}
[Fragment shader]
uniform sampler2D colorSampler;
uniform sampler2D noiseSampler;

void main( )
{
	vec4 np4=noise(pos.xyz);
	np4=texture2D(noiseSampler, uv);
	vec4 texcol = np4 * texture2D(colorSampler, uv.x);
	texcol.a=1;

	gl_FragColor=GammaCorrect4(texcol);
}

void main2( )
{
	vec4 np4=noise(pos.xyz);
	np4=texture2D(noiseSampler, uv);
	vec4 texcol = np4 * texture2D(colorSampler, uv.x);
	texcol.a=1;

	gl_FragColor=GammaCorrect4(texcol);
}

float turbulence(int octaves, vec3 P, float lacunarity, float gain)
{	
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*noise(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}

#define iCloud 1
#define iFur 2
#define iMarble 3
#define iWood 4
#define iPNoise 5

int iI=6;

void main()
{
  vec3 v_texCoord3D=pos.xyz;
  vec3 color1, color2, diffuse;
  if(iI==iPNoise)
  {
  	color1 = vec3(0.1f, 0.1f, 0.99);
  	color2 = vec3(0.8f, 0.8, 0.8f);  
  	vec3 diffuse=lerp(color1, color2, cos(v_texCoord3D.z*0.5+2*turbulence(6,v_texCoord3D.xyz,2,0.5))*0.9);
  }
  else if(iI==iCloud)
  {
  	color1 = vec3(0.1f, 0.1f, 0.99);
  	color2 = vec3(0.8f, 0.8, 0.8f);    
  	diffuse=lerp(color1, color2, cos(v_texCoord3D.z*0.5+2*turbulence(6,v_texCoord3D.xyz,2,0.5))*0.9);
  }
  else if(iI==iFur)
  {
  color1 = vec3(0.8, 0.7, 0.0);
  color2 = vec3(0.6, 0.1, 0.0);
  diffuse=lerp(color1, color2, clamp(0.7*turbulence(6,v_texCoord3D.xyz,3,0.9), 0,1));
  }
  else if(iI==iMarble)
  {
  	color1 = vec3(0.1f, 0.8f, 0.2);
 	color2 = vec3(0.8f, 0.8, 0.8f);
      	diffuse=lerp(color1, color2, cos(v_texCoord3D.z*0.1+6*turbulence(5,v_texCoord3D.xyz,2,0.6)));
  }
  else if(iI==iWood)
  {
  	color1 = vec3(0.658, 0.356, 0.0588);
  	color2 = vec3(0.952, 0.713, 0.470);
  float x_v=v_texCoord3D.x*0.6;
  float y_v=v_texCoord3D.y*0.6;
  float z_v=v_texCoord3D.z*0.6;
  vec3 t1=vec3(x_v*2,y_v*2,z_v*2);
  vec3 t11=vec3(x_v*2+1,y_v*2+1,z_v*2+1);
  vec3 t2=vec3(x_v*0,y_v*0,z_v*0);
  float temp_x=60*noise(t1);
  float temp_y=60*noise(t2);
  x_v=x_v-temp_x;
  y_v=y_v-temp_y;
  diffuse=lerp(color1, color2, pow(cos(sqrt(x_v*x_v+y_v*y_v)+30*turbulence(68, t2, 2, 0.6))*0.5+0.5,3));
  }
  else
  {
  	float n = noise(vec3(4.0 * v_texCoord3D.xyz));
  	gl_FragColor=vec4(0.5 + 0.85 * vec3(n, n, n), 1.0);  
  }
  gl_FragColor=vec4(diffuse, 1.0);
}



