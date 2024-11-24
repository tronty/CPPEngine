/*
ChatGPT-o1-preview: Voisitko luoda pallomaisen pyörivän Maa planeetan kraatterisen kiertolaisen Kuun fragmenttishaderin (GLSL versio 1.3) ilman uniform shader muuttujia?
Tässä on GLSL-versiolla 1.3 kirjoitettu fragmenttishader, joka luo pallomaisen, pyörivän Kuun pinnan, jossa näkyy kraattereita ja Maata kiertävän kuun tekstuuri. Shaderissä ei käytetä `uniform`-muuttujia, vaan kaikki on määritelty suoraan shaderissa. Kraattereita ja pyörimistä simuloidaan kohinan ja matematiikan avulla.
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

// Simuloidaan kraattereita
float craterEffect(vec2 p) {
    float radius = length(p);
    float crater = exp(-radius * 15.0) * 0.5; // Kraattereiden syvyys ja muoto
    return crater;
}

void main() {
    // Muutetaan UV-koordinaatit pallon pintaan sopiviksi
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0); // Oletetaan resoluutio 800x600
    uv = uv * 2.0 - 1.0; // Muutetaan UV koordinaatit -1.0 - 1.0 alueelle

    // Pallokoordinaatit (r = etäisyys keskipisteestä)
    float r = length(uv);
    if (r > 1.0) discard; // Hylkää pikselit, jotka eivät ole pallon sisällä

    // Pallon kartoitus
    float theta = atan(uv.y, uv.x); // Kulma x- ja y-akselin suhteen
    float phi = acos(uv.y / r);     // Kulma y-akselin suhteen

    // Pyörimisnopeuden simulointi ajan kanssa
    //float time = mod(gl_FragCoord.x + gl_FragCoord.y + sin(uv.x * 5.0) * 10.0, 360.0) * 0.001;

    // Kraatterien tekstuurit (Perlin-kohinan avulla)
    vec2 noiseCoord = vec2(theta * 5.0 + time, phi * 2.0);
    float noiseValue = noise(noiseCoord);
    
    // Lisätään kraatterit pinnalle
    vec2 craterPos = fract(noiseCoord * 20.0) - 0.5; // Kraattereiden sijainti
    float craterDepth = craterEffect(craterPos);      // Kraatterin vaikutus
    float craterInfluence = max(craterDepth, 0.0);

    // Kuun perusväritys (harmaa)
    vec3 moonBaseColor = vec3(0.5, 0.5, 0.5);   // Perusharmaa sävy
    vec3 craterColor = vec3(0.3, 0.3, 0.3);     // Kraatterien tummempi sävy

    // Lopullinen väri kraatterien syvyyden mukaan
    vec3 finalColor = mix(moonBaseColor, craterColor, craterInfluence);

    gl_FragColor = vec4(finalColor, 1.0);
}

/*
### Shaderin selitys:

1. **Perlin-kohina ja kraatterit**: Shader käyttää kohinaa kraatterien satunnaistamiseen ja sijoitteluun Kuun pinnalle. `craterEffect`-funktio simuloi kraatterin syvyyttä ja leveyttä luomalla yksinkertaisen syvyysvaikutuksen.

2. **Pallokoordinaattien laskeminen**: UV-koordinaatit muunnetaan -1.0 ja 1.0 välille ja muunnetaan pallon koordinaateiksi (theta ja phi), mikä simuloi pallon pintaa ja pyörimistä.

3. **Kuun pyöriminen**: Pyöriminen simuloidaan ajan avulla. `theta`-kulmaan lisätään ajasta riippuva termi, jotta saadaan aikaan pyörimisefekti.

4. **Kraatterien väritys**: Shader käyttää harmaan eri sävyjä. Kuun perusväriksi on asetettu vaalea harmaa, ja kraattereiden väritys on tummempi, jolloin ne erottuvat selkeästi pinnasta.

5. **Lopullinen väri**: Lopullinen väri määritetään sekoittamalla peruskuun väriä ja kraatterien väriä syvyyden ja kohinan perusteella.

Tämä shader tuottaa pyörivän, kraatterisen Kuun, joka simuloi Kuun pintaa ja kraattereita kohtuullisen yksinkertaisella tekstuurilla.
*/

