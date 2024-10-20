void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
    uv = uv * 2.0 - 1.0;
    uv.x *= 800.0 / 600.0;

    float radius = length(uv);
    float craters = 0.0;

    // Generoi kraaterit yksinkertaisella summaisella funktiolla
    for (int i = 1; i < 10; i++) {
        vec2 offset = vec2(sin(float(i) * 1.23), cos(float(i) * 2.34)) * 0.1;
        float dist = length(uv - offset);
        craters += smoothstep(0.02, 0.04, 0.05 - dist);
    }

    // Määritä kuu
    float moon = smoothstep(0.8, 0.9, 1.0 - radius) + craters;

    // Väritä kuu ja tausta
    vec3 color = vec3(0.0);
    if (radius < 1.0) {
        color = vec3(0.9, 0.9, 0.7) * moon;
    }

    gl_FragColor = vec4(color, 1.0);
}
