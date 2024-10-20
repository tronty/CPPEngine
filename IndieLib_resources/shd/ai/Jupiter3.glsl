#ifdef GL_ES
precision mediump float;
#endif

// Perlin-kohinan kaltaisen funktion avulla luodaan turbulenssia
float noise(vec2 p) {
    return sin(p.x) * sin(p.y);
}

// Turbulenssiefekti pilvien "pyörteille"
float turbulence(vec2 p) {
    float w = 100.0;
    float t = 0.0;
    for (float i = 1.0; i <= 10.0; i++) {
        t += abs(noise(p * i)) / i;
        w *= 0.5;
    }
    return t;
}

void main() {
    // Normalisoidaan koordinaatit [0, 1] välille
    vec2 uv = xlv_TEXCOORD0.xy / resolution.xy;
    uv = uv * 2.0 - 1.0;  // Normaalisoidaan koordinaatit [-1, 1] alueelle
    
    // Korjataan leveys-korkeussuhde
    uv.x *= resolution.x / resolution.y;
    
    // Jupiterin pilvivyöt: Taajuus ja määrä pilvikerroksia
    float cloudBands = sin(uv.y * 20.0 + turbulence(uv * 3.0 + time * 0.2)) * 0.3;
    
    // Pilvien värin määrittäminen (pohjavärit, turbulenssi ja sekoitus)
    vec3 baseColor = vec3(1.0, 0.5, 0.3);   // Oranssin värinen Jupiterin pinta
    vec3 cloudColor = vec3(0.9, 0.9, 0.9);  // Vaaleammat pilvet
    
    // Turbulenssia ja pilvikerroksia lisätään
    float t = turbulence(uv * 3.0 + time * 0.1) * 0.2;  // Turbulenssin vaikutus
    float bandMix = smoothstep(0.4, 0.6, cloudBands + t); // Sekoitetaan pilvikerrokset
    
    // Lopullinen väri: sekoitus pilviä ja taustaa
    vec3 color = mix(baseColor, cloudColor, bandMix);

    gl_FragColor = vec4(color, 1.0);  // Viedään lopullinen väri ulos
}

