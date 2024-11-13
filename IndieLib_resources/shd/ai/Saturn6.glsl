// Saturnuksen ja renkaiden värit
const vec3 planetColor = vec3(1.0, 0.8, 0.5);
const vec3 ringColor = vec3(0.8, 0.7, 0.6);

void main() {
    // Normalisoidaan koordinaatit
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    // Saturnuksen keskipiste
    vec2 center = vec2(0.0, 0.0);

    // Lasketaan etäisyys keskipisteestä
    float dist = length(uv - center);

    // Planetaarisen säteen säätö
    float planetRadius = 0.4;

    // Rengasalueen määrittely
    float innerRing = 0.45;
    float outerRing = 0.7;

    vec3 color = vec3(0.0); // Alustetaan mustaksi

    // Saturnuksen planeetan renderöinti
    if (dist < planetRadius) {
        color = planetColor;
    }

    // Saturnuksen renkaat
    if (dist > innerRing && dist < outerRing) {
        float ringPattern = sin(30.0 * dist - time * 2.0);
        float ringAlpha = smoothstep(0.1, 0.2, ringPattern); // Pehmennetään reunat
        color = mix(color, ringColor, ringAlpha);
    }

    // Lopputulos
    gl_FragColor = vec4(color, 1.0);
}
