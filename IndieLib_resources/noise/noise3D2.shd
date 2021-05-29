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

float turbulence2(float2 point, float size)
{
  float value = 0, initialSize = size;

  while(size >= 1)
  {
    value += snoise(float2(point.x / size, point.y / size)) * size;
    size /= 2;
  }

  return(128.0 * value / initialSize);
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
int to255(float v) { return (int)min(255,256*v); }
#define Uint8 to255
#define fabs abs

#define noiseWidth  128
#define noiseHeight 128

#if 0

/** Helper method that converts hue to rgb */
float hueToRgb(float p, float q, float t) {
    if (t < 0f)
        t += 1f;
    if (t > 1f)
        t -= 1f;
    if (t < 1f/6f)
        return p + (q - p) * 6f * t;
    if (t < 1f/2f)
        return q;
    if (t < 2f/3f)
        return p + (q - p) * (2f/3f - t) * 6f;
    return p;
}
#endif
/**
* HSV to RGB color conversion
*
* H runs from 0 to 360 degrees
* S and V run from 0 to 100
*
* Ported from the excellent java algorithm by Eugene Vishnevsky at:
* http://www.cs.rit.edu/~ncs/color/t_convert.html
*/
float3 HSVtoRGB(float3 hsv)
{
    float3 rgb;
    float i;
    float f, p, q, t;
     
    // Make sure our arguments stay in-range
    hsv.x = max(0, min(360, hsv.x));
    hsv.y = max(0, min(100, hsv.y));
    hsv.z = max(0, min(100, hsv.z));
     
    // We accept saturation and value arguments from 0 to 100 because that's
    // how Photoshop represents those values. Internally, however, the
    // saturation and value are calculated from a range of 0 to 1. We make
    // That conversion here.
    hsv.y /= 100;
    hsv.z /= 100;
     
    if(hsv.s == 0) {
        // Achromatic (grey)
        rgb.x = hsv.z;
        rgb.y = hsv.z;
        rgb.z = hsv.z;
        return float3(
            round(rgb.x * 255), 
            round(rgb.y * 255), 
            round(rgb.z * 255)
        );
    }
     
    hsv.x /= 60; // sector 0 to 5
    i = floor(hsv.x);
    f = hsv.x - i; // factorial part of h
    p = hsv.z * (1 - hsv.y);
    q = hsv.z * (1 - hsv.y * f);
    t = hsv.z * (1 - hsv.y * (1 - f));
     
    if(i==0) {
        rgb.x = hsv.z;
        rgb.y = t;
        rgb.z = p;
    } else if(i==1) {
        rgb.x = q;
        rgb.y = hsv.z;
        rgb.z = p;
    } else if(i==2) {
        rgb.x = p;
        rgb.y = hsv.z;
        rgb.z = t;
    } else if(i==3) {
        rgb.x = p;
        rgb.y = q;
        rgb.z = hsv.z;
    } else if(i==4) {
       rgb.x = t;
       rgb.y = p;
       rgb.z = hsv.z;
    } else {
       rgb.x = hsv.z;
       rgb.y = p;
       rgb.z = q;
    }
     
    return float3(
        round(rgb.x * 255), 
        round(rgb.y * 255), 
        round(rgb.z * 255)
    );
}
/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param h       The hue
 * @param s       The saturation
 * @param l       The lightness
 * @return int array, the RGB representation
 */
float3 HSLtoRGB(float3 hsl) // float h, float s, float l)
{
    float h=hsl.x;
    float s=hsl.y;
    float l=hsl.z;
    float3 rgb;
    float m, c, x;
	/*
    if (!isfinite(h)) h = 0;
    if (!isfinite(s)) s = 0;
    if (!isfinite(l)) l = 0;
	*/
    h /= 60;
    if (h < 0) h = 6 - (-h % 6);
    h = h % 6;

    s = max(0, min(1, s / 100));
    l = max(0, min(1, l / 100));

    c = (1 - abs((2 * l) - 1)) * s;
    x = c * (1 - abs((h % 2) - 1));

    if (h < 1) {
        rgb.x = c;
        rgb.y = x;
        rgb.z = 0;
    } else if (h < 2) {
        rgb.x = x;
        rgb.y = c;
        rgb.z = 0;
    } else if (h < 3) {
        rgb.x = 0;
        rgb.y = c;
        rgb.z = x;
    } else if (h < 4) {
        rgb.x = 0;
        rgb.y = x;
        rgb.z = c;
    } else if (h < 5) {
        rgb.x = x;
        rgb.y = 0;
        rgb.z = c;
    } else {
        rgb.x = c;
        rgb.y = 0;
        rgb.z = x;
    }

    m = l - c / 2;
    rgb.x = round((rgb.x + m) * 255);
    rgb.y = round((rgb.y + m) * 255);
    rgb.z = round((rgb.z + m) * 255);

    return rgb;
}

float turbulence(int octaves, float3 P, float lacunarity, float gain)
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
		//xPeriod and yPeriod together define the angle of the lines
		//xPeriod and yPeriod both 0 ==> it becomes a normal clouds or turbulence pattern
		float xPeriod = 5.0; //defines repetition of marble lines in x direction
		float yPeriod = 10.0; //defines repetition of marble lines in y direction
		//turbPower = 0 ==> it becomes a normal sine pattern
		float turbPower = 5.0; //makes twists
		float turbSize = 32.0; //initial size of the turbulence

    // float f = rand_float(p.x + p.y * 290 + time * 100.0);
	if(iI==0)
	{
    		f = fbm(In.texCoord, 8); // 1.0 * a * 3.14159267), 4);
		rgb = float3(f);
	}
	else if(iI==1)
	{
    		f = turbulence(In.texCoord); // 1.0 * a * 3.14159267), 4);
		rgb = float3(f);
	}
	else if(iI==2) 
	{   		
		f = turbulence2(In.texCoord, 64);
		rgb = float3(f);
	}
	else if(iI==3)
	{
		rgb = HSLtoRGB(float3(169, 255, (192 + Uint8(turbulence2(In.texCoord, 64)) / 4)));
	}
	else if(iI==4)
	{
		float xyValue = In.texCoord.x * xPeriod / noiseWidth + In.texCoord.y * yPeriod / noiseHeight + turbPower * turbulence2(In.texCoord, turbSize) / 256.0;
    		float sineValue = 256 * fabs(sin(xyValue * 3.14159));
    		rgb.r = rgb.g = rgb.b = Uint8(sineValue);
	}
	else if(iI==5)
	{
		float xyValue = In.texCoord.x * xPeriod / noiseWidth + In.texCoord.y * yPeriod / noiseHeight + turbPower * turbulence2(In.texCoord, turbSize) / 256.0;
		float sineValue = 226 * fabs(sin(xyValue * 3.14159));
    		rgb.r = Uint8(30 + sineValue);
    		rgb.g = Uint8(10 + sineValue);
    		rgb.b = Uint8(sineValue);
	}
	else if(iI==6)
	{
		float xyPeriod = 12.0; //number of rings
		float turbPower = 0.1; //makes twists
		float turbSize = 32.0; //initial size of the turbulence
		float xValue = (In.texCoord.x - noiseWidth / 2) / float(noiseWidth);
    		float yValue = (In.texCoord.y - noiseHeight / 2) / float(noiseHeight);
    		float distValue = sqrt(xValue * xValue + yValue * yValue) + turbPower * turbulence2(In.texCoord, turbSize) / 256.0;
    		float sineValue = 128.0 * fabs(sin(2 * xyPeriod * distValue * 3.14159));
    		rgb.r = Uint8(80 + sineValue);
    		rgb.g = Uint8(30 + sineValue);
    		rgb.b = 30;
	}
	else if(iI==7)
	{
		float h=128.0;
		float w=128.0;
		float xyPeriod = 12.0; //???
		float xValue = (In.texCoord.x - noiseWidth / 2) / float(noiseWidth) + turbPower * turbulence2(In.texCoord, turbSize) / 256.0;
		float yValue = (In.texCoord.y - noiseHeight / 2) / float(noiseHeight) + turbPower * turbulence(float2(h - In.texCoord.y, w - In.texCoord.x) /* , turbSize */ ) / 256.0;
		float sineValue = 22.0 * fabs(sin(xyPeriod * xValue * 3.1415) + sin(xyPeriod * yValue * 3.1415));
		rgb.rgb = HSVtoRGB(float3(Uint8(sineValue), 255, 255));
	}
	
    rgb = float3(f);

	//rgb=fireball(float3(f));

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

