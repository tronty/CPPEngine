// ChatGPT-o1-preview: 3D textureless, vector-based, bufferless house with chimney in a GLSL 1.3 fragment shader without uniform shader variables?

// Function to draw a simple box
float boxSDF(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// Function to create a gabled roof using a triangular prism SDF
float gableRoofSDF(vec3 p, vec3 b, float roofHeight) {
    float boxDist = boxSDF(p, vec3(b.x, b.y * roofHeight, b.z));
    vec2 q = vec2(length(p.xz) / b.x, abs(p.y) / roofHeight);
    float roofDist = q.x + q.y - 1.0;
    return min(boxDist, roofDist);
}

// Function to render the scene
float scene(vec3 p) {
    // Define the house size and chimney
    vec3 houseSize = vec3(1.0, 1.0, 1.0);
    float house = gableRoofSDF(p, houseSize, 0.5);
    
    // Chimney size and position
    vec3 chimneyPos = p - vec3(0.6, 0.8, 0.0);
    vec3 chimneySize = vec3(0.1, 0.3, 0.1);
    float chimney = boxSDF(chimneyPos, chimneySize);

    // Combine the house and chimney using min
    return min(house, chimney);
}

// Function to perform raymarching
float rayMarch(vec3 ro, vec3 rd) {
    float dO = 0.0;
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + rd * dO;
        float dS = scene(p);
        if (dS < 0.001) break;
        dO += dS;
    }
    return dO;
}

// Function to compute normal from distance field
vec3 calcNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        scene(p + e.xyy) - scene(p - e.xyy),
        scene(p + e.yxy) - scene(p - e.yxy),
        scene(p + e.yyx) - scene(p - e.yyx)
    ));
}

// Function to create a simple lighting effect
vec3 lighting(vec3 p, vec3 normal, vec3 lightDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    return vec3(0.8, 0.5, 0.3) * diff; // Light color with a brownish tint for the house
}

void main() {
    vec2 uv = (xlv_TEXCOORD0.xy - resolution.xy * 0.5) / resolution.y;
    vec3 ro = vec3(3.0 * sin(time * 0.1), 1.5, 3.0 * cos(time * 0.1));
    vec3 lookAt = vec3(0.0, 0.5, 0.0);
    vec3 fwd = normalize(lookAt - ro);
    vec3 right = normalize(cross(fwd, vec3(0.0, 1.0, 0.0)));
    vec3 up = cross(right, fwd);
    
    vec3 rd = normalize(fwd + uv.x * right + uv.y * up);

    // Perform raymarching
    float d = rayMarch(ro, rd);
    
    // Calculate color based on the result
    vec3 color = vec3(0.0);
    if (d < 100.0) {
        vec3 p = ro + rd * d;
        vec3 normal = calcNormal(p);
        vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0));
        color = lighting(p, normal, lightDir);
    }
    
    gl_FragColor = vec4(color, 1.0);
}
