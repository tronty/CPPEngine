/**
 * Created by Steven Sell (ssell) / 2017
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 *
 *  - Sources -
 *
 * Hash function by "Dave_Hoskins" (David Hoskins) https://www.shadertoy.com/view/4djSRW
 * Raw simplex by "candycat" https://www.shadertoy.com/view/4sc3z2
 * UI template by "XT95" (Anatole Duprat) https://www.shadertoy.com/view/ldKSDm
 *
 *  - Control Variables -
 *
 * - Octaves: For each octave, a higher frequency/lower amplitude function 
 * will be added to the original. Additional octaves increase the definition 
 * of the resulting noise but may be computationally expensive.
 *
 *   Slider scale [0.0, 1.0] -> [1.0, 10.0]
 *
 * - Persistence: The higher the persistence, the more of each succeeding 
 * octave will be added to the original. Very high persistence values can lead 
 * to output resembling raw (non-coherent) noise.
 *
 *   Slider scale [0.0, 1.0] -> [0.0, 1.0]
 *
 * - Scale: Scale can be thought of as a zoom-level. The lower the scale, the 
 * closer in the zoom and vice versa. Extremely low values can result in a blobby 
 * or solid output. Extremely high values can result in output resembling raw noise.
 *
 *   Slider scale [0.0, 1.0] -> [0.001, 0.1]
 */

#define MIX(a, b, r) mix(r, color, step(a, b))

float Octaves     = 0.0;
float Scale       = 0.0;
float Persistence = 0.0;
float Speed       = 0.0;

//--------------------------------------------------------------------
// Noise Functions
//--------------------------------------------------------------------

// Modified hash33 by Dave_Hoskins (original does not play well with simplex)
// Original Source: https://www.shadertoy.com/view/4djSRW
vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * vec3(0.1031, 0.11369, 0.13787));
    p3 += dot(p3, p3.yxz + 19.19);
    return -1.0 + 2.0 * fract(vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}

// Raw simplex implementation by candycat
// Source: https://www.shadertoy.com/view/4sc3z2
float SimplexNoiseRaw(vec3 pos)
{
    const float K1 = 0.333333333;
    const float K2 = 0.166666667;
    
    vec3 i = floor(pos + (pos.x + pos.y + pos.z) * K1);
    vec3 d0 = pos - (i - (i.x + i.y + i.z) * K2);
    
    vec3 e = step(vec3(0.0), d0 - d0.yzx);
	vec3 i1 = e * (1.0 - e.zxy);
	vec3 i2 = 1.0 - e.zxy * (1.0 - e);
    
    vec3 d1 = d0 - (i1 - 1.0 * K2);
    vec3 d2 = d0 - (i2 - 2.0 * K2);
    vec3 d3 = d0 - (1.0 - 3.0 * K2);
    
    vec4 h = max(0.6 - vec4(dot(d0, d0), dot(d1, d1), dot(d2, d2), dot(d3, d3)), 0.0);
    vec4 n = h * h * h * h * vec4(dot(d0, hash33(i)), dot(d1, hash33(i + i1)), dot(d2, hash33(i + i2)), dot(d3, hash33(i + 1.0)));
    
    return dot(vec4(31.316), n);
}

float SimplexNoise(
    vec3  pos,
    float octaves,
    float scale,
    float persistence)
{
    float final        = 0.0;
    float amplitude    = 1.0;
    float maxAmplitude = 0.0;
    
    for(float i = 0.0; i < octaves; ++i)
    {
        final        += SimplexNoiseRaw(pos * scale) * amplitude;
        scale        *= 2.0;
        maxAmplitude += amplitude;
        amplitude    *= persistence;
    }
    
    return (final / maxAmplitude);
}

vec3 Color(float h)
{
    vec3 color = vec3(0.10980, 0.31764, 0.79215);            // Dark water
    
    color = MIX(h, 0.375, vec3(0.29803, 0.61568, 0.92156));  // Shallow water
    color = MIX(h, 0.45,  vec3(0.992157, 1.0, 0.541176));    // Sand
    color = MIX(h, 0.46,  vec3(0.37647, 0.90980, 0.34509));  // Grass
    color = MIX(h, 0.5,   vec3(0.37647, 0.78431, 0.34509));  // Forests
    color = MIX(h, 0.65,  vec3(1.0, 1.0, 1.0));              // Peaks
    
    return color;
}
#if 0
//--------------------------------------------------------------------
// UI Functions
//--------------------------------------------------------------------

float uiSlider(int id)
{
    return texture(iChannel0, vec2(float(id)+.5,0.5)/iResolution.xy).r;
}

vec4 HandleSliders(in vec2 uv)
{
    vec4 sliderVal = texture(iChannel0, uv);
	
    Octaves     = 1.0 + uiSlider(0) * 9.0;
    Scale       = 0.0001 + uiSlider(1) * 0.1;
    Persistence = uiSlider(2) * 1.0;
    Speed       = uiSlider(3) * 50.0;
    
    return sliderVal;
}
#endif
//--------------------------------------------------------------------
// Main
//--------------------------------------------------------------------

void main( )
{
	vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
    vec4 sliders = vec4(0.5);//HandleSliders(uv);
    
    float noise = SimplexNoise(
        vec3(xlv_TEXCOORD0.x, xlv_TEXCOORD0.y, iTime * Speed), 
        Octaves,
        Scale, 
        Persistence);
    
    noise = (noise + 1.0) * 0.5;  // Transform noise from [-1, 1] to [0, 1]
    
    if(uv.x < 0.5)
    {
        gl_FragColor.rgb = vec3(noise);
    }
    else
    {
        gl_FragColor.rgb = Color(noise);
    }
    
    gl_FragColor.rgb = mix(gl_FragColor.rgb, sliders.rgb, sliders.a);
}
