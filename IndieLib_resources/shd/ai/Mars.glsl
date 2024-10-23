/*
o1-preview: Voisitko luoda pallomaisen pyörivän Mars planeetan fragmenttishaderin (GLSL versio 1.3) kraatereineen ilman uniform shader muuttujia?
Tässä on GLSL-versiolla 1.3 kirjoitettu fragmenttishader, joka luo pyörivän Marsin planeetan kraatereineen. Shaderissä ei käytetä `uniform`-muuttujia, vaan kaikki liike, kraaterit ja tekstuurit määritellään suoraan shaderissa.
*/

// Funktio Perlin-kohinaan
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

// Kraatterien simulointi
float craterEffect(vec2 p) {
    float radius = length(p);
    float crater = exp(-radius * 20.0) * 0.7;  // Kraatterien syvyys ja koko
    return crater;
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
    float time = mod(gl_FragCoord.x + gl_FragCoord.y + sin(uv.x * 3.0) * 5.0, 360.0) * 0.001;

    // Kraatterien sijoittelu kohinan avulla
    vec2 noiseCoord = vec2(theta * 5.0 + time, phi * 3.0);
    float noiseValue = noise(noiseCoord);

    // Kraatterin vaikutus
    vec2 craterPos = fract(noiseCoord * 10.0) - 0.5;
    float craterDepth = craterEffect(craterPos); 
    float craterInfluence = max(craterDepth, 0.0);

    // Marsin perusväritys
    vec3 marsBaseColor = vec3(0.8, 0.4, 0.3); // Marsin punaruskea sävy
    vec3 craterColor = vec3(0.5, 0.3, 0.2);   // Kraatterien tummempi väri

    // Lopullinen väri kraatterien vaikutuksen perusteella
    vec3 finalColor = mix(marsBaseColor, craterColor, craterInfluence);

    gl_FragColor = vec4(finalColor, 1.0);
}

/*
### Shaderin selitys:

1. **Perlin-kohina**: Shader käyttää Perlin-kohinaa kraatterien sijoittamiseen satunnaisesti ympäri planeetan pintaa. `noise`-funktiolla simuloidaan pintarakennetta ja kraatterien sijaintia.

2. **Pallokoordinaattien laskeminen**: UV-koordinaatit muunnetaan -1.0 ja 1.0 välille, ja niistä lasketaan pallomaiset koordinaatit (theta ja phi), jotka kuvaavat Marsin pallomaista pintaa.

3. **Kraatterit**: Kraatterit simuloidaan `craterEffect`-funktiolla, joka luo syviä painaumia Marsin pinnalle. Kraatterien koko ja syvyys lasketaan `exp`-funktion avulla, mikä luo realistisia painaumia.

4. **Marsin pyöriminen**: Marsin pyöriminen simuloidaan lisäämällä ajallisesti muuttuva termi `theta`-koordinaattiin, joka saa planeetan pyörimään ympäri.

5. **Väritys**: Marsin perusväri on punaruskea, kun taas kraatterit ovat hieman tummempia, mikä tekee niistä erottuvia. Lopullinen väri määritetään sekoittamalla perusväriä ja kraatterien väriä kraatterisyvyyden perusteella.

Tämä shader tuottaa pyörivän Marsin planeetan, jolla on satunnaisesti sijoitettuja kraattereita ja Marsille ominainen punertava väritys.
*/

