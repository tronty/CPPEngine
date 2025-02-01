const float PI = 3.141592653589793;
const float TWO_PI = 6.283185307179586;

// Moon parameters
const float moonRadius = 0.5;
const vec3 moonColor = vec3(0.7, 0.7, 0.7);
const vec3 craterColor = vec3(0.5, 0.5, 0.5);

// Crater parameters
const int numCraters = 10; // Base number of craters per tile
const float craterRadius = 0.05; // Smaller craters for higher density
const float craterDepth = 0.02;

// Rotation parameters
float rotationAngle = 0.0;

// Function to generate a random float
float random(vec2 st) {
    return fract(sin(dot(st, vec2(12.9898, 78.233))) * 43758.5453);
}

// Function to generate a procedural crater
float crater(vec2 uv, vec2 center, float radius, float depth) {
    float dist = length(uv - center);
    return smoothstep(radius, radius - depth, dist);
}

// Function to generate multiple craters in a tiled grid
float generateCraters(vec2 uv) {
    float craterEffect = 0.0;
    vec2 tileSize = vec2(0.2); // Size of each tile
    vec2 tileOffset = vec2(0.1); // Offset to avoid visible grid lines

    // Loop over a 5x5 grid (25 tiles) to simulate 30x more craters
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            vec2 tileUV = uv + vec2(i, j) * tileSize + tileOffset;
            for (int k = 0; k < numCraters; k++) {
                vec2 center = vec2(random(vec2(i, j) + vec2(k, 0.0)), random(vec2(i, j) + vec2(k, 1.0)));
                float radius = craterRadius * random(vec2(i, j) + vec2(k, 2.0));
                float depth = craterDepth * random(vec2(i, j) + vec2(k, 3.0));
                craterEffect += crater(tileUV, center, radius, depth);
            }
        }
    }

    return clamp(craterEffect, 0.0, 1.0); // Clamp to avoid overbrightening
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0); // Normalize coordinates
    uv = uv * 2.0 - 1.0; // Center coordinates

    // Apply rotation
    rotationAngle += 0.01; // Increment rotation angle
    float cosAngle = cos(rotationAngle);
    float sinAngle = sin(rotationAngle);
    uv = vec2(uv.x * cosAngle - uv.y * sinAngle, uv.x * sinAngle + uv.y * cosAngle);

    // Check if the pixel is inside the Moon
    float dist = length(uv);
    if (dist > moonRadius) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Background color
        return;
    }

    // Generate craters
    float craterEffect = generateCraters(uv);

    // Mix Moon color with crater color
    vec3 color = mix(moonColor, craterColor, craterEffect);

    gl_FragColor = vec4(color, 1.0);
}

