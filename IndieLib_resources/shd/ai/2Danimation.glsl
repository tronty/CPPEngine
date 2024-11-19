// o1-preview: 2D textureless, vector-based, bufferless color filled animation in a GLSL 1.3 fragment shader without uniform shader variables?

#ifdef GL_ES
precision mediump float;
#endif

// Function to draw a circle at position `center` with radius `r`
float drawCircle(vec2 uv, vec2 center, float r) {
    return smoothstep(r, r + 0.01, length(uv - center)); // Antialiased circle edge
}

void main() {
    // Normalize the fragment coordinates to be in range [0, 1]
    vec2 uv = xlv_TEXCOORD0.xy / resolution.xy;

    // Ensure the aspect ratio is maintained by scaling the y-coordinate
    uv.y *= resolution.y / resolution.x;

    // Animation parameters: make the circle move horizontally over time
    //float time = time * 0.5;
    vec2 circleCenter = vec2(0.5 + 0.4 * sin(time * 0.5), 0.5); // Circle's center moves horizontally

    // Draw the circle using the `drawCircle` function
    float circle = drawCircle(uv, circleCenter, 0.2); // Circle with a radius of 0.2

    // Define base colors for the background and the circle
    vec3 backgroundColor = vec3(0.1, 0.1, 0.3); // Dark blue background
    vec3 circleColor = vec3(0.9, 0.6, 0.2); // Orange circle

    // Smooth color transitions over time
    vec3 animatedColor = mix(circleColor, vec3(0.2, 0.6, 0.9), 0.5 + 0.5 * sin(time));

    // Final color: blend the circle and the background
    vec3 color = mix(backgroundColor, animatedColor, circle);

    // Set the final fragment color
    gl_FragColor = vec4(color, 1.0);
}

