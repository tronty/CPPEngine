void main()
{
    // Fixed screen resolution
    const vec2 resolution = vec2(800.0, 600.0);

    // Normalize coordinates to range [-1, 1]
    vec2 uv = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y; // Correct aspect ratio

    // Compute distance from the center
    float radius = length(uv);

    // Discard fragments outside the circle (sphere projection)
    if (radius > 1.0)
    {
        discard;
    }

    // Compute the z-coordinate of the sphere's surface (before rotation)
    float z = sqrt(1.0 - radius * radius);

    // Sphere position before rotation
    vec3 spherePos = vec3(uv.x, uv.y, z);

    // Apply tilt rotation to the sphere around the x-axis
    float angle = radians(177.36); // Venus's axial tilt (retrograde rotation)
    mat3 rotation = mat3(
        1.0,        0.0,         0.0,
        0.0, cos(angle), -sin(angle),
        0.0, sin(angle),  cos(angle)
    );
    spherePos = rotation * spherePos;

    // Compute spherical coordinates from the rotated position
    float phi = asin(spherePos.y);                // Latitude
    float theta = atan(spherePos.x, spherePos.z); // Longitude

    // Generate turbulent cloud patterns
    float noise = sin(theta * 30.0) * sin(phi * 30.0);
    float turbulence = sin(theta * 100.0 + sin(phi * 100.0)) * 0.1;
    float pattern = noise + turbulence;

    // Map the pattern to Venus-like colors
    vec3 venusColor = mix(vec3(0.95, 0.85, 0.7), vec3(0.9, 0.8, 0.65), pattern * 0.5 + 0.5);

    gl_FragColor = vec4(venusColor, 1.0);
}

