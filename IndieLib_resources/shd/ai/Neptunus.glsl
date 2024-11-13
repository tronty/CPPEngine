/*
Luodaan GLSL-fragmenttishaderi, joka simuloisi pyörivän Neptunuksen kaltaista planeettaa, johon on lisätty pilvivyöt ja turbulenssia. Koska toivottiin, ettei käytetä `uniform`-muuttujia, emme voi hyödyntää muuttujia esimerkiksi ajan tai pyörimisnopeuden säätelyyn suoraan shaderissa. Käytämme pelkkiä sisäisiä laskelmia luomaan pyörimisefektin ja turbulenssin tekstuurin, joka simuloi Neptunuksen ulkonäköä.

Shader perustuu Perlinin meluun, jonka avulla saadaan aikaan pilvimäistä rakennetta ja pyörimisliikettä.
*/
// Perlin noise -laskenta funktioita
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float total = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; i++) {
        total += noise(p) * amplitude;
        p *= 2.0;
        amplitude *= 0.5;
    }
    return total;
}

void main() {
    // Koordinaattien normalisointi ja pyörimisefekti
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);  // Sovita resoluution mukaisesti
    uv = uv * 2.0 - 1.0;  // Keskelle (0, 0)
    
    float angle = atan(uv.y, uv.x);       // Kulma koordinaateista
    float radius = length(uv);            // Etäisyys keskipisteestä
    
    // Rajoita piiriä pallomaiseksi Neptunukseksi
    if (radius > 1.0) discard;            // Jos ei pallon sisällä, älä piirrä
    
    // Pyörimisliikkeen laskenta (staattinen simulaatio)
    float rotationOffset = 0.1;          // Muutetaan kulmaa jotta saadaan pyörivä vaikutelma
    angle += rotationOffset * radius;
    
    // Uudelleenkoordinoi tekstuurin kohdistus
    vec2 rotatedUV = vec2(cos(angle), sin(angle)) * radius;

    // Pilvivyöhykkeet: fbm ja Perlin noise tekstuuri
    float cloudLayers = fbm(rotatedUV * 5.0 + vec2(0.5, 0.5));
    
    // Turbulenssi: lisää voimakkuutta alaspäin kasvavalla melulla
    float turbulence = fbm(rotatedUV * 15.0);
    
    // Lopullinen pilvikuvio
    float clouds = smoothstep(0.2, 0.8, cloudLayers + 0.5 * turbulence);
    
    // Värien määrittely Neptunusmaisesti
    vec3 neptuneColor = mix(vec3(0.1, 0.2, 0.7), vec3(0.2, 0.3, 0.9), clouds);
    
    // Lopputulos
    gl_FragColor = vec4(neptuneColor, 1.0);
}

/*
### Shaderin toiminta:

1. **Koordinaattien normalisointi:** Koordinaatit asetetaan keskelle ruutua (0,0) niin, että ne sopivat ympyrän muotoon.

2. **Pyörimisliike:** Käytetään säde-etäisyyttä ja kulmaa, jotka lasketaan polar-koordinaateista. Kulmaa hieman muunnellaan (`rotationOffset`) niin, että pilvet vaikuttavat pyörivän, vaikkakin ilman animaatiota.

3. **Perlinin melu ja fbm-funktiot:** Nämä luovat tekstuurille kerroksittaisen efektin. Käytämme fraktaalista melua pilviefektiä varten, joka sekoitetaan pienemmän mittakaavan turbulenssin kanssa.

4. **Pilvikuvio:** `cloudLayers` ja `turbulence` yhdistetään smoothstep-funktiolla, mikä luo pehmeän siirtymän Neptunuksen eri värivyöhykkeiden välille.

5. **Lopputulos:** Lopuksi sekoitamme värejä, mikä antaa sinivalkoiset Neptunuksen pilvet.
*/

