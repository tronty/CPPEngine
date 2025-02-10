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

const vec3 boxSize = vec3(0.5); // the half–extents of our box

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
        float d = sdBox(pos, boxSize); // now both parameters are supplied
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
    float dx = sdBox(p + vec3(eps, 0.0, 0.0), boxSize) - sdBox(p - vec3(eps, 0.0, 0.0), boxSize);
    float dy = sdBox(p + vec3(0.0, eps, 0.0), boxSize) - sdBox(p - vec3(0.0, eps, 0.0), boxSize);
    float dz = sdBox(p + vec3(0.0, 0.0, eps), boxSize) - sdBox(p - vec3(0.0, 0.0, eps), boxSize);
    return normalize(vec3(dx, dy, dz));
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
    float t = rayMarch(ro, rd);
    vec3 pos = ro + rd * t;
    
    vec3 col;
    if (t > 100.0) {
        // No hit – display a sky-like gradient.
        col = vec3(0.7, 0.9, 1.0) - rd.y * 0.5;
    } else {
        // Surface hit: compute lighting.
        vec3 n = estimateNormal(pos);
        
        // Light direction (rotating in the xz–plane).
        vec3 lightDir = normalize(vec3(cos(iTime), 0.8, sin(iTime)));
        
        // Diffuse lighting.
        float diff = max(dot(n, lightDir), 0.0);
        
        // Specular lighting.
        vec3 viewDir = normalize(ro - pos); // Direction from surface to camera.
        vec3 reflectDir = reflect(-lightDir, n);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        
        // Combine ambient, diffuse, and specular lighting.
        vec3 ambient = vec3(0.2);
        vec3 diffuse = diff * vec3(0.8, 0.7, 0.6);
        vec3 specular = spec * vec3(1.0);
        
        col = ambient + diffuse + specular;
    }
    
    // Output the final color.
    gl_FragColor = vec4(col, 1.0);
}

