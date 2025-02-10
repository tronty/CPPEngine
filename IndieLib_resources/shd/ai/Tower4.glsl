vec3 castlePos=vec3(0.5);
// ---------------------------------------------------------------------------
// 2D Rotation Matrix (rotates a vec2 in the XZ–plane)
mat2 rot(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s,
                s,  c);
}

// ---------------------------------------------------------------------------
// Standard SDF for an axis–aligned box centered at the origin with half–extents b.
float sdBox(in vec3 p, in vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, vec3(0.0))) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// ---------------------------------------------------------------------------
// Tower SDF: A rectangular tower with battlements.
// The tower body is a box of half–size (0.3,0.5,0.3) centered at (0,0.5,0)
// and battlements are created by unioning several small cubes along the top edge.
float towerSDF(in vec3 p) {
    // Tower body
    float dBody = sdBox(p - vec3(0.0, 0.5, 0.0), vec3(0.3, 0.5, 0.3));
    
    // Battlements: Place small cubes along the perimeter of an 8x8 grid on the top.
    float dBatt = 1e6;
    for (int ix = 0; ix < 8; ix++) {
        for (int iz = 0; iz < 8; iz++) {
            // Only place cubes along the outer ring.
            if (ix != 0 && ix != 7 && iz != 0 && iz != 7) continue;
            
            // Compute a base grid coordinate (centered about 0)
            float spacing = 0.08;
            float centerX = (float(ix) - 3.5) * spacing;
            float centerZ = (float(iz) - 3.5) * spacing;
            
            // Push the battlement cubes outward (overhanging the tower edge)
            float overhang = 0.35;
            if (ix == 0)      centerX = -overhang;
            if (ix == 7)      centerX =  overhang;
            if (iz == 0)      centerZ = -overhang;
            if (iz == 7)      centerZ =  overhang;
            
            // Each battlement cube is centered at (centerX, 1.05, centerZ)
            vec3 cubeCenter = vec3(centerX, 1.05, centerZ);
            float dCube = sdBox(p - cubeCenter, vec3(0.05));
            dBatt = min(dBatt, dCube);
        }
    }
    return min(dBody, dBatt);
}

// ---------------------------------------------------------------------------
// Castle SDF: Builds a quad (square) castle by unioning a wall and four towers.
// The castle is rotated as a whole by time.
float castleSDF(in vec3 p) {
    // Translate p so that the castle is centered at castlePos.
    p -= castlePos;
    // Rotate the entire castle about the Y–axis.
    vec3 q = p;
    q.xz = rot(time) * q.xz;
    
    // ----- Castle Walls -----
    // Define four wall segments as elongated boxes.
    // Each wall is centered at y=0.5 (so it sits on the ground) and extends horizontally.
    float dWallN = sdBox(q - vec3(0.0, 0.5,  1.0), vec3(1.0, 0.5, 0.1)); // North wall
    float dWallS = sdBox(q - vec3(0.0, 0.5, -1.0), vec3(1.0, 0.5, 0.1)); // South wall
    float dWallE = sdBox(q - vec3( 1.0, 0.5, 0.0), vec3(0.1, 0.5, 1.0)); // East wall
    float dWallW = sdBox(q - vec3(-1.0, 0.5, 0.0), vec3(0.1, 0.5, 1.0)); // West wall
    float dWalls = min(min(dWallN, dWallS), min(dWallE, dWallW));
    
    // ----- Towers at the Four Corners -----
    // Define the four corner positions for the towers.
    float dTowers = 1e6;
    vec2 corners[4];
    corners[0] = vec2( 1.0,  1.0);
    corners[1] = vec2( 1.0, -1.0);
    corners[2] = vec2(-1.0,  1.0);
    corners[3] = vec2(-1.0, -1.0);
    for (int i = 0; i < 4; i++) {
        // Translate the coordinate system into the local space of each tower.
        vec3 tp = q - vec3(corners[i].x, 0.0, corners[i].y);
        float dT = towerSDF(tp);
        dTowers = min(dTowers, dT);
    }
    
    // The castle is the union of the walls and the towers.
    return min(dWalls, dTowers);
}

// ---------------------------------------------------------------------------
// Raymarching routine: marches from ray origin (ro) along direction (rd)
// until the SDF is below a small threshold or a maximum distance is exceeded.
float rayMarch(in vec3 ro, in vec3 rd) {
    float t = 0.0;
    const int MAX_STEPS = 100;
    const float MAX_DIST = 100.0;
    const float EPSILON = 0.001;
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 pos = ro + rd * t;
        float d = castleSDF(pos);
        if (d < EPSILON) break;
        t += d;
        if (t > MAX_DIST) break;
    }
    return t;
}

// ---------------------------------------------------------------------------
// Estimate the normal at a point by finite differences.
vec3 estimateNormal(in vec3 p) {
    const float eps = 0.001;
    float dx = castleSDF(p + vec3(eps, 0.0, 0.0)) - castleSDF(p - vec3(eps, 0.0, 0.0));
    float dy = castleSDF(p + vec3(0.0, eps, 0.0)) - castleSDF(p - vec3(0.0, eps, 0.0));
    float dz = castleSDF(p + vec3(0.0, 0.0, eps)) - castleSDF(p - vec3(0.0, 0.0, eps));
    return normalize(vec3(dx, dy, dz));
}
#if 0
// ---------------------------------------------------------------------------
// Main Fragment Shader
void main(void) {
    // Compute normalized pixel coordinates (uv) with aspect correction.
    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
    
    // ----- Camera Setup -----
    // Position the camera at (0,2,5) looking toward (0,0.5,0).
    vec3 ro = vec3(0.0, 2.0, 5.0);
    vec3 target = vec3(0.0, 0.5, 0.0);
    vec3 forward = normalize(target - ro);
    vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
    vec3 up = cross(right, forward);
    // Construct the ray direction.
    vec3 rd = normalize(uv.x * right + uv.y * up + 1.5 * forward);
    
    // ----- Raymarching -----
    float t = rayMarch(ro, rd);
    vec3 pos = ro + rd * t;
    vec3 normal = estimateNormal(pos);
    
    // ----- Lighting -----
    // Rotating light direction (rotates in the XZ–plane, with a slight Y–offset).
    vec3 lightDir = normalize(vec3(sin(time * 1.3), 0.7, cos(time * 1.3)));
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Phong) component.
    vec3 viewDir = normalize(ro - pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    
    // Combine ambient, diffuse, and specular lighting.
    vec3 col = vec3(0.1)           // ambient
             + diff * vec3(0.6)    // diffuse
             + spec * vec3(1.0);   // specular
    
    // Optional: Add a bit of distance–based fog.
    float fog = exp(-0.02 * t * t);
    col *= fog;
    
    // Output the final color.
    gl_FragColor = vec4(col, 1.0);
}
#elif 1
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
    float t = rayMarch(ro, rd);
    vec3 pos = ro + rd * t;
    
    vec3 col;
    if(t > 100.0) {
        // No hit – display a sky-like gradient.
        col = vec3(0.7, 0.9, 1.0) - rd.y * 0.5;
    } else {
        // Surface hit: compute lighting.
        vec3 n = estimateNormal(pos);
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
#endif

