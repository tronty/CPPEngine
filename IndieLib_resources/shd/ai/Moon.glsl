void main()
{
    // Fixed screen resolution
    const vec2 resolution = vec2(800.0, 600.0);

    // Normalize coordinates to range [-1, 1]
    vec2 uv = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y; // Correct aspect ratio

    // Compute distance from the center
    float radius = length(uv);

    // Initialize the fragment color
    vec4 color = vec4(0.0);

    // Determine if the fragment is within the sphere
    if (radius <= 1.0)
    {
        // Compute the z-coordinate of the sphere's surface
        float z = sqrt(1.0 - radius * radius);

        // Sphere position
        vec3 spherePos = vec3(uv.x, uv.y, z);

        // Compute spherical coordinates
        float phi = asin(spherePos.y);                // Latitude
        float theta = atan(spherePos.x, spherePos.z); // Longitude

        // Simulate craters using noise functions
        float craterPattern = 0.0;
        float frequency = 15.0;
        float amplitude = 0.1;

        for (int i = 1; i <= 5; i++)
        {
            float f = frequency * float(i);
            float a = amplitude / float(i);
            craterPattern += a * sin(f * theta) * sin(f * phi);
        }

        // Normalize crater pattern
        craterPattern = clamp(craterPattern, -1.0, 1.0);

        // Map the pattern to grayscale colors to simulate the Moon's surface
        float brightness = 0.6 + craterPattern * 0.4;
        vec3 moonColor = vec3(brightness);

        color = vec4(moonColor, 1.0);
    }

    gl_FragColor = color;
}

