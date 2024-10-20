#ifdef GL_ES
precision mediump float;
#endif

// Perlin noise -funktio
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float smoothNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(noise(i + vec2(0.0, 0.0)), noise(i + vec2(1.0, 0.0)), u.x),
               mix(noise(i + vec2(0.0, 1.0)), noise(i + vec2(1.0, 1.0)), u.x), u.y);
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < 6; i++) {
        value += amplitude * smoothNoise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    
    return value;
}

vec3 getJupiterColor(float value) {
    // Jupiterin värit - ruskeat, oranssit ja valkoiset sävyt
    vec3 color1 = vec3(0.8, 0.6, 0.4); // Vaalea oranssi
    vec3 color2 = vec3(0.6, 0.4, 0.2); // Tumma oranssi
    vec3 color3 = vec3(1.0, 0.9, 0.8); // Vaalea lähes valkoinen
    
    if (value < 0.4) {
        return mix(color2, color1, smoothstep(0.0, 0.4, value));
    } else {
        return mix(color1, color3, smoothstep(0.4, 1.0, value));
    }
}

void main() {
    vec2 uv = xlv_TEXCOORD0.xy / resolution.xy;
    uv.x *= resolution.x / resolution.y;
    
    // Sovitetaan Jupiterin pyörivään planeettaan (käyrät raidat)
    float longitude = uv.x * 3.141592 * 2.0 - 3.141592; // Longitude  (-π to π)
    float latitude = (uv.y - 0.5) * 3.141592;            // Latitude   (-π/2 to π/2)
    
    // Simuloidaan pyörivää Jupiterin ilmakehää
    float time = time * 0.1;
    float noiseScale = 3.0;
    vec2 p = vec2(longitude + time, latitude);
    
    // Lisätään fraktaalista kohinaa Jupiterin tekstuuriin
    float noiseValue = fbm(p * noiseScale);
    
    // Käytetään fraktaalikohinaa turbulenssin luomiseen
    vec3 color = getJupiterColor(noiseValue);
    
    // Lisätään pyöreän planeetan efekti sovittamalla pallokoordinaatteihin
    float circleMask = smoothstep(0.9, 1.0, length(uv - 0.5));
    
    // Varmistetaan, että planeetta on pyöreä ja värjätty
    color *= circleMask;
    
    gl_FragColor = vec4(color, 1.0);
}

