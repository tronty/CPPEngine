// -----------------------------------------------------
// Self-contained raymarching SDF example – no uniforms,
// textures or varyings. (Fixed resolution and time.)
// -----------------------------------------------------

// A simple 2D rotation matrix for use on xz–vectors.
mat2 rot(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

// SDF for an axis–aligned box centered at the origin with half–sizes b.
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, vec3(0.0))) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// The scene SDF: a tower built from cubes.
// The tower body is a rectangular box and on its top, eight small cube
// “battlements” (four corners and four mid–sides) are added.
// The whole structure is rotated about the y–axis by an angle driven by time.
float map(vec3 p) {
    // Rotate the scene coordinate (only xz) so that the tower appears to rotate.
    float towerAngle = time * 0.5;
    vec3 q = p;
    q.xz = rot(towerAngle) * q.xz;
    
    // --- Tower Body ---
    // Define the tower body as a box of half–dimensions (0.5, 1.0, 0.5)
    // centered at (0, towerCenterY, 0).
    const vec3 towerHalfSize = vec3(0.5, 1.0, 0.5);
    const float towerCenterY = 1.0;
    float towerTop = towerCenterY + towerHalfSize.y;
    float dTower = sdBox(q - vec3(0.0, towerCenterY, 0.0), towerHalfSize);
    
    // --- Battlements ---
    // Define small battlement cubes to sit on top of the tower.
    float bSize = 0.2;   // half–size in x and z
    float bHeight = 0.2; // full height of battlement cube
    vec3 battHalf = vec3(bSize, bHeight * 0.5, bSize);
    // The battlement cubes are centered at y = towerTop + (bHeight/2)
    float battY = towerTop + bHeight * 0.5;
    
    // Define eight battlement positions: four at the corners and four at the mid–sides.
    vec3 offsets[8];
    offsets[0] = vec3( towerHalfSize.x - bSize, battY,  towerHalfSize.z - bSize);
    offsets[1] = vec3(-towerHalfSize.x + bSize, battY,  towerHalfSize.z - bSize);
    offsets[2] = vec3( towerHalfSize.x - bSize, battY, -towerHalfSize.z + bSize);
    offsets[3] = vec3(-towerHalfSize.x + bSize, battY, -towerHalfSize.z + bSize);
    offsets[4] = vec3( 0.0,                    battY,  towerHalfSize.z - bSize);
    offsets[5] = vec3( towerHalfSize.x - bSize, battY,  0.0);
    offsets[6] = vec3(-towerHalfSize.x + bSize, battY,  0.0);
    offsets[7] = vec3( 0.0,                    battY, -towerHalfSize.z + bSize);
    
    // Compute the SDF for all battlements (using union = min).
    float dBattlement = 1e5;
    for (int i = 0; i < 8; i++) {
        float d = sdBox(q - offsets[i], battHalf);
        dBattlement = min(dBattlement, d);
    }
    
    // The scene is the union of the tower body and the battlements.
    return min(dTower, dBattlement);
}

// Standard raymarching: step along the ray until we hit the surface or exceed a max distance.
float raymarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    for (int i = 0; i < 100; i++) {
        float d = map(ro + rd * t);
        if(d < 0.001) break;
        t += d;
        if(t > 100.0) break;
    }
    return t;
}

// Estimate the surface normal at point p by finite differences.
vec3 calcNormal(vec3 p) {
    float eps = 0.001;
    return normalize(vec3(
        map(p + vec3(eps, 0.0, 0.0)) - map(p - vec3(eps, 0.0, 0.0)),
        map(p + vec3(0.0, eps, 0.0)) - map(p - vec3(0.0, eps, 0.0)),
        map(p + vec3(0.0, 0.0, eps)) - map(p - vec3(0.0, 0.0, eps))
    ));
}

void main() {
    // Compute normalized pixel coordinates (with y scaled by the fixed resolution).
    vec2 uv = (gl_FragCoord.xy - 0.5 * resolution.xy) / resolution.y;
    
    // --- Camera Setup ---
    vec3 ro = vec3(3.0, 2.0, 3.0);      // camera (ray origin) position
    vec3 target = vec3(0.0, 1.0, 0.0);    // point to look at
    vec3 forward = normalize(target - ro);
    vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
    vec3 up = cross(right, forward);
    // Form the ray direction using a simple perspective projection.
    vec3 rd = normalize(uv.x * right + uv.y * up + 1.5 * forward);
    
    // --- Raymarching ---
    float t = raymarch(ro, rd);
    vec3 pos = ro + rd * t;
    
    vec3 col;
    if(t > 100.0) {
        // No hit – show a sky–like background (a simple gradient).
        col = vec3(0.7, 0.9, 1.0) - rd.y * 0.5;
    } else {
        // Surface hit: compute lighting.
        vec3 n = calcNormal(pos);
        // A light direction that rotates (in the xz–plane) over time.
        vec3 lightDir = normalize(vec3(cos(time), 0.8, sin(time)));
        float diff = max(dot(n, lightDir), 0.0);
        // Simple specular highlight.
        vec3 ref = reflect(-lightDir, n);
        float spec = pow(max(dot(ref, -rd), 0.0), 16.0);
        // Combine ambient, diffuse, and specular terms.
        col = vec3(0.2) + diff * vec3(0.8, 0.7, 0.6) + spec * vec3(1.0);
    }
    
    gl_FragColor = vec4(col, 1.0);
}

