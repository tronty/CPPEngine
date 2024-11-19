/*
o1-preview:
Voisitko luoda pallomaisen pyörivän Venuksen fragmenttishaderin (GLSL versio 1.3) turbulenssilla ilman uniform shader muuttujia?
ChatGPT sanoi:
GPT

Tässä on esimerkki GLSL-versiolla 1.3 kirjoitetusta pallomaisesta fragmenttishaderista, joka luo pyörivän Venuksen kaltaisen planeetan, jossa on turbulenttinen ilmakehä. Shaderissä ei käytetä uniform-muuttujia, joten kaikki liike ja tekstuurit määritetään suoraan shaderissa itsessään. Pyörivä efekti ja turbulenssi toteutetaan käyttäen Perlin noise -pohjaista turbulenttista kohinaa.
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

// Turbulenssi-funktio
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
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0); // Oletetaan 800x600 resoluutio
    uv = uv * 2.0 - 1.0; // Muutetaan UV koordinaatit -1.0 - 1.0 alueelle

    // Pallokoordinaatit
    float r = length(uv);
    if (r > 1.0) discard; // Hylkää pikselit, jotka eivät ole pallon sisällä

    // Spherical mapping
    float theta = atan(uv.y, uv.x) + time; // Kulma x- ja y-akselin suhteen
    float phi = acos(uv.y / r); // Kulma y-akselin suhteen

    // Pyörivä tekstuuri: käytetään aikaa simuloimaan Venuksen pyörimistä
    float time = mod(gl_FragCoord.x + gl_FragCoord.y + sin(uv.x * 10.0) * 10.0, 360.0) * 0.001;

    // Turbulenssi koordinaatit ja väritys
    vec2 noiseCoord = vec2(theta * 0.5 + time, phi * 0.5);
    float turb = turbulence(noiseCoord * 3.0);

    // Planeetan väritys (Venusmaiset värit)
    vec3 venusColor = vec3(1.0, 0.7, 0.3);  // Lähes keltainen väri
    vec3 cloudColor = vec3(1.0, 0.9, 0.6);  // Pilvien väritys

    // Lopullinen väri turbulenssin perusteella
    vec3 color = mix(venusColor, cloudColor, turb * 0.5);

    gl_FragColor = vec4(color, 1.0);
}

/*
*/

