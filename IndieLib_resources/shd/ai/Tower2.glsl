// Replace this definition with your time value (or animate it externally)
#define TIME (10)

// ---------------------------------------------------------------------
// CONSTANTS & PARAMETERS
// ---------------------------------------------------------------------
const float PI           = 3.141592653589793;
const float EPSILON      = 0.001;
const float MAX_DIST     = 100.0;
const int   MAX_STEPS    = 100;
const vec2  RESOLUTION   = vec2(800.0, 600.0);

// ------------------------
// Parameters for the round tower
// ------------------------
const float TOWER_HEIGHT = 2.0;
const float TOWER_RADIUS = 1.0;
const float BATT_SIZE    = 0.3;

// ------------------------
// Parameters for the castle wall (a rectangular ring)
// ------------------------
const float castleWallHeight = 1.2;
const vec2  castleOuterHalf  = vec2(4.0, 4.0);  // half–dimensions of the outer rectangle
const vec2  castleInnerHalf  = vec2(3.0, 3.0);  // half–dimensions of the inner courtyard

// ---------------------------------------------------------------------
// HELPER FUNCTIONS
// ---------------------------------------------------------------------

// Rotate a 3D vector about the Y–axis
vec3 rotateY(in vec3 p, in float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return vec3(c * p.x - s * p.z, p.y, s * p.x + c * p.z);
}

// SDF for an axis–aligned box (cube) centered at the origin with half–sizes 'b'
float boxSDF(in vec3 p, in vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, vec3(0.0))) + min(max(max(d.x, d.y), d.z), 0.0);
}

// SDF for a finite vertical cylinder (centered at the origin)
// of radius 'r' and half–height 'h'
float sdCylinder(in vec3 p, in float r, in float h) {
    vec2 d = vec2(length(p.xz) - r, abs(p.y) - h);
    return min(max(d.x, d.y), 0.0) + length(max(d, vec2(0.0)));
}

// SDF for a horizontal plane at y = 0
float planeSDF(in vec3 p) {
    return p.y;
}

// ---------------------------------------------------------------------
// SDF DEFINITIONS
// ---------------------------------------------------------------------

// SDF for a round tower with battlements.
// The tower body is a vertical cylinder (with center at y = TOWER_HEIGHT/2).
// Twelve battlement cubes are arranged evenly around the top.
// The entire tower rotates (via an inverse rotation on the sample point).
float towerSDF(in vec3 p) {
    // Rotate the tower by TIME (apply inverse rotation so the tower appears to spin)
    p = rotateY(p, -TIME);
    
    // Tower body: a vertical cylinder with center shifted upward
    vec3 pBody = p - vec3(0.0, TOWER_HEIGHT * 0.5, 0.0);
    float dBody = sdCylinder(pBody, TOWER_RADIUS, TOWER_HEIGHT * 0.5);
    
    // Battlements: place small cubes evenly around the top edge
    float dBatt = 1000.0;
    const int nBatt = 12;
    for (int i = 0; i < nBatt; i++) {
        float angle = float(i) * (2.0 * PI / float(nBatt));
        vec3 posBatt = vec3(TOWER_RADIUS * cos(angle),
                            TOWER_HEIGHT + BATT_SIZE * 0.5,
                            TOWER_RADIUS * sin(angle));
        float dCube = boxSDF(p - posBatt, vec3(BATT_SIZE * 0.5));
        dBatt = min(dBatt, dCube);
    }
    return min(dBody, dBatt);
}

// SDF for the castle wall – a rectangular ring extruded vertically.
// The wall is defined as the difference between an outer and an inner box.
float castleWallSDF(in vec3 p) {
    vec3 cp = p;
    cp.y -= castleWallHeight * 0.5; // center the wall vertically
    vec3 outerHalf = vec3(castleOuterHalf, castleWallHeight * 0.5);
    vec3 innerHalf = vec3(castleInnerHalf, castleWallHeight * 0.5);
    float dOuter = boxSDF(cp, outerHalf);
    float dInner = boxSDF(cp, innerHalf);
    // Using SDF difference: the wall is the region inside the outer box but outside the inner box.
    return max(dOuter, -dInner);
}

