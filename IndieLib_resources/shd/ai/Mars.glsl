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

        // Simulate craters and terrain features using noise functions
        float terrainPattern = 0.0;
        float frequency = 10.0;
        float amplitude = 0.2;

        for (int i = 1; i <= 8; i++)
        {
            float f = frequency * pow(2.0, float(i));
            float a = amplitude / pow(1.5, float(i));
            terrainPattern += a * sin(f * theta + f * phi);
        }

        // Normalize terrain pattern
        terrainPattern = clamp(terrainPattern, -1.0, 1.0);

        // Simulate color variations for Mars
        vec3 baseColor = vec3(0.8, 0.4, 0.3); // Reddish base color
        vec3 darkColor = vec3(0.5, 0.25, 0.2); // Darker color for craters and valleys

        // Map the terrain pattern to Mars-like colors
        vec3 marsColor = mix(baseColor, darkColor, terrainPattern * 0.5 + 0.5);

        // Optional: Add polar caps
        float polarCaps = smoothstep(1.0, 0.8, abs(spherePos.y));
        marsColor = mix(vec3(1.0, 1.0, 1.0), marsColor, polarCaps);

        color = vec4(marsColor, 1.0);
    }

    gl_FragColor = color;
}

