#define PI 3.1415926538
#define TAU 6.2831853071

// https://iquilezles.org/articles/distfunctions2d/
float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}

void main() {
    vec2 uv = (xlv_TEXCOORD0 - .5 * iResolution.xy) / iResolution.y;
    float pixelSize = 1. / iResolution.y;
    
    uv *= 2.;
    
    vec3 black = vec3(0.17, 0.24, 0.31);
    vec3 accent = vec3(1.0, 0.42, 0.31);
    
    // background
    vec3 color = vec3(1);
    
    // clock border
    color = mix(
        color,
        black,
        smoothstep(1., 1. - pixelSize, length(uv))
    );
    
    // clock face
    color = mix(
        color,
        vec3(1),
        smoothstep(.95, .95 - pixelSize, length(uv))
    );
    
    // minute marks
    // TODO (optimization): space folding instead of iteration
    for (int i = 0; i < 60; i++) {
        float angle = (float(i) / 60.) * TAU + (PI / 2.);
        float width = i % 5 == 0 ? .01 : .005;
        float start = i % 5 == 0 ? .8 : .85;
        vec2 point = vec2(-cos(angle), sin(angle));
        float sdf = sdSegment(uv, point * start, point * .9);
        
        color = mix(
            color,
            vec3(black),
            smoothstep(width, width - pixelSize, sdf)
        );
    }
    
    { // hour hand
        float angle = (iTime / (60. * 60. * 12.)) * TAU + (PI / 2.);
        vec2 point = vec2(-cos(angle), sin(angle));
        float width = .0125;
        float sdf = sdSegment(uv, -point * .1, point * .4);
        
        color = mix(
            color,
            vec3(black),
            smoothstep(width, width - pixelSize, sdf)
        );
    }
    
    { // minute hand
        float angle = (iTime / (60. * 60.)) * TAU + (PI / 2.);
        vec2 point = vec2(-cos(angle), sin(angle));
        float width = .01;
        float sdf = sdSegment(uv, -point * .1, point * .7);
        
        color = mix(
            color,
            vec3(black),
            smoothstep(width, width - pixelSize, sdf)
        );
    }
    
    { // second hand
        float angle = (iTime / 60.) * TAU + (PI / 2.);
        vec2 point = vec2(-cos(angle), sin(angle));
        float width = .005;
        
        float sdf = sdSegment(uv, -point * .1, point * .9);
        color = mix(
            color,
            vec3(accent),
            smoothstep(width, width - pixelSize, sdf)
        );
        
        sdf = sdSegment(uv, -point * .2, -point * .1);
        color = mix(
            color,
            vec3(accent),
            smoothstep(width * 2., width * 2. - pixelSize, sdf)
        );
    }
    
    // clock hand cap
    color = mix(
        color,
        black,
        smoothstep(.03, .03 - pixelSize, length(uv))
    );
    
    gl_FragColor = vec4(color, 1);
}