// SDF for the full castle: the union of the castle wall and four round towers
// positioned at the four corners of the outer wall.
float castleSDF(in vec3 p) {
    float dWall = castleWallSDF(p);
    
    float dTower = 1000.0;
    // Define tower centers at the four corners of the castle wall (in the xz–plane)
    vec2 offsets[4];
    offsets[0] = vec2(castleOuterHalf.x,  castleOuterHalf.y);
    offsets[1] = vec2(castleOuterHalf.x, -castleOuterHalf.y);
    offsets[2] = vec2(-castleOuterHalf.x, castleOuterHalf.y);
    offsets[3] = vec2(-castleOuterHalf.x, -castleOuterHalf.y);
    
    for (int i = 0; i < 4; i++) {
        vec3 towerPos = vec3(offsets[i], 0.0);
        float d = towerSDF(p - towerPos);
        dTower = min(dTower, d);
    }
    return min(dWall, dTower);
}

// SDF for the entire scene: the castle (walls + towers) and a ground plane.
float sceneSDF(in vec3 p) {
    return min(castleSDF(p), planeSDF(p));
}

// Estimate the normal at point 'p' using finite differences.
vec3 estimateNormal(in vec3 p) {
    float eps = 0.001;
    return normalize(vec3(
        sceneSDF(p + vec3(eps, 0.0, 0.0)) - sceneSDF(p - vec3(eps, 0.0, 0.0)),
        sceneSDF(p + vec3(0.0, eps, 0.0)) - sceneSDF(p - vec3(0.0, eps, 0.0)),
        sceneSDF(p + vec3(0.0, 0.0, eps)) - sceneSDF(p - vec3(0.0, 0.0, eps))
    ));
}

// ---------------------------------------------------------------------
// MAIN RENDERING FUNCTION
// ---------------------------------------------------------------------
void main() {
    // Compute normalized pixel coordinates (with aspect–ratio preserved)
    vec2 uv = (gl_FragCoord.xy - 0.5 * RESOLUTION) / RESOLUTION.y;
    
    // Set up a simple camera.
    // The castle is centered at the origin; the camera is placed so that the entire castle is in view.
    vec3 ro = vec3(0.0, 3.0, -12.0);  // Camera (ray origin) position
    vec3 ta = vec3(0.0, 1.0, 0.0);      // Look–at target (roughly the castle center)
    vec3 cw = normalize(ta - ro);
    vec3 cp = vec3(0.0, 1.0, 0.0);
    vec3 cu = normalize(cross(cw, cp));
    vec3 cv = cross(cu, cw);
    vec3 rd = normalize(cw + uv.x * cu + uv.y * cv);
    
    // Raymarching loop
    float t = 0.0;
    int steps;
    for (steps = 0; steps < MAX_STEPS; steps++) {
        vec3 pos = ro + rd * t;
        float d = sceneSDF(pos);
        if (d < EPSILON) break;
        t += d;
        if (t > MAX_DIST) break;
    }
    
    vec3 col;
    if (t < MAX_DIST) {
        // We hit an object: compute the point and estimated normal.
        vec3 pos = ro + rd * t;
        vec3 n   = estimateNormal(pos);
        
        // Rotating specular light (the light direction rotates over time)
        vec3 lightDir = normalize(vec3(sin(TIME * 1.3), 0.8, cos(TIME * 1.3)));
        float diff = max(dot(n, lightDir), 0.0);
        vec3 refl = reflect(-lightDir, n);
        float spec = pow(max(dot(refl, -rd), 0.0), 32.0);
        
        // Combine ambient, diffuse, and specular lighting
        col = vec3(0.2) + vec3(0.5) * diff + vec3(1.0) * spec;
    } else {
        // No hit: render a sky–like gradient.
        col = vec3(0.7, 0.9, 1.0) - rd.y * 0.5;
    }
    
    gl_FragColor = vec4(col, 1.0);
}

