// Simplex noise function
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x * 34.0) + 1.0) * x); }
float snoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    vec2 i = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);
    vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod289(i);
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));
    vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

// Generate turbulence for cloud belts
float turbulence(vec2 p, float size) {
    float value = 0.0;
    float initialSize = size;
    while (size >= 1.0) {
        value += abs(snoise(p / size)) * size;
        size /= 2.0;
    }
    return value / initialSize;
}

// Generate cloud belts with turbulent edges
float cloudBelts(vec2 uv) {
    float belts = 0.0;
    float frequency = 10.0; // Controls the number of belts
    float amplitude = 0.5;  // Controls the intensity of the belts

    // Create band-like structures using sine waves
    float baseBelts = sin(uv.y * frequency) * amplitude;

    // Add turbulence to the edges of the belts
    float edgeTurbulence = turbulence(uv * vec2(1.0, 4.0) + vec2(time * 0.1), 16.0) * 0.2;
    belts = baseBelts + edgeTurbulence;

    // Smoothstep to sharpen the belts and add turbulence to the edges
    belts = smoothstep(0.3, 0.7, abs(belts));

    return belts;
}

// Generate the Great Red Spot
float greatRedSpot(vec2 uv) {
    vec2 spotCenter = vec2(0.7, 0.5); // Position of the Great Red Spot
    float spotRadius = 0.1; // Size of the spot
    float dist = distance(uv, spotCenter);
    return smoothstep(spotRadius, spotRadius - 0.02, dist);
}

void main() {
    // Normalize coordinates to [-1, 1]
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y; // Correct aspect ratio

    // Rotate the planet over time
    float angle = time * 0.1; // Rotation speed
    mat2 rotation = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    uv = rotation * uv;

    // Map to spherical coordinates
    float radius = length(uv);
    if (radius > 1.0) {
        gl_FragColor = vec4(0.0); // Discard pixels outside the sphere
        return;
    }
    vec2 sphereUV = vec2(atan(uv.y, uv.x) / 3.1416, radius);

    // Generate cloud belts with turbulent edges
    float belts = cloudBelts(sphereUV * vec2(1.0, 4.0)); // Stretch vertically for more bands
    vec3 baseColor = vec3(0.8, 0.6, 0.4); // Base color of Jupiter
    vec3 cloudColor = baseColor + vec3(belts * 0.3); // Apply cloud belts

    // Add the Great Red Spot
    float redSpot = greatRedSpot(sphereUV);
    cloudColor = mix(cloudColor, vec3(0.8, 0.2, 0.1), redSpot * 0.5);

    // Final color with lighting (simple diffuse)
    vec3 normal = normalize(vec3(uv, sqrt(1.0 - radius * radius)));
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 finalColor = cloudColor * (diffuse + 0.2);

    gl_FragColor = vec4(finalColor, 1.0);
}

