vec3 castlePos=vec3(0.5);
// A simple 2D rotation matrix (rotates vectors in the xz–plane).
mat2 rot(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

// SDF for an axis-aligned box centered at the origin with half–sizes b.
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, vec3(0.0))) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// SDF for a capped vertical cylinder (or “round tower”).
// The cylinder is centered at the origin and extends from -h to h in y.
float sdCylinder(vec3 p, float r, float h) {
    vec2 d = vec2(length(p.xz) - r, abs(p.y) - h);
    return min(max(d.x, d.y), 0.0) + length(max(d, vec2(0.0)));
}

// The scene SDF: a rectangular castle with round towers at each corner.
float map(vec3 p) {
    // Translate p so that the castle is centered at castlePos.
    p -= castlePos;
    // Rotate the entire scene (castle) about the y–axis.
    float castleAngle = time * 0.3;
    p.xz = rot(castleAngle) * p.xz;

    // --- Castle Walls ---
    // We build the walls as a rectangular "frame" (an outer box minus an inner box)
    // The wall extends vertically from y = 0 to castleHeight.
    float castleHeight = 1.0;
    // Outer extents of the wall (half–sizes in x and z).
    vec2 outerHalf = vec2(1.5, 1.0);
    // Inner extents define the courtyard (the wall thickness is outerHalf - innerHalf).
    vec2 innerHalf = vec2(1.2, 0.7);

    // Center the wall vertically by shifting p so that y=castleHeight/2 is at the center.
    vec3 pa = p - vec3(0.0, castleHeight * 0.5, 0.0);
    float dOuter = sdBox(pa, vec3(outerHalf.x, castleHeight * 0.5, outerHalf.y));
    float dInner = sdBox(pa, vec3(innerHalf.x, castleHeight * 0.5, innerHalf.y));
    // Subtract the inner box from the outer box to get a frame.
    float dWall = max(dOuter, -dInner);

    // --- Round Towers ---
    // Define parameters for the towers.
    float towerHeight = 1.5;
    float towerRadius = 0.3;
    float towerCenterY = towerHeight * 0.5; // so towers rise from y=0 to y=towerHeight

    // Position the four towers at the corners of the outer wall.
    vec3 towerPositions[4];
    towerPositions[0] = vec3( outerHalf.x, towerCenterY,  outerHalf.y);
    towerPositions[1] = vec3(-outerHalf.x, towerCenterY,  outerHalf.y);
    towerPositions[2] = vec3( outerHalf.x, towerCenterY, -outerHalf.y);
    towerPositions[3] = vec3(-outerHalf.x, towerCenterY, -outerHalf.y);

    float dTower = 1e5;
    for (int i = 0; i < 4; i++) {
        // Translate p into each tower’s local space.
        float d = sdCylinder(p - towerPositions[i], towerRadius, towerHeight * 0.5);
        dTower = min(dTower, d);
    }

    // The castle is the union of the wall and the towers.
    return min(dWall, dTower);
}

// Raymarching: step along the ray until the surface is reached or a max distance is exceeded.
float raymarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    for (int i = 0; i < 100; i++) {
        float d = map(ro + rd * t);
        if (d < 0.001) break;
        t += d;
        if (t > 100.0) break;
    }
    return t;
}

// Estimate the surface normal by finite differences.
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
    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
    
    // --- Camera Setup ---
    vec3 ro = vec3(3.0, 2.0, 3.0);    // camera position
    vec3 target = vec3(0.0, 0.5, 0.0);  // look–at target (roughly the castle center)
    vec3 forward = normalize(target - ro);
    vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
    vec3 up = cross(right, forward);
    // Build the ray direction (simple perspective projection).
    vec3 rd = normalize(uv.x * right + uv.y * up + 1.5 * forward);
    
    // --- Raymarching ---
    float t = raymarch(ro, rd);
    vec3 pos = ro + rd * t;
    
    vec3 col;
    if(t > 100.0) {
        // No hit – display a sky-like gradient.
        col = vec3(0.7, 0.9, 1.0) - rd.y * 0.5;
    } else {
        // Surface hit: compute lighting.
        vec3 n = calcNormal(pos);
        // A rotating light direction (rotating in the xz–plane).
        vec3 lightDir = normalize(vec3(cos(time), 0.8, sin(time)));
        float diff = max(dot(n, lightDir), 0.0);
        // Simple specular highlight.
        vec3 ref = reflect(-lightDir, n);
        float spec = pow(max(dot(ref, -rd), 0.0), 16.0);
        // Combine ambient, diffuse, and specular lighting.
        col = vec3(0.2) + diff * vec3(0.8, 0.7, 0.6) + spec * vec3(1.0);
    }
    
    gl_FragColor = vec4(col, 1.0);
}

