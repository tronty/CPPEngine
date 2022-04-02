uniform sampler2D iChannel0;

//#define COLORFUL

const int ITERATIONS = 30;
const float STEP = 0.02;
const float DITHER = 0.2;
const float DURATION = 10.0;
const float PI = 3.14159265;

float noise(vec3 x) // iq's 3D noise
{
    vec3 f = fract(x);
    vec3 p = x - f;
    f = f*f*(3.0 - 2.0*f);
    vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
    vec2 rg = texture2D(iChannel0, (uv + 0.5)/256.0, -100.0).rg;
    return mix(rg.y, rg.x, f.z);
}

float fbm(vec3 x)
{
    float r = 0.0;
    float w = 1.0, s = 1.0;
    for (int i=0; i<5; i++)
    {
        w *= 0.5;
        s *= 2.0;
        r += w * noise(s * x);
    }
    return r;
}

float sphere(vec3 p, float r)
{
    return length(p) - r;
}

vec4 scene(vec3 pos, vec3 dir, float t, out float dist)
{
    float rad1 = 0.4, rad2 = 0.35;
    float d = sphere(pos, rad1);
    d -= rad2 * fbm(pos * mix(1.5, 3.5, sin(t/DURATION*2.*PI)*.5+.5) + iTime * 0.1);
    dist = d;
    vec4 color;
    float r = length(pos) / rad1;
    color.a = smoothstep(1., 0., clamp(d / .05, 0., 1.));
    color.a *= smoothstep(1., 0., clamp((r - 1.25) * 2., 0., 1.));
    color.rgb = 1.2 * mix(vec3(1.0,0.95,0.2), vec3(1.0,0.35,0.), clamp((r - 1.1)*3., 0., 1.));
    color.rgb = mix(color.rgb, vec3(0.95,0.95,1.0)*0.25, clamp((r - 1.3)*3., 0., 1.));
    return color;
}

vec3 render(vec3 eye, vec3 dir, vec2 xlv_TEXCOORD0, float t)
{
    vec4 color = vec4(0.0);
    
    vec3 pos = eye + dir * DITHER * texture2D(iChannel0, xlv_TEXCOORD0.xy/iChannelResolution[0].x).r;
    
    for (int i=0; i<ITERATIONS; i++)
    {
        if (color.a > 0.99) continue;
        
        float dist;
        vec4 d = scene(pos, dir, t, dist);
        d.rgb *= d.a;
        color += d * (1.0 - color.a);
     
        pos += dir * max(STEP, dist * 0.5);
    }
    
    color.rgb += vec3(0.1 * vec3(1.0,0.5,0.1)) * (1.0 - color.a);
    
    return clamp(color.rgb, 0.0, 1.0);
}

vec3 lookAtDir(vec2 uv, vec3 eye, vec3 at, vec3 up, float fov)
{
    vec3 f = normalize(at - eye);
    vec3 r = normalize(cross(up, f));
    vec3 u = normalize(cross(f, r));
    return normalize(f + fov * (uv.x*r + uv.y*u));
}

void main( )
{
    vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    
    float angle = iTime * 0.3;
    if (iMouse.z > 0.) angle = iMouse.x / iResolution.x * 2. * PI;
    vec3 eye = vec3(sin(angle), sin(angle * 4.5)*0.1, cos(angle));
    vec3 dir = lookAtDir(uv, eye, vec3(0, 0, 0), vec3(0, 1, 0), 1.0);
    
    float time = mod(iTime, DURATION);
	
    vec3 color = render(eye, dir, xlv_TEXCOORD0, time);
    
#ifdef COLORFUL
    color = mix(color.rgb, mix(color.bgr, color.brg, sin(iTime * 0.3)*.5+.5), sin(iTime * 0.5)*.5+.5);
#endif
    
    gl_FragColor = vec4(color, 1.0);
}
