uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

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

float fbm(vec2 uv)
{
    float f = (texture(iChannel0, uv * 2.0).r - 0.5) * 0.2;
    f += (texture(iChannel0, uv * 4.0).r - 0.5) * 0.125;
    f += (texture(iChannel0, uv * 8.0).r - 0.5) * 0.125 * 0.5;
    f += (texture(iChannel0, uv * 16.0).r - 0.5) * 0.125 * 0.25;
    f += (texture(iChannel0, uv * 32.0).r - 0.5) * 0.125 * 0.24;
    f += (texture(iChannel0, uv * 64.0).r - 0.5) * 0.125 * 0.22;
    f += (texture(iChannel0, uv * 128.0).r - 0.5) * 0.125 * 0.12;
    f += (texture(iChannel0, uv * 256.0).r - 0.5) * 0.125 * 0.1;
    f += 0.5;
    return clamp(f, 0.0, 1.0);
}



vec2 getuv(in vec2 uv, float l)
{
    vec3 rd = normalize(vec3(uv, 0.4));
    vec2 _uv = vec2(rd.x / abs(rd.y) * l, rd.z / abs(rd.y) * l);
    return _uv;
}

// cloud rendering
void clouds (vec2 uv, inout vec4 col, float t, float freq)
{
    vec2 _uv = getuv(uv, 1.0);
    _uv.y += t;
    float l = 1.0;
    
    vec2 mouse = (iMouse.xy - iResolution.xy * 0.5) / iResolution.xy;
    
    for (int i = 0; i < PARALLAX_LAYERS; ++i)
    {
        // 3 parallax layers of clouds
        float h = fbm(_uv * freq) * 0.5;
        h += fbm(vec2(-t * 0.001, t * 0.0015) + _uv * freq * 1.1) * 0.35;
        h += fbm(vec2(t * 0.001, -t * 0.0025) + _uv * freq * 1.2) * 0.15;
        
        float f = flter(h, CLOUD_FILTER + mouse.x * 0.1);
        f -= (l - 1.0) * CLOUD_HEIGHT; // height
        
        f = clamp(f, 0.0, 1.0);
        
        col += f * vec4(0.9, 0.9, 1.0, 1.0) * (1.0 - col.a);
        
        
        l *= 1.09 - h * (0.18 * (1.0 + (mouse.y + 0.5) * 0.2) ); // parallax control, offset uv by fbm density
       
        _uv = getuv(uv, l);
    	_uv.y += t;
    }
}

// fbm for reflections
float wfbm(vec2 uv)
{
    float f = (texture(iChannel1, uv * 1.0).r - 0.5) * 0.5;
    f += (texture(iChannel1, uv * 2.0).r - 0.5) * 0.5 * 0.5;
    f += (texture(iChannel1, uv * 4.0).r - 0.5) * 0.25 * 0.5;
    f += (texture(iChannel1, uv * 8.0).r - 0.5) * 0.25 * 0.5 * 0.5;
    f += (texture(iChannel1, uv * 16.0).r - 0.5) * 0.25 * 0.5 * 0.5 * 0.5;
    return f + 0.5;
    
}

vec3 grad(vec2 uv)
{
	vec2 off = vec2(0.15, 0.0);
    vec3 g = vec3(wfbm(uv + off.xy) - wfbm(uv - off.xy),
                  off.x,
				  wfbm(uv + off.yx) - wfbm(uv - off.yx));
    
    return normalize(g);
}


void main( )
{
    gl_FragColor = vec4(0.0);
    
	vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
    uv -= vec2(0.5);
    uv.y /= iResolution.x / iResolution.y;
    
    vec4 dark = vec4(0.1, 0.2, 0.3, 0.0) * 1.5;
    vec4 light = vec4(0.3, 0.4, .55, 0.0) * 1.5;
    vec4 bg = mix(light, dark, abs(uv.y) * 6.5);
    vec4 col = vec4(0);
    
    vec2 _uv = uv;
    _uv.y -= iTime * 0.01;
    _uv.x *= 0.1;
    vec2 guv = vec2(0.0);
    
    if (uv.y < 0.0)
    {
	    vec3 g = grad(_uv * 5.0);
	    guv = vec2(g.x / g.y, g.z / g.y);
    }
    
    clouds(uv + guv * 0.015 * mix(-0.0, 1.0, clamp(abs(uv.y) * 5.0 - 0.04, 0.0, 1.0)  ), col, iTime * 0.4, UV_FREQ);
   
    gl_FragColor = mix(bg, col, col.a);

    // some graphical candy, sun halos etc.
    if(uv.y < 0.0)
    {
        _uv = uv;
    	gl_FragColor = mix(dark, gl_FragColor, (1.0 - smoothstep(-0.05, -0., uv.y) * 0.75));
        
        _uv.x *= 0.1;
        gl_FragColor = mix(gl_FragColor, vec4(1.0), 1.0 - smoothstep(0.0, 0.025, length(_uv)));

        _uv.y *= 0.05;
        _uv.x *= 0.35;
        gl_FragColor = mix(gl_FragColor, vec4(1.0), 1.0 - smoothstep(0.0, 0.005, length(_uv)));
        
    }else
    {
	    vec4 wcolor = light * 1.3;
    	gl_FragColor = mix(wcolor, gl_FragColor, (smoothstep(0., .1, uv.y)));        
		vec2 _uv = uv;	
        _uv.x *= 0.1;
        gl_FragColor = mix(gl_FragColor, vec4(1.0), 1.0 - smoothstep(0.0, 0.1, length(_uv)));
    }
    
    uv.x *= 0.015;
    gl_FragColor = mix(gl_FragColor, vec4(1.0), 1.0 - smoothstep(0., 0.01, length(uv)));
    gl_FragColor = mix(gl_FragColor, vec4(1.0), 1.0 - smoothstep(0., 0.005, length(uv)));
    
    // contrast
    float contr = 0.6;
    gl_FragColor = mix(vec4(0.0), vec4(1.0), gl_FragColor * contr + (1.0 - contr) * gl_FragColor * gl_FragColor * (3.0 - 2.0 * gl_FragColor));
}
