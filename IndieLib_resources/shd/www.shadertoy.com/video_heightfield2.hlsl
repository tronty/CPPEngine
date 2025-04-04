struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

// The MIT License
// Copyright � 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// https://www.youtube.com/c/InigoQuilez
// https://iquilezles.org

// Simplex Noise (http://en.wikipedia.org/wiki/Simplex_noise), a type of gradient noise
// that uses N+1 vertices for random gradient interpolation instead of 2^N as in regular
// latice based Gradient Noise.

// Value    Noise 2D, Derivatives: https://www.shadertoy.com/view/4dXBRH
// Gradient Noise 2D, Derivatives: https://www.shadertoy.com/view/XdXBRH
// Value    Noise 3D, Derivatives: https://www.shadertoy.com/view/XsXfRH
// Gradient Noise 3D, Derivatives: https://www.shadertoy.com/view/4dffRH
// Value    Noise 2D             : https://www.shadertoy.com/view/lsf3WH
// Value    Noise 3D             : https://www.shadertoy.com/view/4sfGzS
// Gradient Noise 2D             : https://www.shadertoy.com/view/XdXGW8
// Gradient Noise 3D             : https://www.shadertoy.com/view/Xsl3Dl
// Simplex  Noise 2D             : https://www.shadertoy.com/view/Msf3WH
// Wave     Noise 2D             : https://www.shadertoy.com/view/tldSRj

float2 hash( float2 p ) // replace this by something better
{
	p = float2( dot(p,float2(127.1,311.7)), dot(p,float2(269.5,183.3)) );
	return -1.0 + 2.0*frac(sin(p)*43758.5453123);
}

float noise2( in float2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	float2  i = floor( p + (p.x+p.y)*K1 );
    float2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    float2  o = float2(m,1.0-m);
    float2  b = a - o + K2;
	float2  c = a - 1.0 + 2.0*K2;
    float3  h = max( 0.5-float3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	float3  n = h*h*h*h*float3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, float3(70.0, 70.0, 70.0) );
}

// hash based 3d value noise
// function taken from https://www.shadertoy.com/view/XslGRr
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// ported from GLSL to HLSL

float hash( float n )
{
    return frac(sin(n)*43758.5453);
}

float noise3( in float3 x )
{
    // The noise function returns a value in the range -1.0f -> 1.0f

    float3 p = floor(x);
    float3 f = frac(x);

    f       = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;

    return lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
                   lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
               lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
                   lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

float tileableNoise2(float x, float y, float w, float h){
	return (noise2(float2(x,     y))     * (w - x) * (h - y) +
			noise2(float2(x - w, y))     *      x  * (h - y) +
			noise2(float2(x,     y - h)) * (w - x) *      y  +
			noise2(float2(x - w, y - h)) *      x  *      y) / (w * h);
}

#define NOISE_SIZE 128

float2 worldToTex(float2 p)
{
	float2 uv = p.xy*0.5+0.5;
	uv.y = 1.0 - uv.y;
	return uv;
}

float heightField(float2 p)
{
	//return tileableNoise2(p.x * 0.143, p.y * 0.143, 0.143 * NOISE_SIZE, 0.143 * NOISE_SIZE);
	//return sin(p.x*1.0);
	return sin(p.x*1.0)*sin(p.y*1.0);
	//return luminance(iChannel0, worldToTex(float2(p.x, p.z)))*0.5;
}

/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
#if 0
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
#else
	float h=heightField(float2(In.position.x, In.position.y));
	float4 vertex = float4(In.position.x, h, In.position.y, 1.0);
	Out.position = mul(worldViewProj, vertex);
#endif
	//Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	Out.uv = In.uv;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	float h=heightField(float2(In.position.x, In.position.y));
	float4 vertex = float4(In.position.x, h, In.position.y, 1.0);
	Out.position = mul(worldViewProj, vertex);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
// https://www.shadertoy.com/view/Xss3zr
// video heightfield
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)

/*
 * Copyright 2020 Simon Green (@simesgreen)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

int _Steps = 64;
float3 lightDir = float3(0.577, 0.577, 0.577);

bool intersectBox(float3 ro, float3 rd, float3 boxmin, float3 boxmax, out float tnear, out float tfar)
{
	// compute intersection of ray with all six bbox planes
	float3 invR = 1.0 / rd;
	float3 tbot = invR * (boxmin - ro);
	float3 ttop = invR * (boxmax - ro);
	// re-order intersections to find smallest and largest on each axis
	float3 tmin = min (ttop, tbot);
	float3 tmax = max (ttop, tbot);
	// find the largest tmin and the smallest tmax
	float2 t0 = max (tmin.xx, tmin.yz);
	tnear = max (t0.x, t0.y);
	t0 = min (tmax.xx, tmax.yz);
	tfar = min (t0.x, t0.y);
	// check for hit
	bool hit;
	if ((tnear > tfar)) 
		hit = false;
	else
		hit = true;
	return hit;
}

bool traceHeightField(float3 ro, float3 rayStep, out float3 hitPos)
{
	float3 p = ro;
	bool hit = false;
	float pH = 0.0;
	float3 pP = p;
	for(int i=0; i<_Steps; i++) {
		float h = heightField(float2(p.x, p.z));
		if ((p.y < h) && !hit) {
			hit = true;
			//hitPos = p;
			// interpolate based on height
            hitPos = lerp(pP, p, (pH - pP.y) / ((p.y - pP.y) - (h - pH)));
		}
		pH = h;
		pP = p;
		p += rayStep;
	}
	return hit;
}

float3 background(float3 rd)
{
     return lerp(float3(1.0, 1.0, 1.0), float3(0.0, 0.5, 1.0), abs(rd.y));
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;

    float2 pixel = (fragCoord.xy / iResolution.xy)*2.0-1.0;

    // compute ray origin and direction
    float asp = iResolution.x / iResolution.y;
    float3 rd = normalize(float3(asp*pixel.x, pixel.y, -2.0));
    float3 ro = float3(0.0, 0.0, 2.0);

	// intersect with bounding box
    bool hit;	
	const float3 boxMin = float3(-1.0, -0.01, -1.0);
	const float3 boxMax = float3(1.0, 0.5, 1.0);
	float tnear, tfar;
	hit = intersectBox(ro, rd, boxMin, boxMax, tnear, tfar);

	tnear -= 0.0001;
	float3 pnear = ro + rd*tnear;
    float3 pfar = ro + rd*tfar;
	
    float stepSize = length(pfar - pnear) / float(_Steps);
	
    float3 rgb = background(rd);
    if(hit)
    {
    	// intersect with heightfield
		ro = pnear;
		float3 hitPos;
		hit = traceHeightField(ro, rd*stepSize, hitPos);
		//if (hit)
		{
			float2 uv = worldToTex(float2(hitPos.x, hitPos.z));
			//rgb = tex2D(iChannel0, uv).xyz;
			rgb=float3(0,1,0);
		}
     }

    fragColor=float4(rgb, 1.0);
    return fragColor;
}

