// o1-preview: Voisitko luoda Jupiterin  fragmenttishaderin (GLSL) turbulenssilla ilman uniform shader muuttujia?

#ifdef GL_ES
precision mediump float;
#endif

// Perlin noise -tyylinen funktion osatekijä
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Lineaarinen interpolaatio
float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Smooth noise -toiminto
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

// Turbulenssin luominen käyttämällä fraktaalista kohinaa
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

// Jupiterin vöiden generaattori, johon lisätään turbulenssia
vec3 generateBands(vec2 uv) {
    float bandFactor = sin(uv.y * 10.0);  // Jupiterin pilvivyö
    vec3 color = vec3(1.0, 0.8, 0.6);     // Perusväri

    // Lisää FBM (Fractal Brownian Motion) turbulenssia
    float turbulence = fbm(uv * 5.0 + time * 0.1);
    bandFactor += turbulence * 0.2;

    // Väritetään turbulenssin ja vyömäisyyden mukaan
    if (bandFactor > 0.2) {
        color = vec3(0.8, 0.5, 0.3);  // Tummemmat vyöt
    } else if (bandFactor < -0.2) {
        color = vec3(0.6, 0.3, 0.1);  // Tummemmat alueet
    }

    return color;
}

void main() {
    // Normalisoidaan UV-koordinaatit [-1, 1] alueelle
    vec2 uv = xlv_TEXCOORD0.xy / resolution.xy * 2.0 - 1.0;

    // Jupiterin ympyrä
    float radius = 0.8;
    float dist = length(uv);
    if (dist > radius) {
        discard;  // Vain ympyrä näytetään
    }

    // Pyöritetään planeettaa ajan funktiona
    float rotationSpeed = 0.2;
    float angle = time * rotationSpeed;
    uv = mat2(cos(angle), -sin(angle), sin(angle), cos(angle)) * uv;

    // Luodaan Jupiterin pilvivyöt, joihin on lisätty turbulenssia
    vec3 color = generateBands(uv);

    // Vignette-efekti (tummennetaan reunaa)
    float vignette = smoothstep(radius, radius - 0.1, dist);
    color *= vignette;

    // Lopputulos
    gl_FragColor = vec4(color, 1.0);
}

