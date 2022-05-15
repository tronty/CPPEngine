// crater function

const float radius = .3;
const float bottom = .6;

float bowl(float r)
{
    // y = ax² + c
    const float c = -bottom;
    const float a = bottom / (radius * radius);
    
    float y = a * r * r + c;
    return clamp(y, -1., 0.);
}

float ridge(float r)
{
    return smoothstep(.0, .7, 1. * (.5 - clamp(r, radius, .5)));
}

// uv in [-1, 1]²
// result in [-1, 1]
float height(vec2 uv)
{
	// custom interpolation to avoid discontinuities
    int n = 43;
    float a = float(n) * (atan(uv.y, uv.x) + radians(180.)) / radians(360.);
    ivec2 size = textureSize(iChannel0, 0);
    ivec2 mid = size / 2;
    float noise = .14 * (mix(texelFetch(iChannel0, mid + ivec2(int(round(a - .5)), 0), 0).x,
                            texelFetch(iChannel0, mid + ivec2((int(round(a + .5))) % n, 0), 0).x,
                            smoothstep(-.3, 1.3, fract(a))) - .5) * clamp(length(uv), 0., .5);

    float r = length(uv) + noise;
    return .05 * texture(iChannel1, 1.3 * uv).r + min(
    	bowl(r) + ridge(r),
        .13 + .14 * texture(iChannel2, .5 + 2. * uv).r);
}

// https://iquilezles.org/articles/normalsSDF
vec3 normal(vec2 uv)
{
    const float eps = 0.001; // or some other value
    const vec2 h = vec2(eps,0);
    return normalize( vec3( height(uv-h.xy) - height(uv+h.xy),
                            2.0*h.x,
                            height(uv-h.yx) - height(uv+h.yx) ) );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    const float scale = 1.2;
    vec2 uv = scale * (fragCoord - .5 * iResolution.xy) / iResolution.y;

#if 0
    vec3 col = vec3(.5 + .5 * height(uv));
#else
    float t = radians(20.) * iTime;
    vec3 col = vec3(.2, 0, 0)
        + vec3(1) * max(0., dot(normal(uv), normalize(vec3(cos(t), sin(t), .4))))
        + vec3(0) * height(uv);
#endif

#if 0 // overlay slice graphs
    col = mix(col, vec3(1., 0., 0.), smoothstep(2. * scale / iResolution.y, 0., abs(uv.y - .5 * bowl(abs(uv.x)))));
    col = mix(col, vec3(0., 0., 1.), smoothstep(2. * scale / iResolution.y, 0., abs(uv.y - .5 * ridge(abs(uv.x)))));
    col = mix(col, vec3(1., 1., 1.), smoothstep(2. * scale / iResolution.y, 0., abs(uv.y - .5 * height(vec2(uv.x, 0.)))));
    col = mix(col, vec3(1., 1., 1.), smoothstep(2. * scale / iResolution.y, 0., abs(length(uv) - .5)));
#endif

    fragColor = vec4(col, 1.);
}

