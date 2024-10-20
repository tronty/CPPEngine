// o1-preview: Voisitko luoda Venuksen fragmenttishaderin (GLSL) turbulenssilla ilman uniform shader muuttujia?

#ifdef GL_ES
precision mediump float;
#endif

// Satunnaisluku-funktio UV-koordinaateille
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Lineaarinen interpolaatio
float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Perlin noise -tyyppinen funktio
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Fraktaalinen Brownian Motion (FBM) kohinan luomiseksi
float fbm(vec2 st) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 0.0;

    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(st);
        st *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

// Venuksen pintatekstuurin ja turbulenssin luominen
vec3 generateVenusTexture(vec2 uv) {
    // Lisää turbulenssia Fractal Brownian Motionilla
    float turbulence = fbm(uv * 5.0 + time * 0.1);

    // Venuksen pintaväriä
    vec3 baseColor = vec3(0.9, 0.7, 0.3); // Venuksen kellertävä sävy

    // Pehmeitä pilvimuodostelmia ja pyörteitä
    vec3 color = baseColor * (0.8 + 0.3 * turbulence);

    return color;
}

void main() {
    // Normalisoidut UV-koordinaatit [-1, 1] alueelle
    vec2 uv = xlv_TEXCOORD0.xy / resolution.xy * 2.0 - 1.0;

    // Venuksen ympyrä
    float radius = 0.8;
    float dist = length(uv);
    if (dist > radius) {
        discard;  // Vain ympyränmuotoinen alue näkyy
    }

    // Pyörimisliike (Venuksen hidas pyöriminen)
    float rotationSpeed = 0.05;  // Venuksen pyörimisnopeus on hitaampi
    float angle = time * rotationSpeed;
    uv = mat2(cos(angle), -sin(angle), sin(angle), cos(angle)) * uv;

    // Venuksen pinnan värit ja tekstuurit
    vec3 color = generateVenusTexture(uv);

    // Vignette-efekti (tummennetaan reunaa)
    float vignette = smoothstep(radius, radius - 0.1, dist);
    color *= vignette;

    // Lopputulos
    gl_FragColor = vec4(color, 1.0);
}
