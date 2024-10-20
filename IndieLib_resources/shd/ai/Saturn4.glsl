const float PI = 3.14159265359;

// Funktio perus noise-tehosteen luomiseen
float noise(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float smoothNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(noise(i + vec2(0.0, 0.0)), noise(i + vec2(1.0, 0.0)), u.x),
               mix(noise(i + vec2(0.0, 1.0)), noise(i + vec2(1.0, 1.0)), u.x), u.y);
}

vec3 rings(vec2 uv, float radius) {
    float ringPattern = sin(100.0 * uv.x * uv.x + 20.0 * smoothNoise(uv * 10.0)) * 0.5 + 0.5;
    ringPattern *= smoothstep(radius, radius + 0.005, length(uv) - 0.3);
    vec3 ringColor = vec3(0.8, 0.7, 0.5);
    return ringColor * ringPattern;
}

void main() {
    vec2 uv = vec2(xlv_TEXCOORD0);
    vec3 color = vec3(0);
    float radius=1.0;

    // Add rings
    color += rings(uv, radius);

    gl_FragColor = vec4(color, 1.0);
}

