#define N 40
#define PRECISION .001

float rand(vec2 p) {
    return fract(sin(dot(p, vec2(1.234, 56.78))) * 87654.3);
}

vec2 map(vec3 p) {
    vec2 ip = floor(p.xz);
    vec2 fp = p.xz - ip;
    float d = (rand(ip) < .5 ? abs(fp.x + fp.y - 1.) : -abs(fp.x - fp.y)) * .707;
    vec2 cp = vec2(.5) - abs(fp - vec2(.5));
    float d2 = max(max(cp.x, cp.y) * .7, min(cp.x, cp.y) * 1.4);
    d = d < 0. ? max(-d2, d) : min(d2, d);
    if (abs(d) < p.y) {
        return vec2(abs(d), p.y > .95 ? 2. : (d > 0. ? 0. : 1.));
    } else {
        return vec2(p.y, 2.);
    }
}

vec3 scene(vec2 coord) {
    vec2 uv = (2. * coord - iResolution.xy) / min(iResolution.x, iResolution.y);
    vec3 o = vec3(iTime, 3., -iTime);
    vec3 d = normalize(vec3(uv.x, -1. + uv.y * .707, -2. - uv.y * .707));
    float t = .2 * iTime;
    float ct = cos(t), st = sin(t);
    d = vec3(d.x * ct - d.z * st, d.y, d.x * st + d.z * ct);
    vec3 col = vec3(0.);
    if (d.y < 0.) {
        float l = (o.y - 1.) / -d.y;
        vec3 p = o + l * d;
        vec2 m;
        for (int i = 0; i < N; ++i) {
            m = map(p);
            if (m.x < PRECISION) {
                break;
            }
            l += m.x;
            p = o + l * d;
        }
        if (m.y < .5) {
            col = vec3(0.7529, 0.2157, 0.3333);
        } else if (m.y < 1.5) {
            col = vec3(0.);
        } else if (m.y < 2.5) {
            col = vec3(0.6353, 0.5686, 0.6588);
        }
    }
    return col;
}

void main() {
    // simple AA
    vec3 col = .25 * (scene(xlv_TEXCOORD0.xy + vec2(.1, .4)) + scene(xlv_TEXCOORD0.xy + vec2(-.4, .1)) + scene(xlv_TEXCOORD0.xy + vec2(-.1, -.4)) + scene(xlv_TEXCOORD0.xy + vec2(.4, -.1)));
    gl_FragColor = vec4(col, 1.0);
}

