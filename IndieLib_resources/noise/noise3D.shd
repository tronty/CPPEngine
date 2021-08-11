/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
float time;

float noiseFreq = 0.05;

float noiseAmp = 1.0;

float distanceScale = 1.0;

float3 timeScale = float3( 0, -0.01, 0 );
int iI=0;
sampler2D tExplosion;

float rand_float(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
  float fc = fract(p);
	return mix(rand_float(fl), rand_float(fl + 1.0), fc);
}

float3 mod289(float3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float2 mod289(float2 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float3 permute(float3 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(float2 v)
{
    const float4 C = float4(0.211324865405187, // (3.0-sqrt(3.0))/6.0
                        0.366025403784439, // 0.5*(sqrt(3.0)-1.0)
                       -0.577350269189626, // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    // First corner
    float2 i = floor(v + dot(v, C.yy) );
    float2 x0 = v - i + dot(i, C.xx);

    // Other corners
    float2 i1;
    //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    //i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    float3 p = permute( permute( i.y + float3(0.0, i1.y, 1.0 )) +
                      i.x + float3(0.0, i1.x, 1.0 ));

    float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    float3 x = 2.0 * fract(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

    // Compute final noise value at P
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

float fbm(float2 p, int octaves)
{
    float f = 0.0;

    //const mat2 m = mat2(0.8, 0.6, -0.6, 0.8);
    float a = 0.5;
    float t = 1.0;

    int i;

    for (i = 0; i < octaves; i++)
    {
        f += a * snoise(p);
        p *= 2.02;
        t *= a;
        a *= 0.5;
    }

    return f / (1.0 - t);
}

//define fract frac
//define mix lerp

//
// GLSL textureless classic 2D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-08-22
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/stegu/webgl-noise
//

float4 mod289(float4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 permute(float4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

float4 taylorInvSqrt(float4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float2 fade(float2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(float2 P)
{
  float4 Pi = floor(P.xyxy) + float4(0.0, 0.0, 1.0, 1.0);
  float4 Pf = fract(P.xyxy) - float4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  float4 ix = Pi.xzxz;
  float4 iy = Pi.yyww;
  float4 fx = Pf.xzxz;
  float4 fy = Pf.yyww;

  float4 i = permute(permute(ix) + iy);

  float4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  float4 gy = abs(gx) - 0.5 ;
  float4 tx = floor(gx + 0.5);
  gx = gx - tx;

  float2 g00 = float2(gx.x,gy.x);
  float2 g10 = float2(gx.y,gy.y);
  float2 g01 = float2(gx.z,gy.z);
  float2 g11 = float2(gx.w,gy.w);

  float4 norm = taylorInvSqrt(float4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;  
  g01 *= norm.y;  
  g10 *= norm.z;  
  g11 *= norm.w;  

  float n00 = dot(g00, float2(fx.x, fy.x));
  float n10 = dot(g10, float2(fx.y, fy.y));
  float n01 = dot(g01, float2(fx.z, fy.z));
  float n11 = dot(g11, float2(fx.w, fy.w));

  float2 fade_xy = fade(Pf.xy);
  float2 n_x = mix(float2(n00, n01), float2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

float4 mod(float4 a1, float2 a2){return float4(0.0, 0.0, 0.0, 0.0);}

// Classic Perlin noise, periodic variant
float pnoise(float2 P, float2 rep)
{
  float4 Pi = floor(P.xyxy) + float4(0.0, 0.0, 1.0, 1.0);
  float4 Pf = fract(P.xyxy) - float4(0.0, 0.0, 1.0, 1.0);
  //Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
  Pi = mod289(Pi);        // To avoid truncation effects in permutation
  float4 ix = Pi.xzxz;
  float4 iy = Pi.yyww;
  float4 fx = Pf.xzxz;
  float4 fy = Pf.yyww;

  float4 i = permute(permute(ix) + iy);

  float4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  float4 gy = abs(gx) - 0.5 ;
  float4 tx = floor(gx + 0.5);
  gx = gx - tx;

  float2 g00 = float2(gx.x,gy.x);
  float2 g10 = float2(gx.y,gy.y);
  float2 g01 = float2(gx.z,gy.z);
  float2 g11 = float2(gx.w,gy.w);

  float4 norm = taylorInvSqrt(float4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;  
  g01 *= norm.y;  
  g10 *= norm.z;  
  g11 *= norm.w;  

  float n00 = dot(g00, float2(fx.x, fy.x));
  float n10 = dot(g10, float2(fx.y, fy.y));
  float n01 = dot(g01, float2(fx.z, fy.z));
  float n11 = dot(g11, float2(fx.w, fy.w));

  float2 fade_xy = fade(Pf.xy);
  float2 n_x = mix(float2(n00, n01), float2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

// https://www.clicktorelease.com/blog/vertex-displacement-noise-3d-webgl-glsl-three-js/
float turbulence(float2 point)
{
  float value=0.0;
  float freq=1.0;
  value += abs(snoise(point));
  value += abs(snoise(point)*2.0)/2.0;
  value += abs(snoise(point)*4.0)/4.0;
  value += abs(snoise(point)*8.0)/8.0;
  return clamp(value, -1.0, 1.0);
}

float3 fireball(float3 p)
{
	float d = length(p);
//	d += tex_noise(p*noiseFreq)*noiseAmp;
	d += turbulence(p*noiseFreq + time*timeScale).x*noiseAmp;
	#if 0
	float4 c = tex1D(tExplosion, d*distanceScale);
	#else
	float2 tPos = float2( d*distanceScale, 0);
	float3 c = tex2D( tExplosion, tPos ).rgb;
	#endif
	return c;
}

struct VsIn {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 texCoord	: TEXCOORD1;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
[Vertex shader]
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1));
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]

float4 main(VsOut In) : COLOR
{
	#if 0
    float2 p = In.texCoord;
	#else
    float2 p = 8.0 * In.texCoord * (1.2 + sin(time * 1000.0));
	#endif
    float r = length(p);
    float a = atan(p.y, p.x);

    float3 rgb;
    float f = 0;

    // float f = rand_float(p.x + p.y * 290 + time * 100.0);
	if(iI==0)
    		f = fbm(In.texCoord, 8); // 1.0 * a * 3.14159267), 4);
	else
    		f = turbulence(In.texCoord); // 1.0 * a * 3.14159267), 4);

    rgb = float3(f);

	rgb=fireball(rgb);

    float4 c1=float4(rgb * 0.5 + float3(0.5), 1.0);

  // get a random offset
  //float r = .01 * random( float3( 12.9898, 78.233, 151.7182 ), 0.0, In.position.xyz );
  r = f;
  // lookup vertically in the texture, using noise and offset
  // to get the right RGB colour
  float2 tPos = float2( 1.3 * noise(r) + r + 0.4, 0);
  float4 color = tex2D( tExplosion, tPos );
	//color=clamp(color, 0.0, 1.0);

  color = c1*float4( color.rgb, 1.0 );
  return color;

}

