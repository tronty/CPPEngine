#ifdef GL_ES
precision mediump float;
#endif

//uniform vec2 resolution;
uniform vec3 u_cameraPos = vec3(0.0, 5.0, 10.0);  // Kameran sijainti (katsoo rakennusta)
uniform vec3 u_lightPos = vec3(5.0, 10.0, 5.0);   // Valonlähteen sijainti

// Helper functions
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)), 0.0) + length(max(d, 0.0));
}

// Union of two SDFs
float unionSDF(float d1, float d2) {
    return min(d1, d2);
}

// Building SDF
float buildingSDF(vec3 p) {
    // Repeat the building blocks in space to create floors
    float floorHeight = 3.0;  // Height of each floor
    vec3 buildingSize = vec3(1.0, floorHeight, 1.0);
    vec3 q = p;
    
    // Create multiple floors by repeating the Y axis
    q.y = mod(p.y, floorHeight * 2.0) - floorHeight;
    
    // Main building block (a single floor)
    float building = sdBox(q, buildingSize);
    
    // Create some windows on the front of the building using more boxes
    vec3 windowSize = vec3(0.2, 0.2, 0.1);
    vec3 windowPos = vec3(0.5, 0.5, 0.95); // Position of the window in front
    float window = sdBox(q - windowPos, windowSize);

    // Subtract the window from the building (carve out the window)
    return unionSDF(building, -window);
}

// Normal calculation
vec3 calculateNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        buildingSDF(p + e.xyy) - buildingSDF(p - e.xyy),
        buildingSDF(p + e.yxy) - buildingSDF(p - e.yxy),
        buildingSDF(p + e.yyx) - buildingSDF(p - e.yyx)
    ));
}

// Simple lighting function (Lambertian reflection)
float lighting(vec3 p, vec3 normal, vec3 lightPos) {
    vec3 lightDir = normalize(lightPos - p);
    return clamp(dot(normal, lightDir), 0.0, 1.0);
}

// Raymarching
float raymarch(vec3 ro, vec3 rd) {
    float depth = 0.0;
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + depth * rd;
        float d = buildingSDF(p);
        if (d < 0.001) break;
        depth += d;
        if (depth > 50.0) break;  // Max render distance
    }
    return depth;
}

void main() {
    vec2 uv = (xlv_TEXCOORD0.xy - resolution.xy * 0.5) / resolution.y;

    // Camera and ray direction
    vec3 ro = u_cameraPos;  // Camera position
    vec3 rd = normalize(vec3(uv, -1.0));  // Ray direction

    // Raymarching to find intersection with the building
    float depth = raymarch(ro, rd);
    vec3 p = ro + depth * rd;
    
    // Compute the normal and lighting
    vec3 normal = calculateNormal(p);
    float lightIntensity = lighting(p, normal, u_lightPos);

    // Output color
    vec3 color = vec3(0.2, 0.5, 0.7) * lightIntensity;  // Simple color
    gl_FragColor = vec4(color, 1.0);
}

