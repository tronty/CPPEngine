/*
o1-preview: Voisitko luoda pallomaisen pyörivän Jupiter planeetan fragmenttishaderin (GLSL versio 1.3) pilvivöineen turbulenssilla ilman uniform shader muuttujia?
Tässä on GLSL-versiolla 1.3 kirjoitettu fragmenttishader, joka luo pyörivän Jupiterin kaltaisen planeetan pilvivöineen ja turbulenttisuudella ilman `uniform`-muuttujia. Shader sisältää Perlin-kohinaan perustuvan turbulenttisuuden, joka simuloi Jupiterin ilmakehän kaasuvirtauksia ja pilvivöitä.
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

void main() {
    // Muutetaan UV-koordinaatit pallon pintaan sopiviksi
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0); // Oletetaan resoluutio 800x600
    uv = uv * 2.0 - 1.0; // Skalaus -1.0 ja 1.0 välille

    // Pallokoordinaatit (r = etäisyys keskipisteestä)
    float r = length(uv);
    if (r > 1.0) discard; // Hylätään pikselit, jotka ovat pallon ulkopuolella

    // Spherical mapping
    float theta = atan(uv.y, uv.x); // x-y-akselin välinen kulma
    float phi = acos(uv.y / r); // y-akselin suhteen kulma

    // Pyörimisnopeuden simulointi ajan kanssa
    //float time = mod(gl_FragCoord.x + gl_FragCoord.y + sin(uv.x * 5.0) * 10.0, 360.0) * 0.001;

    // Jupiterin pilvivyöt
    float latitudeEffect = sin(phi * 8.0); // Vyöt symmetrisesti leveysasteiden mukaan
    vec2 noiseCoord = vec2(theta * 3.0 + time, latitudeEffect);

    // Turbulenssi pilvivyötä varten
    float turb = turbulence(noiseCoord * 2.0);

    // Jupiterin väripaletti
    vec3 jupiterBaseColor = vec3(0.8, 0.5, 0.3);  // Ruskehtava sävy
    vec3 jupiterCloudColor = vec3(1.0, 0.9, 0.8); // Pilvien vaalea sävy
    vec3 stripeColor = mix(jupiterBaseColor, jupiterCloudColor, latitudeEffect);

    // Lopullinen väri turbulenssin perusteella
    vec3 finalColor = mix(stripeColor, jupiterCloudColor, turb * 0.4);

    gl_FragColor = vec4(finalColor, 1.0);
}

/*
### Selitys:

1. **Perlin-kohina ja turbulenssi**: `noise`- ja `turbulence`-funktiot luovat Perlin-kohinaan perustuvan turbulenssin. Tämä simuloi Jupiterin ilmakehän virtauksia ja pyörteitä.

2. **Pallokoordinaattien laskeminen**: `uv`-koordinaatit muunnetaan -1.0 ja 1.0 välille ja niistä johdetaan pallomaiset koordinaatit (`theta` ja `phi`).

3. **Jupiterin vyöt**: Leveysasteita jäljittelemällä käytämme sinusfunktiota (`latitudeEffect`), joka luo Jupiterille ominaiset vaakasuuntaiset pilvivyöt.

4. **Pyöriminen ja pilvien liike**: Simuloimme Jupiterin pyörimistä lisäämällä ajallisesti muuttuvan termiin `theta`-koordinaattiin, joka luo jatkuvan liikkeen vaikutelman.

5. **Väritys**: `jupiterBaseColor` ja `jupiterCloudColor` ovat väripaletit, jotka jäljittelevät Jupiterin ruskehtavia pilvivöitä ja vaaleampia pilviä. Lopullinen väri saadaan sekoittamalla värit turbulenssin ja leveysastevaikutuksen avulla.

Tämä shader tuottaa Jupiterin kaltaisen planeetan, jossa on ominaiset vaakasuuntaiset pilvivyöt ja turbulenttinen ilmakehä.
*/

