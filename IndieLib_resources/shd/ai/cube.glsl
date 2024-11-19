/*
Jos käytämme `uniform vec4 iMouse` -muuttujaa kuution asennon määrittämiseen, voimme tulkita hiiren sijainnin avulla, missä asennossa kuutio on. Yleisesti `iMouse`-muuttujan komponenteista `(iMouse.x, iMouse.y)` voidaan tulkita seuraavasti:

- `iMouse.x` ja `iMouse.y`: voivat määrittää kuution kierron kulmat x- ja y-akseleiden suhteen.
- `iMouse.z` ja `iMouse.w` ovat lisäparametreja, joita voitaisiin käyttää esimerkiksi zoomaukseen tai tarkentamiseen, mutta tässä keskitymme perusasentoon ja kiertoon.

Käytämme `iMouse`-muuttujan ensimmäistä kahta komponenttia kääntämään kuutiota fragment shaderissa.
*/

// Kiinteä valon suunta
vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

// Kuution keskipiste
vec3 cubeCenter = vec3(200.0, 150.0, 0.0);  // Kuvitteellinen keskipiste

// Kuution koko
vec3 cubeSize = vec3(50.0, 50.0, 50.0);

// Kuution perusväri
vec3 baseColor = vec3(1.0, 0.5, 0.3);

void main() {
    
    // Hiiren koordinaateista lasketaan kulmat (esim. välillä -1.0 - 1.0)
    float angleX = iMouse.x+time;// / 800.0) * 3.14159;  // Skaalataan näytön koon mukaan
    float angleY = iMouse.y;// / 600.0) * 3.14159;
    
    // Lasketaan kierron mukaan normaali suuntaamalla x- ja y-kiertoja
    mat3 rotX = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(angleX), -sin(angleX),
        0.0, sin(angleX), cos(angleX)
    );
    mat3 rotY = mat3(
        cos(angleY), 0.0, sin(angleY),
        0.0, 1.0, 0.0,
        -sin(angleY), 0.0, cos(angleY)
    );
    
    // Fragmentin sijainti suhteessa kuution keskipisteeseen
    vec3 fragPos = (gl_FragCoord.xyz - cubeCenter) * rotX * rotY;

    // Arvioidaan kuution normaalit fragmentin sijainnin perusteella
    vec3 normal;
    if (abs(fragPos.x) > abs(fragPos.y) && abs(fragPos.x) > abs(fragPos.z)) {
        normal = vec3(sign(fragPos.x), 0.0, 0.0);  // x-suuntainen pinta
    } else if (abs(fragPos.y) > abs(fragPos.x) && abs(fragPos.y) > abs(fragPos.z)) {
        normal = vec3(0.0, sign(fragPos.y), 0.0);  // y-suuntainen pinta
    } else {
        normal = vec3(0.0, 0.0, sign(fragPos.z));  // z-suuntainen pinta
    }
    
    // Diffuusi valaistus
    float diffuse = max(dot(normal, lightDir), 0.0);
    
    // Kuution väri valaistuksen mukaan
    vec3 color = baseColor * diffuse;

    // Tarkistetaan, onko fragmentti kuution rajoissa
    if (abs(fragPos.x) <= cubeSize.x / 2.0 && abs(fragPos.y) <= cubeSize.y / 2.0 && abs(fragPos.z) <= cubeSize.z / 2.0) {
        // Jos fragmentti on kuution rajoissa, väritetään se
        gl_FragColor = vec4(color, 1.0);
    } else {
        // Muuten jätetään fragmentti tyhjäksi (esim. läpinäkyvä)
        discard;
    }
}

/*
### Selitys

1. **Hiiren koordinaatit kääntönä**: `iMouse.x` ja `iMouse.y` skaalataan kulmiksi, joiden avulla kuutiota voidaan kääntää x- ja y-akseleiden suhteen.
   - `angleX` vastaa x-akselin kääntöä.
   - `angleY` vastaa y-akselin kääntöä.

2. **Kääntömatriisit**:
   - `rotX` kääntää kuutiota x-akselin ympäri.
   - `rotY` kääntää kuutiota y-akselin ympäri.
   - Fragmentin sijaintia (`fragPos`) muokataan kertomalla se näillä matriiseilla.

3. **Fragmentin sijainti**: `fragPos` määrittää fragmentin sijainnin suhteessa kuution keskipisteeseen ja sen kiertoon perustuen.

4. **Normaalien arviointi**: Normaalisuunta päätellään `fragPos`-arvon perusteella, ja näin simuloidaan kuution valaistusta ilman `varying`-muuttujia.

5. **Diffuusi valaistus ja väritys**: Lopuksi lasketaan valaistuksen diffuusiosa ja väritetään kuution pinta sen mukaan.

Tämän avulla voit pyörittää kuutiota hiiren x- ja y-koordinaattien avulla ilman `varying`-muuttujia.
*/
