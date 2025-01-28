#if 1
// Signed distance function for a rounded box
float sdRoundBox(vec3 p, vec3 b, float r) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

// Soft shadow computation
float softShadow(vec3 ro, vec3 rd, float k) {
    float res = 1.0;
    float t = 0.02; // Start ray march at a small step
    for (int i = 0; i < 32; i++) {
        float h = sdRoundBox(ro + rd * t, vec3(0.5), 0.1);
        res = min(res, k * h / t);
        t += clamp(h, 0.02, 0.1);
        if (h < 0.001) break;
    }
    return clamp(res, 0.0, 1.0);
}

// Phong lighting model
vec3 phongLighting(vec3 pos, vec3 normal, vec3 lightDir, vec3 viewDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0); // Specular power
    return vec3(0.1) + vec3(1.0) * diff + vec3(1.0) * spec;
}

void main() {
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 ro = vec3(0.0, 0.0, 3.0); // Ray origin
    vec3 rd = normalize(vec3(uv, -1.5)); // Ray direction

    vec3 lightPos = vec3(2.0 * sin(time), 2.0, 2.0 * cos(time));
    // Background color (sky-ish)
    vec3 bgColor = vec3(0.7, 0.8, 1.0); 
    vec3 col     = bgColor;

    // Ray marching
    float t = 0.0;
    float d;
    for (int i = 0; i < 128; i++) {
        vec3 p = ro + rd * t;
        d = sdRoundBox(p, vec3(0.5), 0.1);
        if (d < 0.001) {
            // Compute normal at the intersection point
            vec3 normal = normalize(vec3(
                sdRoundBox(p + vec3(0.001, 0.0, 0.0), vec3(0.5), 0.1) - d,
                sdRoundBox(p + vec3(0.0, 0.001, 0.0), vec3(0.5), 0.1) - d,
                sdRoundBox(p + vec3(0.0, 0.0, 0.001), vec3(0.5), 0.1) - d
            ));

            // Lighting calculations
            vec3 lightDir = normalize(lightPos - p);
            vec3 viewDir = normalize(-rd);
            float shadow = softShadow(p + normal * 0.01, lightDir, 8.0);
            col = phongLighting(p, normal, lightDir, viewDir) * shadow;

            break;
        }
        t += d;
        if (t > 10.0) break;
    }

    col = mix(col, bgColor, exp(-0.05 * t * t)); // Add fog effect
    gl_FragColor = vec4(col, 1.0);
}
#else
// ---------------------------------------------------------------------------------
// 1) Basic box SDF
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    float outsideDist = length(max(d, 0.0));
    float insideDist  = min(max(d.x, max(d.y, d.z)), 0.0);
    return outsideDist + insideDist;
}

// 2) Single brick SDF
float sdBrick(vec3 p, vec2 brickSize, float brickDepth) {
    vec3 halfSize = vec3(brickSize * 0.5, brickDepth * 0.5);
    return sdBox(p, halfSize);
}

// 3) Infinite brick wall
float sdBrickWall(vec3 p)
{
    vec2 brickSize = vec2(0.6, 0.3);
    float brickDepth = 0.2;
    float mortar     = 0.02;

    float row = floor(p.y / (brickSize.y + mortar));
    float rowOffset = mod(row, 2.0) * 0.5 * brickSize.x;

    vec3 local = p;
    local.x = mod(local.x - rowOffset, (brickSize.x + mortar)) - 0.5*(brickSize.x + mortar);
    local.y = mod(local.y,            (brickSize.y + mortar)) - 0.5*(brickSize.y + mortar);
    // local.z = p.z remains as-is (no repeating in Z).

    return sdBrick(local, brickSize, brickDepth);
}

// ---------------------------------------------------------------------------------
// 4) Main loop replacement
void main() {
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 ro = vec3(0.0, 0.0, 3.0); // Ray origin
    vec3 rd = normalize(vec3(uv, -1.5)); // Ray direction

    vec3 lightPos = vec3(2.0 * sin(time), 2.0, 2.0 * cos(time));
    // Background color (sky-ish)
    vec3 bgColor = vec3(0.7, 0.8, 1.0); 
    vec3 col     = bgColor;

    // Ray marching
    float t = 0.0;
    // Replace sdRoundBox with sdBrickWall:
    float d;
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + rd * t;
        d = sdBrickWall(p); // <---------------- changed here

        if (d < 0.001) {
            // Normal via small offsets
            vec3 normal = normalize(vec3(
                sdBrickWall(p + vec3(EPSILON, 0.0,     0.0    )) - d,
                sdBrickWall(p + vec3(0.0,     EPSILON, 0.0    )) - d,
                sdBrickWall(p + vec3(0.0,     0.0,     EPSILON)) - d
            ));

            // Continue with your shadow & lighting
            float shadow = softShadow(p + 0.01*normal, lightDir, 8.0);
            vec3  color  = phongLighting(p, normal, lightDir, viewDir);
            col = color * shadow;

            break;
        }
        t += d;
        if (t > FAR_DIST) break;
    }
    // Fog, background, etc.
    col = mix(col, bgColor, exp(-0.05 * t * t));
    gl_FragColor = vec4(col, 1.0);
}
#endif

