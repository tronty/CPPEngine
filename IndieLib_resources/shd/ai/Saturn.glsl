//Certainly! Below is a GLSL 1.3 fragment shader that renders a **tilted spherical Jupiter planet with horizontal turbulent cloud belts**. This shader does not use any `uniform` or `varying` variables, adhering to your requirements.

#if 0
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
    float angle = radians(25.0); // Tilt angle in degrees
    mat3 rotation = mat3(
        1.0,        0.0,         0.0,
        0.0, cos(angle), -sin(angle),
        0.0, sin(angle),  cos(angle)
    );
    spherePos = rotation * spherePos;

    // Compute spherical coordinates from the rotated position
    float phi = asin(spherePos.y);                // Latitude
    float theta = atan(spherePos.x, spherePos.z); // Longitude

    // Generate horizontal turbulent cloud belts using phi (latitude)
    float bands = sin(phi * 20.0);
    float turbulence = sin(theta * 40.0 + sin(phi * 60.0));
    float pattern = bands + turbulence * 0.2;

    // Map the pattern to Jupiter-like colors
    //vec3 planetColor = mix(vec3(1.0, 0.8, 0.6), vec3(0.8, 0.5, 0.2), pattern * 0.5 + 0.5);	// JupiterColor
    
    vec3 planetColor = mix(vec3(0.9, 0.85, 0.7), vec3(0.8, 0.75, 0.6), pattern * 0.5 + 0.5);	// SaturnColor
    //vec3 planetColor = mix(vec3(0.4, 0.7, 0.9), vec3(0.6, 0.8, 1.0), pattern * 0.5 + 0.5);	// UranusColor
    //vec3 planetColor = mix(vec3(0.1, 0.2, 0.7), vec3(0.2, 0.3, 0.9), pattern * 0.5 + 0.5);	// NeptunusColor
    
    gl_FragColor = vec4(planetColor, 1.0);
}
#elif 0
void main()
{
    // Fixed screen resolution
    const vec2 resolution = vec2(800.0, 600.0);

    // Normalize coordinates to range [-1, 1]
    vec2 uv = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y; // Correct aspect ratio

    // Save original uv for rings calculation
    vec2 uvOriginal = uv;

    // Compute distance from the center
    float radius = length(uv);

    // Compute the z-coordinate of the sphere's surface (before rotation)
    float z = sqrt(1.0 - radius * radius);

    // Sphere position before rotation
    vec3 spherePos = vec3(uv.x, uv.y, z);

    // Apply tilt rotation to the sphere around the x-axis
    float angle = radians(26.73); // Saturn's axial tilt
    mat3 rotation = mat3(
        1.0,        0.0,         0.0,
        0.0,  cos(angle), -sin(angle),
        0.0,  sin(angle),  cos(angle)
    );
    spherePos = rotation * spherePos;

    // Compute spherical coordinates from the rotated position
    float phi = asin(spherePos.y);                // Latitude
    float theta = atan(spherePos.x, spherePos.z); // Longitude

    // Generate horizontal turbulent cloud belts using phi (latitude)
    float bands = sin(phi * 20.0);
    float turbulence = sin(theta * 40.0 + sin(phi * 60.0));
    float pattern = bands + turbulence * 0.2;

    // Map the pattern to Saturn-like colors
    vec3 saturnColor = mix(vec3(0.9, 0.85, 0.7), vec3(0.8, 0.75, 0.6), pattern * 0.5 + 0.5);

    // Initialize the fragment color
    vec4 color = vec4(0.0);

    // Determine if the fragment is within the planet
    if (radius <= 1.0)
    {
        color = vec4(saturnColor, 1.0);
    }

    // Now compute the rings
    // Apply tilt rotation to the uv coordinates for the rings
    //float ringAngle = radians(26.73); // Same tilt as the planet
    float ringAngle = radians(100.0); // Same tilt as the planet
    mat2 ringRotation = mat2(
        cos(ringAngle), -sin(ringAngle),
        sin(ringAngle),  cos(ringAngle)
    );
    vec2 uvRings = ringRotation * uvOriginal;

    // Compute distance from center in rotated space
    float ringRadius = length(uvRings);

    // Define inner and outer radius of the rings
    float innerRadius = 1.1;
    float outerRadius = 1.8;

    // Determine if the fragment is within the rings
    if (ringRadius >= innerRadius && ringRadius <= outerRadius)
    {
        // Create ring pattern
        float ringPattern = fract(ringRadius * 20.0);
        ringPattern = smoothstep(0.45, 0.55, ringPattern);
        vec3 ringColor = mix(vec3(0.8, 0.8, 0.7), vec3(0.9, 0.9, 0.85), ringPattern);

        // Combine ring color with existing color using alpha blending
        float alpha = 0.7; // Adjust transparency of the rings
        color = mix(color, vec4(ringColor, 1.0), alpha);
    }

    gl_FragColor = color;
}
#elif 1
void main()
{
    // Fixed screen resolution
    const vec2 resolution = vec2(800.0, 600.0);

    // Normalize coordinates to range [-1, 1]
    vec2 uv = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y; // Correct aspect ratio

    // Save original uv for rings calculation
    vec2 uvOriginal = uv;

    // Compute distance from the center
    float radius = length(uv);

    // Compute the z-coordinate of the sphere's surface (before rotation)
    float z = sqrt(1.0 - radius * radius);

    // Sphere position before rotation
    vec3 spherePos = vec3(uv.x, uv.y, z);

    // Apply tilt rotation to the planet around the x-axis
    float planetTilt = radians(26.73); // Saturn's axial tilt
    mat3 planetRotation = mat3(
        1.0,          0.0,           0.0,
        0.0,  cos(planetTilt), -sin(planetTilt),
        0.0,  sin(planetTilt),  cos(planetTilt)
    );
    spherePos = planetRotation * spherePos;

    // Compute spherical coordinates from the rotated position
    float phi = asin(spherePos.y);                // Latitude
    float theta = atan(spherePos.x, spherePos.z); // Longitude

    // Generate horizontal turbulent cloud belts using phi (latitude)
    float bands = sin(phi * 20.0);
    float turbulence = sin(theta * 40.0 + sin(phi * 60.0));
    float pattern = bands + turbulence * 0.2;

    // Map the pattern to Saturn-like colors
    vec3 saturnColor = mix(vec3(0.9, 0.85, 0.7), vec3(0.8, 0.75, 0.6), pattern * 0.5 + 0.5);

    // Initialize the fragment color
    vec4 color = vec4(0.0);

    // Determine if the fragment is within the planet
    if (radius <= 1.0)
    {
        color = vec4(saturnColor, 1.0);
    }

    // Now compute the rings
    // Apply tilt rotation to the uv coordinates for the rings
    float ringTilt = radians(85.0); // Rings tilted by 85 degrees
    mat2 ringRotation = mat2(
        cos(ringTilt), -sin(ringTilt),
        sin(ringTilt),  cos(ringTilt)
    );
    vec2 uvRings = ringRotation * uvOriginal;

    // Compute distance from center in rotated space
    float ringRadius = length(uvRings);

    // Define inner and outer radius of the rings
    float innerRadius = 1.1;
    float outerRadius = 1.8;

    // Determine if the fragment is within the rings
    if (ringRadius >= innerRadius && ringRadius <= outerRadius)
    {
        // Create ring pattern
        float ringPattern = fract(ringRadius * 20.0);
        ringPattern = smoothstep(0.45, 0.55, ringPattern);
        vec3 ringColor = mix(vec3(0.8, 0.8, 0.7), vec3(0.9, 0.9, 0.85), ringPattern);

        // Combine ring color with existing color using alpha blending
        float alpha = 0.7; // Adjust transparency of the rings
        color = mix(color, vec4(ringColor, 1.0), alpha);
    }

    gl_FragColor = color;
}
#endif

