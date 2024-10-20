// Function to draw the road
float road(vec2 uv) {
    // Sine wave function to create the road curve
    float roadWidth = 0.2; // Half width of the road
    float frequency = 2.0; // Frequency of the road wave
    float speed = 1.0;     // Speed at which the road moves

    // Create a moving wave pattern for the road's centerline
    float curve = sin(uv.y * frequency + time * speed) * 0.2;

    // Check if the pixel is inside the road bounds
    return smoothstep(roadWidth, roadWidth - 0.01, abs(uv.x - curve));
}

void main() {
    // Normalized coordinates (range [-1, 1] in both x and y axes)
    vec2 uv = (xlv_TEXCOORD0.xy * 2.0 - resolution.xy) / resolution.y;

    // Road function output (returns 0 or 1)
    float roadMask = road(uv);

    // Road color (dark color for road, bright for surrounding)
    vec3 roadColor = mix(vec3(0.7, 0.7, 0.7), vec3(0.2, 0.2, 0.2), roadMask);

    // Output the final color
    gl_FragColor = vec4(roadColor, 1.0);
}

