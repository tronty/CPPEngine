// Funktio tähtien piirtämiseen
float star(vec2 p, vec2 center, float radius, float glow) {
    float d = length(p - center);
    return exp(-pow(d / radius, glow));
}

void main() {
    // Ikkunan koko (kovakoodattu, ei uniform)
    vec2 resolution = vec2(800.0, 600.0);

    // Normaalisoitu koordinaattijärjestelmä
    vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;
    uv.y *= resolution.y / resolution.x; // Korjaa kuvasuhde

    // Tähtien sijainnit (kovakoodatut)
    vec2 star1Pos = vec2(-0.3, 0.0); // Ensimmäinen tähti
    vec2 star2Pos = vec2(0.3, 0.0);  // Toinen tähti

    // Tähtien ominaisuudet
    float star1Radius = 0.1;
    float star2Radius = 0.15;
    float glow = 4.0;

    // Tähtien värit
    vec3 star1Color = vec3(1.0, 0.7, 0.4); // Oranssi tähti
    vec3 star2Color = vec3(0.4, 0.7, 1.0); // Sininen tähti

    // Taustaväri
    vec3 bgColor = vec3(0.0);

    // Piirrä tähdet
    float brightness1 = star(uv, star1Pos, star1Radius, glow);
    float brightness2 = star(uv, star2Pos, star2Radius, glow);

    // Lopullinen väri yhdistämällä molemmat tähdet
    vec3 color = bgColor
                 + brightness1 * star1Color
                 + brightness2 * star2Color;

    // Aseta fragmentin väri
    gl_FragColor = vec4(color, 1.0);
}

