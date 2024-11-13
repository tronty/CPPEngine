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
        float frequency = 12.0;
        float amplitude = 0.15;

        for (int i = 1; i <= 6; i++)
        {
            float f = frequency * float(i);
            float a = amplitude / float(i);
            craterPattern += a * sin(f * theta) * sin(f * phi);
        }

        // Normalize crater pattern
        craterPattern = clamp(craterPattern, -1.0, 1.0);

        // Simulate color variations for Mercury
        vec3 baseColor = vec3(0.6, 0.6, 0.6); // Grayish base color
        vec3 darkColor = vec3(0.3, 0.3, 0.3); // Darker color for craters

        // Map the crater pattern to Mercury-like colors
        vec3 mercuryColor = mix(baseColor, darkColor, craterPattern * 0.5 + 0.5);

        color = vec4(mercuryColor, 1.0);
    }

    gl_FragColor = color;
}

