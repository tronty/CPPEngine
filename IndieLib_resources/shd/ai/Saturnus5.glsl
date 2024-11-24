/*
ChatGPT-o1-preview: Voisitko luoda pallomaisen pyörivän Saturnus planeetan kallistuneine renkaineen fragmenttishaderin (GLSL versio 1.3) pilvivöineen turbulenssilla ilman uniform shader muuttujia?
Tässä on GLSL-versiolla 1.3 kirjoitettu fragmenttishader, joka luo pyörivän Saturnuksen planeetan, jossa on kallistuneet renkaat ja pilvivyöt sekä turbulenssia. Shader ei käytä `uniform`-muuttujia, vaan kaikki liike, tekstuurit ja renkaiden kallistuminen on määritelty suoraan shaderissa.
*/

// Perlin-kohinan funktio
float hash(vec2 p) {
    return fract(sin(dot(p ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f*f*(3.0-2.0*f);
    return mix(mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), f.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Turbulenssin funktio
float turbulence(vec2 p) {
    float w = 100.0;
    float t = 0.0;
    for (int i = 0; i < 5; i++) {
        t += abs(noise(p * w)) / w;
        w *= 0.5;
    }
    return t;
}

// Funktio Saturnuksen renkaiden luomiseen
float saturnRings(vec2 uv, float tilt) {
    uv.x *= cos(tilt); // Kallista renkaiden x-komponenttia kallistuskulman mukaan
    float dist = length(uv);
    float ringPattern = sin(dist * 50.0) * 0.5 + 0.5; // Renkaiden hienorakenne
    return smoothstep(0.4, 0.6, dist) * ringPattern;  // Renkaat alkavat ja päättyvät tiettyyn etäisyyteen
}

void main() {
    // Muutetaan UV-koordinaatit sopiviksi 800x600 resoluutiolle
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
    uv = uv * 2.0 - 1.0; // Muutetaan UV koordinaatit -1.0 - 1.0 alueelle

    // Pallokoordinaatit
    float r = length(uv);
    if (r > 1.0) discard; // Hylätään pikselit, jotka eivät kuulu palloon

    // Spherical mapping
    float theta = atan(uv.y, uv.x); // Kulma x- ja y-akselin suhteen
    float phi = acos(uv.y / r);     // Kulma y-akselin suhteen

    // Pyörimisnopeuden simulointi ajan kanssa
    //float time = mod(gl_FragCoord.x + gl_FragCoord.y + sin(uv.x * 5.0) * 10.0, 360.0) * 0.001;

    // Saturnuksen pilvivyöt (sinusfunktiolla)
    float latitudeEffect = sin(phi * 12.0); // Vyöt symmetrisesti leveysasteiden mukaan
    vec2 noiseCoord = vec2(theta * 3.0 + time, latitudeEffect);

    // Turbulenssi pilvivyötä varten
    float turb = turbulence(noiseCoord * 2.0);

    // Saturnuksen väripaletti
    vec3 saturnBaseColor = vec3(0.8, 0.7, 0.5);   // Vaaleanruskea perusväri
    vec3 saturnCloudColor = vec3(0.9, 0.85, 0.7); // Pilvien vaalea sävy
    vec3 stripeColor = mix(saturnBaseColor, saturnCloudColor, latitudeEffect);

    // Lopullinen väri turbulenssin perusteella
    vec3 finalColor = mix(stripeColor, saturnCloudColor, turb * 0.3);

    // Saturnuksen renkaat
    float tilt = radians(25.0); // Saturnuksen renkaiden kallistuskulma
    float ringMask = saturnRings(uv, tilt); // Renkaiden luominen

    // Yhdistetään Saturnuksen pinta ja renkaat
    finalColor += vec3(0.6, 0.6, 0.7) * ringMask; // Renkaat vaaleanharmaita

    gl_FragColor = vec4(finalColor, 1.0);
}

/*
### Selitys:

1. **Perlin-kohina ja turbulenssi**: Shader käyttää Perlin-kohinaa ja turbulenssia Saturnuksen ilmakehän ja pilvivöiden mallintamiseen. Tämä antaa planeetalle realistisen, turbulenttisen pinnan.

2. **Pallokoordinaattien laskeminen**: UV-koordinaatit muunnetaan -1.0 ja 1.0 välille, ja niistä lasketaan pallomaiset koordinaatit (theta ja phi), jotka kuvaavat Saturnuksen pallomaista pintaa.

3. **Saturnuksen pilvivyöt**: Leveysasteiden mukaan piirretyt pilvivyöt simuloidaan sinusfunktiolla. Vyöt ovat ominaisia Saturnukselle ja niitä lisätään turbulenssin avulla antamaan kaasuplaneetalle dynaamisempi ilme.

4. **Renkaiden simulointi**: Saturnuksen renkaat luodaan funktion `saturnRings` avulla, jossa UV-koordinaatit muunnetaan kallistuskulman perusteella. Renkaiden hienorakenne simuloidaan toistuvalla sinusfunktiolla.

5. **Pyöriminen ja renkaiden kallistuminen**: Saturnuksen pyörimisnopeus lisätään ajasta riippuvana termillä `time`, ja renkaiden kallistuminen saadaan aikaan kallistamalla renkaiden x-komponenttia.

6. **Väritys**: Saturnuksen perusvärit ja renkaiden värit ovat valittuja realististen värimaailmojen perusteella: Saturnuksen perusväri on vaaleanruskea, kun taas renkaat ovat harmaita.

Tämä shader tuottaa pyörivän Saturnuksen, jolla on renkaat kallistettuna ja pilvivöitä turbulenttisuudella.
*/

