// GLSL 1.3 Fragment Shader - Procedural Tree Rendering
/*
#version 130

precision highp float;

uniform vec2 resolution;
uniform float time;
*/
#define PI 3.14159265359
#define EPSILON 0.001
#define FAR_PLANE 50.0

// Distance Function for a Cylinder (Tree Trunk)
float sdCylinder(vec3 p, float h, float r) {
    vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(r, h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

// Distance Function for a Sphere (Tree Leaves)
float sdSphere(vec3 p, float r) {
    return length(p) - r;
}

// Union Operation for Distance Fields
float opUnion(float d1, float d2) {
    return min(d1, d2);
}

// Map Function to Define the Scene
float map(vec3 p) {
    // Tree Trunk
    float trunk = sdCylinder(p - vec3(0.0, 1.0, 0.0), 1.5, 0.1);
    
    // Main Tree Leaves (Positioned above the trunk)
    vec3 leafPos = p - vec3(0.0, 2.5, 0.0);
    float leaves = sdSphere(leafPos, 0.75);
    
    // Additional Branches and Leaves
    for (int i = 0; i < 6; i++) {
        float angle = time * 0.5 + float(i) * (PI / 3.0);
        float branchHeight = 1.5 + float(i) * 0.3;
        vec3 branchOffset = vec3(cos(angle) * 0.75, branchHeight, sin(angle) * 0.75);
        float branch = sdCylinder(p - branchOffset, 0.75, 0.05);
        leaves = opUnion(leaves, branch);
        
        // Leaves for Each Branch
        for (int j = 0; j < 3; j++) {
            float leafAngle = float(j) * (2.0 * PI / 3.0);
            vec3 branchLeafOffset = branchOffset + vec3(cos(leafAngle) * 0.5, 0.75, sin(leafAngle) * 0.5);
            vec3 branchLeafPos = p - branchLeafOffset;
            float branchLeaves = sdSphere(branchLeafPos, 0.5);
            leaves = opUnion(leaves, branchLeaves);
        }
    }
    
    // Combine Trunk and Leaves
    return opUnion(trunk, leaves);
}

// Raymarching Function
float rayMarch(vec3 ro, vec3 rd) {
    float depth = 0.0;
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + depth * rd;
        float dist = map(p);
        if (dist < EPSILON) {
            return depth;
        }
        depth += dist;
        if (depth >= FAR_PLANE) {
            break;
        }
    }
    return FAR_PLANE;
}

// Lighting Calculation
vec3 getNormal(vec3 p) {
    vec2 e = vec2(0.01, 0.0);
    float d = map(p);
    vec3 n = vec3(
        map(p + e.xyy) - d,
        map(p + e.yxy) - d,
        map(p + e.yyx) - d
    );
    return normalize(n);
}

vec3 lighting(vec3 p, vec3 lightDir) {
    vec3 n = getNormal(p);
    float diff = max(dot(n, lightDir), 0.0);
    return vec3(0.2, 0.8, 0.2) * diff + vec3(0.1); // Green leaves with ambient light
}

void main() {
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    // Camera Setup
    vec3 ro = vec3(0.0, 1.0, -5.0);
    vec3 rd = normalize(vec3(uv, 1.5));

    // Light Direction
    vec3 lightDir = normalize(vec3(-0.5, 1.0, -0.5));

    // Raymarching
    float t = rayMarch(ro, rd);
    if (t < FAR_PLANE) {
        vec3 p = ro + t * rd;
        vec3 color = lighting(p, lightDir);
        gl_FragColor = vec4(color, 1.0);
    } else {
        gl_FragColor = vec4(0.6, 0.8, 1.0, 1.0); // Sky color
    }
}

