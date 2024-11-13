/*
Luodaan Uranus-planeetan fragmenttishaderi GLSL 1.3 -versiona. Tämä shader pyrkii luomaan kallistuneen pyörivän vaikutelman, jossa on pilvivöitä ja turbulenssia. Shader ei käytä uniform-muuttujia, joten se on täysin riippumaton ulkopuolisista muuttujista.

Koodissa käytetään seuraavia ominaisuuksia:
- **Pilvivyöt**: Lisätään säännöllisiä vaaka-alueita, jotka edustavat planeetan pilvivöitä.
- **Turbulenssi**: Käytetään noise-funktiota satunnaisen turbulenssin luomiseen.
- **Kallistus**: Muutetaan koordinaatteja simuloimaan kallistunut akseli.
- **Pyöriminen**: Lisätään ajan kanssa muuttuva termi, joka simuloi pyörimistä.

*/

// 3D simplex noise funktio Uranuksen pilvivyötä ja turbulenssia varten
float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f*f*(3.0 - 2.0*f);
    
    float n = dot(i, vec3(1.0, 57.0, 113.0));
    return mix(mix(mix(sin(n+ 0.0) * 43758.5453, sin(n+ 1.0) * 43758.5453, f.x),
                   mix(sin(n+57.0) * 43758.5453, sin(n+58.0) * 43758.5453, f.x), f.y),
               mix(mix(sin(n+113.0) * 43758.5453, sin(n+114.0) * 43758.5453, f.x),
                   mix(sin(n+170.0) * 43758.5453, sin(n+171.0) * 43758.5453, f.x), f.y), f.z);
}

// Pääasiallinen fragmenttishader-funktio
void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 800.0) - 0.5;
    
    // Pyörimis- ja kallistuskulmat
    float rotation = 0.1;      // Pyörimisnopeus
    float tilt = radians(97.77); // Uranuksen akselin kallistuskulma

    // Muokataan UV-koordinaatteja kallistuksen simuloimiseksi
    float angle = rotation * gl_FragCoord.x * 0.001;
    mat2 rotationMatrix = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    uv = rotationMatrix * uv;

    // Sovelletaan kallistuskoordinaatteja
    vec3 pos = vec3(uv.x, uv.y * cos(tilt), uv.y * sin(tilt));

    // Turbulenssi ja pilvivyöt
    float bands = sin(10.0 * pos.y + noise(pos * 5.0) * 2.0);
    float clouds = smoothstep(0.3, 0.7, bands + noise(pos * 10.0) * 0.5);
    
    // Värjäys Uranuksen tyypilliseen vaaleansiniseen sävyyn pilvivöillä
    vec3 planetColor = vec3(0.4, 0.7, 0.9);     // Uranuksen väri
    vec3 cloudColor = vec3(0.6, 0.8, 1.0);      // Pilvien väri
    vec3 color = mix(planetColor, cloudColor, clouds);
    
    // Alpha-blending viisteen aikaansaamiseksi (jos halutaan)
    float edge = 1.0 - smoothstep(0.8, 1.0, length(uv));

    gl_FragColor = vec4(color * edge, 1.0);
}

/*
### Selitykset:

1. **Noise-funktio**: Tämä on yksinkertaistettu versio, joka luo "satunnaista" kohinaa. Tässä käytetään sinifunktiota simuloimaan 3D noisea.
2. **Kallistuksen ja pyörimisen toteutus**: `rotationMatrix` kiertää `uv`-koordinaatteja pyörimisen simulointiin. Kallistus otetaan huomioon muuttamalla y-koordinaattia.
3. **Pilvivyöt ja turbulenssi**: `bands` käyttää sinifunktiota ja noisea simuloimaan pilvivöitä, ja `clouds`-kerroin lisää turbulenssia.
4. **Värjäys**: Väritään planeetan runko sinertävällä sävyllä ja sekoitetaan siihen vaaleampia pilvivyöitä.
*/

