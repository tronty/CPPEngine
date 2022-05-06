mat2 rotate(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, -s, s, c);
}

float ring(float uvRadius, float ringRadius) {
    return smoothstep(ringRadius - 0.01, ringRadius, uvRadius) * smoothstep(ringRadius + 0.01, ringRadius, uvRadius);
}

float grid(vec2 uv) {
    if (fract(uv.x * 10.0) < 0.01) return 1.0;
    if (fract(uv.y * 10.0) < 0.01) return 1.0;
    return 0.0;
}

vec2 random2(vec2 st) {
	st = vec2(dot(st, vec2(127.1, 311.7)),
		dot(st, vec2(269.5, 183.3)));
	return -1.0 + 2.0 * fract(sin(st) * 43758.5453123 * 0.7897);
}

float circle(vec2 uv, vec2 pos, float radius) {
    return smoothstep(radius, 0.0, length(uv-pos));
}

float circle2(vec2 uv, vec2 pos, float radius) {
    return radius*0.1 / length(uv-pos);
}

float randomSin(float t) {
    return sin(t*0.2352)*sin(t*1.5843)*sin(t*1.2968)+1.0;
}

void mainImage( )
{
    vec2 uv = (xlv_TEXCOORD0-0.5*iResolution.xy)/iResolution.x;
    float radius = length(uv);
    vec3 col = vec3(0.0);
    
    col += ring(radius, 0.1); 
    col += ring(radius, 0.25);
    col += ring(radius, 0.4);
    
    col += smoothstep(0.01, 0.0, uv.x-uv.y) * smoothstep(-0.01, 0.0, uv.x-uv.y);
    col += smoothstep(0.01, 0.0, -uv.x-uv.y) * smoothstep(-0.01, 0.0, -uv.x-uv.y);
    
    col += grid(uv) * smoothstep(0.4, 0.39, radius);
    
    for (int i = 0; i < 10; i++) {
        vec2 pos = random2(vec2(i, i))*0.25;
        float size = randomSin(iTime/2.0 + random2(vec2(i+200, i+300)).x)*0.05;
        col += circle2(uv, pos, size);
    }
    
    uv *= rotate(iTime);
    
    if (uv.y > 0.0 && uv.x > 0.0) {
        col += vec3(smoothstep(radius / 5.0, 0.0, uv.y));
    }

    if (fract(iTime/10.0) < 0.8) {
        col *= vec3(0.0, 1.0, 0.0);
    }
    else {
        col *= vec3(1.0, 0.0, 0.0);
    }
    gl_FragColor = vec4(col,1.0);
}
