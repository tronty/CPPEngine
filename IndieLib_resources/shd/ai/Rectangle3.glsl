#define PI 3.14159265359
#define TWO_PI 6.28318530718

// Function to generate noise
float noise(vec3 p) {
    return fract(sin(dot(p, vec3(12.9898, 78.233, 45.5432))) * 43758.5453);
}

// Function to rotate a point around the Y axis
vec3 rotateY(vec3 p, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return vec3(c * p.x + s * p.z, p.y, -s * p.x + c * p.z);
}
float sdCube(vec3 p) {
    // The cube is centered at the origin and has a size of 1.0 in all dimensions
    vec3 b = vec3(1.0); // Half-size of the cube (since the cube goes from -0.5 to 0.5 in each axis)
    
    // Calculate the distance to the cube
    vec3 d = abs(p) - b;
    
    // If the point is inside the cube, the distance is the maximum of the negative components
    // Otherwise, it's the length of the vector from the closest face to the point
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}
// Function to calculate the distance to a cube
float sdCube(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

// Function to calculate the normal of a cube
vec3 getNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        sdCube(p + e.xyy) - sdCube(p - e.xyy),
        sdCube(p + e.yxy) - sdCube(p - e.yxy),
        sdCube(p + e.yyx) - sdCube(p - e.yyx)
    ));
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0); // Assuming a 800x600 resolution
    uv = uv * 2.0 - 1.0; // Normalize to [-1, 1]

    // Camera setup
    vec3 ro = vec3(0.0, 0.0, 5.0); // Ray origin
    vec3 rd = normalize(vec3(uv, -1.0)); // Ray direction

    // Cube properties
    vec3 cubePos = vec3(0.0, 0.0, 0.0);
    vec3 cubeSize = vec3(1.0, 1.0, 1.0);
    float cubeRotation = mod(0.1 * float(gl_FragCoord.x + gl_FragCoord.y), TWO_PI); // Rotate based on screen position

    // Raymarching loop
    float t = 0.0;
    for (int i = 0; i < 64; i++) {
        vec3 p = ro + rd * t;
        p = rotateY(p - cubePos, cubeRotation) + cubePos; // Rotate the cube
        float d = sdCube(p - cubePos, cubeSize);
        if (d < 0.001) {
            // Calculate lighting
            vec3 normal = getNormal(p);
            vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0));
            float diff = max(dot(normal, lightDir), 0.0);
            float spec = pow(max(dot(reflect(-lightDir, normal), -rd), 0.0), 32.0);

            // Add noise for a noisy effect
            float n = noise(p * 10.0);
            vec3 color = vec3(0.5 + 0.5 * diff + 0.5 * spec) * n;

            gl_FragColor = vec4(color, 1.0);
            return;
        }
        t += d;
    }

    // Background color if no intersection
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}

