 
// SDF for a capped cylinder
float sdCappedCylinder(vec3 p, float h, float r) {
    float d = length(p.xz) - r;
    d = max(d, abs(p.y) - h);
    return d;
}

// SDF for a box
float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Repeat space along a given axis
vec3 repeat(vec3 p, vec3 c) {
    return mod(p + 0.5 * c, c) - 0.5 * c;
}
 
float towerSDF(vec3 p) {
    // Main tower (capped cylinder)
    float tower = sdCappedCylinder(p, 2.0, 0.5);

    // Add repeated boxes for details (e.g., windows)
    vec3 q = repeat(p, vec3(0.3, 0.3, 0.3));
    float details = sdBox(q, vec3(0.1, 0.1, 0.1));

    // Combine the tower and details
    return min(tower, details);
}
 
float battlementSDF(vec3 p) {
    // Create a single battlement box
    float box = sdBox(p, vec3(0.1, 0.05, 0.1));

    // Repeat the boxes along the x and z axes
    vec3 q = repeat(p, vec3(0.2, 0.0, 0.2));
    return sdBox(q, vec3(0.1, 0.05, 0.1));
}

float sceneSDF(vec3 p) {
    // Combine the tower and battlement
    float tower = towerSDF(p);
    float battlement = battlementSDF(p + vec3(0.0, 2.05, 0.0)); // Position the battlement on top
    return min(tower, battlement);
}
 
float raymarch(vec3 ro, vec3 rd, float maxDist, int maxSteps) {
    float t = 0.0;
    for (int i = 0; i < maxSteps; i++) {
        vec3 p = ro + t * rd;
        float d = sceneSDF(p);
        if (d < 0.001) return t; // Hit
        t += d;
        if (t >= maxDist) break; // Miss
    }
    return -1.0; // Miss
}
 
vec3 computeNormal(vec3 p) {
    float eps = 0.001;
    float d = sceneSDF(p);
    vec3 n = d - vec3(
        sceneSDF(p - vec3(eps, 0.0, 0.0)),
        sceneSDF(p - vec3(0.0, eps, 0.0)),
        sceneSDF(p - vec3(0.0, 0.0, eps))
    );
    return normalize(n);
}
 
vec3 lighting(vec3 p, vec3 rd) {
    vec3 lightPos = vec3(2.0, 5.0, 2.0);
    vec3 normal = computeNormal(p);
    vec3 lightDir = normalize(lightPos - p);

    // Diffuse
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 viewDir = -rd;
    vec3 reflectDir = reflect(-lightDir, normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    return diffuse * vec3(1.0, 0.8, 0.6) + specular * vec3(1.0);
}
 
vec3 sky(vec3 rd) {
    return mix(vec3(0.5, 0.7, 1.0), vec3(1.0), smoothstep(-0.1, 0.1, rd.y));
}
 
mat3 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        c, 0.0, s,
        0.0, 1.0, 0.0,
        -s, 0.0, c
    );
}
 
void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    // Camera setup
    vec3 ro = vec3(0.0, 1.0, 5.0); // Ray origin
    vec3 rd = normalize(vec3(uv, -1.0)); // Ray direction

    // Rotate the scene
    float angle = iTime * 0.5; // Rotate over time
    ro *= rotateY(angle);
    rd *= rotateY(angle);

    // Raymarch
    float t = raymarch(ro, rd, 20.0, 100);

    // Shade the scene
    vec3 col = vec3(0.0);
    if (t > 0.0) {
        vec3 p = ro + t * rd;
        col = lighting(p, rd);
    } else {
        col = sky(rd); // Sky for misses
    }

    gl_FragColor = vec4(col, 1.0);
}

