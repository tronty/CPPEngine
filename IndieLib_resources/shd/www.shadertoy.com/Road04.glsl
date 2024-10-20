// Function to generate hilly ground
float hills(vec2 uv) {
    float hillHeight = 0.5; // Amplitude for steeper hills
    float frequency = 10.0; // Frequency for more hills

    // Generate a sine wave to simulate steeper and more frequent hills
    return sin(uv.y * frequency) * hillHeight;
}

// Function to generate dashed road and stripes
vec3 road(vec2 uv) {
    float roadWidth = 0.5;
    float roadFrequency = 10.0;
    float roadSpeed = 1.0;

    // Road curvature, animated over time
    float roadCurve = sin(uv.y * roadFrequency + time * roadSpeed) * 0.2;

    // Dashed stripes parameters
    float stripeWidth = 0.01;          // Stripe thickness
    float stripeLength = 0.2;          // Length of each dash
    float dashPattern = step(0.5, mod(uv.y, stripeLength));  // Dashed pattern

    // Middle stripe (dashed)
    float middleStripe = step(abs(uv.x - roadCurve), stripeWidth) * dashPattern;

    // Border stripes (dashed)
    float borderWidth = 0.01;
    float leftBorder = step(abs(uv.x - roadCurve - roadWidth), borderWidth) * dashPattern;
    float rightBorder = step(abs(uv.x - roadCurve + roadWidth), borderWidth) * dashPattern;

    // Road color is dark, middle stripe is white, and border stripes are yellow
    vec3 roadColor = vec3(0.1, 0.1, 0.1);  // Dark road color
    vec3 middleStripeColor = vec3(1.0, 1.0, 1.0); // White middle stripe
    vec3 borderStripeColor = vec3(1.0, 1.0, 0.0); // Yellow border stripes

    // Combine road and stripe colors using masks
    vec3 finalRoadColor = mix(roadColor, middleStripeColor, middleStripe);
    finalRoadColor = mix(finalRoadColor, borderStripeColor, leftBorder + rightBorder);

    return finalRoadColor;
}

// Function to create noisy clouds
float noise(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float cloudLayer(vec2 uv) {
    // Layered noise clouds with movement
    float cloudPattern = noise(uv * 3.0 + vec2(time * 0.2, time * 0.1)) * 0.5 + 0.5;
    return smoothstep(0.4, 0.7, cloudPattern);
}

// Function to draw the sun
float sun(vec2 uv, vec2 sunPos, float sunRadius) {
    // Sun is a circle using smoothstep to create a soft edge
    return smoothstep(sunRadius, sunRadius - 0.01, length(uv - sunPos));
}

void main() {
    // Normalized coordinates (range [-1, 1] for both axes)
    vec2 uv = (xlv_TEXCOORD0.xy * 2.0 - resolution.xy) / resolution.y;

    // Define the sky color
    vec3 skyColor = vec3(0.2, 0.6, 0.9);    // Ordinary sky color (light blue)
    vec3 cloudColor = vec3(1.0, 0.8, 0.7);  // Light-colored noisy clouds
    vec3 groundColor = vec3(0.4, 0.3, 0.1); // Ground color (brownish)
    float horizonLine = 0.0;                // y=0 represents the horizon

    // Sky with noisy clouds
    vec3 finalColor = skyColor;
    float cloudMask = cloudLayer(uv);
    finalColor = mix(finalColor, cloudColor, cloudMask);

    // Sun on the horizon
    vec2 sunPos = vec2(0.0, 0.3);           // Sun positioned slightly above the horizon (center)
    float sunRadius = 0.15;                 // Sun size
    vec3 sunColor = vec3(1.0, 0.9, 0.5);    // Bright yellowish color for the sun

    // Mask for the sun, blending it softly with the sky
    float sunMask = sun(uv, sunPos, sunRadius);
    finalColor = mix(finalColor, sunColor, sunMask);

    // Add steeper and more frequent hilly ground under the horizon (uv.y < 0)
    if (uv.y < horizonLine) {
        float hillMask = hills(uv);            // Generate hills based on the y-coordinate
        uv.x += hillMask;                      // Offset x to simulate hills

        // Dark road follows the hills and adds dashed stripes
        vec3 roadColor = road(uv);             // Road with dashed middle and border stripes

        // Blend road on top of the hilly ground
        finalColor = mix(groundColor, roadColor, 1.0);
    }

    // Output the final color
    gl_FragColor = vec4(finalColor, 1.0);
}

