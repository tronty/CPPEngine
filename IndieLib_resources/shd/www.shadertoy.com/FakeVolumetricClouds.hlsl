/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
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

[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
// https://www.shadertoy.com/view/XlsXzN
// FakeVolumetricClouds
float mod(float x, float y)
{
  return x - y * floor(x / y);
}


float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
sampler2D iChannel0;
sampler2D iChannel1;
/*
Copyright (c) 2015 Kari Kuvaja

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// maybe something between 0.5 and 3.0
const float CLOUD_HEIGHT = 2.0;

// scale of clouds
const float UV_FREQ = 0.005;

// cloudiness, bigger number = less clouds
const float CLOUD_FILTER = 0.4;

// parallax layers
const int PARALLAX_LAYERS = 8;

float flter(float f, float a)
{
   f = clamp(f - a, 0.0, 1.0);
   f /= (1.0 - a);    
   return f;
}

float fbm(float2 uv)
{
    float f = (tex2D(iChannel0, uv * 2.0).r - 0.5) * 0.2;
    f += (tex2D(iChannel0, uv * 4.0).r - 0.5) * 0.125;
    f += (tex2D(iChannel0, uv * 8.0).r - 0.5) * 0.125 * 0.5;
    f += (tex2D(iChannel0, uv * 16.0).r - 0.5) * 0.125 * 0.25;
    f += (tex2D(iChannel0, uv * 32.0).r - 0.5) * 0.125 * 0.24;
    f += (tex2D(iChannel0, uv * 64.0).r - 0.5) * 0.125 * 0.22;
    f += (tex2D(iChannel0, uv * 128.0).r - 0.5) * 0.125 * 0.12;
    f += (tex2D(iChannel0, uv * 256.0).r - 0.5) * 0.125 * 0.1;
    f += 0.5;
    return clamp(f, 0.0, 1.0);
}



float2 getuv(in float2 uv, float l)
{
    float3 rd = normalize(float3(uv, 0.4));
    float2 _uv = float2(rd.x / abs(rd.y) * l, rd.z / abs(rd.y) * l);
    return _uv;
}

// cloud rendering
void clouds (float2 uv, inout float4 col, float t, float freq)
{
    float2 _uv = getuv(uv, 1.0);
    _uv.y += t;
    float l = 1.0;
    
    float2 mouse = (iMouse.xy - iResolution.xy * 0.5) / iResolution.xy;
    
    for (int i = 0; i < PARALLAX_LAYERS; ++i)
    {
        // 3 parallax layers of clouds
        float h = fbm(_uv * freq) * 0.5;
        h += fbm(float2(-t * 0.001, t * 0.0015) + _uv * freq * 1.1) * 0.35;
        h += fbm(float2(t * 0.001, -t * 0.0025) + _uv * freq * 1.2) * 0.15;
        
        float f = flter(h, CLOUD_FILTER + mouse.x * 0.1);
        f -= (l - 1.0) * CLOUD_HEIGHT; // height
        
        f = clamp(f, 0.0, 1.0);
        
        col += f * float4(0.9, 0.9, 1.0, 1.0) * (1.0 - col.a);
        
        
        l *= 1.09 - h * (0.18 * (1.0 + (mouse.y + 0.5) * 0.2) ); // parallax control, offset uv by fbm density
       
        _uv = getuv(uv, l);
    	_uv.y += t;
    }
}

// fbm for reflections
float wfbm(float2 uv)
{
    float f = (tex2D(iChannel1, uv * 1.0).r - 0.5) * 0.5;
    f += (tex2D(iChannel1, uv * 2.0).r - 0.5) * 0.5 * 0.5;
    f += (tex2D(iChannel1, uv * 4.0).r - 0.5) * 0.25 * 0.5;
    f += (tex2D(iChannel1, uv * 8.0).r - 0.5) * 0.25 * 0.5 * 0.5;
    f += (tex2D(iChannel1, uv * 16.0).r - 0.5) * 0.25 * 0.5 * 0.5 * 0.5;
    return f + 0.5;
    
}

float3 grad(float2 uv)
{
	float2 off = float2(0.15, 0.0);
    float3 g = float3(wfbm(uv + off.xy) - wfbm(uv - off.xy),
                  off.x,
				  wfbm(uv + off.yx) - wfbm(uv - off.yx));
    
    return normalize(g);
}




float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    fragColor = float4(0, 0, 0, 0);
    
	float2 uv = fragCoord.xy / iResolution.xy;
    uv -= float2(0.5, 0.5);
    uv.y /= iResolution.x / iResolution.y;
    
    float4 dark = float4(0.1, 0.2, 0.3, 0.0) * 1.5;
    float4 light = float4(0.3, 0.4, .55, 0.0) * 1.5;
    float4 bg = mix(light, dark, abs(uv.y) * 6.5);
    float4 col = float4(0, 0, 0, 0);
    
    float2 _uv = uv;
    _uv.y -= iTime * 0.01;
    _uv.x *= 0.1;
    float2 guv = float2(0, 0);
    
    if (uv.y < 0.0)
    {
	    float3 g = grad(_uv * 5.0);
	    guv = float2(g.x / g.y, g.z / g.y);
    }
    
    clouds(uv + guv * 0.015 * mix(-0.0, 1.0, clamp(abs(uv.y) * 5.0 - 0.04, 0.0, 1.0)  ), col, iTime * 0.4, UV_FREQ);
   
    fragColor = mix(bg, col, col.a);

    // some graphical candy, sun halos etc.
    if(uv.y < 0.0)
    {
        _uv = uv;
    	fragColor = mix(dark, fragColor, (1.0 - smoothstep(-0.05, -0., uv.y) * 0.75));
        
        _uv.x *= 0.1;
        fragColor = mix(fragColor, float4(1, 1, 1, 1), 1.0 - smoothstep(0.0, 0.025, length(_uv)));

        _uv.y *= 0.05;
        _uv.x *= 0.35;
        fragColor = mix(fragColor, float4(1, 1, 1, 1), 1.0 - smoothstep(0.0, 0.005, length(_uv)));
        
    }else
    {
	    float4 wcolor = light * 1.3;
    	fragColor = mix(wcolor, fragColor, (smoothstep(0., .1, uv.y)));        
		float2 _uv = uv;	
        _uv.x *= 0.1;
        fragColor = mix(fragColor, float4(1, 1, 1, 1), 1.0 - smoothstep(0.0, 0.1, length(_uv)));
    }
    
    uv.x *= 0.015;
    fragColor = mix(fragColor, float4(1, 1, 1, 1), 1.0 - smoothstep(0., 0.01, length(uv)));
    fragColor = mix(fragColor, float4(1, 1, 1, 1), 1.0 - smoothstep(0., 0.005, length(uv)));
    
    // contrast
    float contr = 0.6;
    fragColor = mix(float4(0, 0, 0, 0), float4(1, 1, 1, 1), fragColor * contr + (1.0 - contr) * fragColor * fragColor * (3.0 - 2.0 * fragColor));
	return fragColor;
}

