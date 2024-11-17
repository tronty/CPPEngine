
precision highp float;

//const vec2 resolution = vec2(800.0, 800.0); // Käsin määritelty resoluutio
const float PI = 3.1415926535897932384626433832795;

// Noise-funktio, joka käyttää sin-kos-menetelmää tuottamaan pseudo-satunnaisia tuloksia
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

// Smooth noise lisäämään pehmeämpää siirtymää
float smoothNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = noise(i);
    float b = noise(i + vec2(1.0, 0.0));
    float c = noise(i + vec2(0.0, 1.0));
    float d = noise(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Fraktaalinen Brownian Motion (FBM) lisäämään monimutkaisuutta kohinaan
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; i++) {
        value += amplitude * smoothNoise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

// Pyörimisefekti
vec2 rotate(vec2 p, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return vec2(p.x * c - p.y * s, p.x * s + p.y * c);
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy - 0.5; // Keskitetään koordinaatit
    uv *= 2.0;

    // Etäisyys keskipisteestä pallon muotoon
    float dist = length(uv);

    // Tarkista, että ollaan pallon alueella
    if (dist > 1.0) discard;

    // Pyörivä tekstuuri
    float time = mod(gl_FragCoord.x * 0.001 + gl_FragCoord.y * 0.001, PI * 2.0);
    vec2 rotatedUV = rotate(uv, time);

    // Pilkut ja flare: FBM-kohinaa eri asteikoilla
    float sunspots = 1.0 - smoothstep(0.5, 0.7, fbm(rotatedUV * 6.0));
    float flares = smoothstep(0.6, 1.0, fbm(rotatedUV * 8.0));

    // Väri: auringon oranssihehku
    vec3 color = vec3(1.0, 0.7, 0.2) * (1.0 - dist) * 1.5; // Vahvempi keskellä

    // Lisää kohina- ja flare-kerrokset
    //color *= sunspots * flares;

    // Sytytä reuna hehkumaan
    color += vec3(1.0, 0.4, 0.0) * (1.0 - smoothstep(0.8, 1.0, dist));

    gl_FragColor = vec4(color, 1.0);
}


