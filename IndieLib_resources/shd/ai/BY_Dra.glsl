// Funktio tähtien piirtämiseen
float star(vec2 p, vec2 center, float radius, float glow) {
    float d = length(p - center);
    return exp(-pow(d / radius, glow));
}

// Funktio tähtien pinnan vaihtelulle (tahrojen simulointi)
float starSpots(vec2 p, vec2 center, float radius, float spotIntensity, float spotSize) {
    vec2 offset = p - center;
    float angle = atan(offset.y, offset.x) * 8.0; // 8 pistettä kiertoradalla
    float spotEffect = sin(angle) * exp(-length(offset) / spotSize);
    return spotIntensity * spotEffect;
}

void main() {
    // Ikkunan koko (kovakoodattu, ei uniform)
    vec2 resolution = vec2(800.0, 600.0);

    // Normaalisoitu koordinaattijärjestelmä
    vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;
    uv.y *= resolution.y / resolution.x; // Korjaa kuvasuhde

    // Tähtien sijainnit
    vec2 star1Pos = vec2(-0.25, 0.0); // Ensimmäinen tähti
    vec2 star2Pos = vec2(0.25, 0.0);  // Toinen tähti

    // Tähtien ominaisuudet
    float star1Radius = 0.15;
    float star2Radius = 0.12;
    float glow = 4.0;

    // Tahrojen ominaisuudet
    float spotIntensity = 0.4;
    float spotSize = 0.2;

    // Tähtien värit
    vec3 star1Color = vec3(1.0, 0.6, 0.3); // Oranssi tähti
    vec3 star2Color = vec3(0.6, 0.8, 1.0); // Sinivalkoinen tähti

    // Taustaväri
    vec3 bgColor = vec3(0.0);

    // Tähtien kirkkaudet
    float brightness1 = star(uv, star1Pos, star1Radius, glow);
    float brightness2 = star(uv, star2Pos, star2Radius, glow);

    // Tahrojen kirkkauden vähennys
    brightness1 -= starSpots(uv, star1Pos, star1Radius, spotIntensity, spotSize);
    brightness2 -= starSpots(uv, star2Pos, star2Radius, spotIntensity, spotSize);

    // Lopullinen väri yhdistämällä molemmat tähdet
    vec3 color = bgColor
                 + max(0.0, brightness1) * star1Color
                 + max(0.0, brightness2) * star2Color;

    // Aseta fragmentin väri
    gl_FragColor = vec4(color, 1.0);
}

