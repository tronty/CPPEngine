// Funktio tähtien piirtämiseen
float star(vec2 p, vec2 center, float radius, float glow) {
    float d = length(p - center);
    return exp(-pow(d / radius, glow));
}

// Funktio tähtien kiertoliikkeeseen
vec2 orbit(vec2 center, float radius, float angle) {
    return center + vec2(cos(angle), sin(angle)) * radius;
}

// Funktio tähtipilkujen luomiseen
float starSpots(vec2 p, vec2 center, float radius, float spotIntensity, float spotFrequency) {
    vec2 offset = p - center;
    float distance = length(offset) / radius;
    float angle = atan(offset.y, offset.x) * spotFrequency; // Useampi pilkku ympyrän reunoilla
    float spotEffect = sin(angle) * exp(-distance * 5.0); // Pilkut vaimenevat keskustaa kohti
    return spotIntensity * spotEffect;
}

void main() {
    // Ikkunan koko (kovakoodattu)
    vec2 resolution = vec2(800.0, 600.0);

    // Normaalisoitu koordinaattijärjestelmä
    vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;
    uv.y *= resolution.y / resolution.x; // Korjaa kuvasuhde

    // Kiertoradan parametrit
    float orbitRadius = 0.3; // Kiertoradan säde
    float speed = 0.5;       // Kiertonopeus

    // Aika (simuloitu gl_FragCoord.x:llä; lisää liikettä joka freimillä)
    float time = gl_FragCoord.x * 0.01;

    // Tähtien sijainnit kiertoradalla
    vec2 center = vec2(0.0, 0.0); // Kiertoradan keskipiste
    vec2 star1Pos = orbit(center, orbitRadius, time * speed);
    vec2 star2Pos = orbit(center, orbitRadius, time * speed + 3.14159); // Vastakkaisella puolella

    // Tähtien ominaisuudet
    float star1Radius = 0.1;
    float star2Radius = 0.15;
    float glow = 4.0;

    // Tähtipilkut
    float spotIntensity = 0.4;
    float spotFrequency = 8.0; // Pilkkujen määrä ympyrän kehällä
    
    // Tähtien värit
    vec3 star1Color = vec3(1.0, 0.7, 0.4); // Oranssi tähti
    vec3 star2Color = vec3(0.4, 0.7, 1.0); // Sininen tähti

    // Taustaväri
    vec3 bgColor = vec3(0.0);

    // Piirrä tähdet
    float brightness1 = star(uv, star1Pos, star1Radius, glow);
    float brightness2 = star(uv, star2Pos, star2Radius, glow);

    // Lisää tähtipilkut
    brightness1 -= starSpots(uv, star1Pos, star1Radius, spotIntensity, spotFrequency);
    brightness2 -= starSpots(uv, star2Pos, star2Radius, spotIntensity, spotFrequency);

    // Lopullinen väri yhdistämällä molemmat tähdet
    vec3 color = bgColor
                 + brightness1 * star1Color
                 + brightness2 * star2Color;

    // Aseta fragmentin väri
    gl_FragColor = vec4(color, 1.0);
}

