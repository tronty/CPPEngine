// Pseudorandom generator
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Pseudorandom generator: returns a value in the range [-1, 1]
vec2 random2(vec2 st) {
    st = vec2( dot(st,vec2(127.1,311.7)),
               dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

// Smooth interpolation function
float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

// 2D Perlin noise function
float perlin_noise(vec2 st) {
    // Grid cell coordinates
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of the grid
    vec2 a = random2(i);
    vec2 b = random2(i + vec2(1.0, 0.0));
    vec2 c = random2(i + vec2(0.0, 1.0));
    vec2 d = random2(i + vec2(1.0, 1.0));

    // Compute the dot product between gradient and distance
    vec2 u = f * f * (3.0 - 2.0 * f);  // Use smootherstep for smoother interpolation
    float n0 = dot(a, f - vec2(0.0, 0.0));
    float n1 = dot(b, f - vec2(1.0, 0.0));
    float n2 = dot(c, f - vec2(0.0, 1.0));
    float n3 = dot(d, f - vec2(1.0, 1.0));

    // Interpolate the results
    float x1 = mix(n0, n1, u.x);
    float x2 = mix(n2, n3, u.x);
    return mix(x1, x2, u.y);
}

// Jupiter-like turbulence function
vec3 jupiter_turbulence(vec2 uv) {
    float noise = 0.0;
    float scale = 1.0;
    float persistence = 0.5;
    float lacunarity = 2.0;

    // Combine multiple layers of noise for turbulence effect
    for (int i = 0; i < 5; i++) {
        noise += perlin_noise(uv * scale) * persistence;
        scale *= lacunarity;
        persistence *= 0.5;
    }

    // Jupiter color bands (approximated)
    vec3 jupiter_colors[5];
    jupiter_colors[0] = vec3(0.8, 0.5, 0.2); // Orange-brown
    jupiter_colors[1] = vec3(0.9, 0.9, 0.9); // White
    jupiter_colors[2] = vec3(0.7, 0.3, 0.1); // Darker brown
    jupiter_colors[3] = vec3(1.0, 0.8, 0.5); // Light brown
    jupiter_colors[4] = vec3(0.6, 0.4, 0.2); // Dark brown

    // Mapping noise to the Jupiter color bands
    float band = smoothstep(-1.0, 1.0, noise);
    int index = int(floor(band * 4.0));

    return mix(jupiter_colors[index], jupiter_colors[index + 1], fract(band * 4.0));
}

// Starry background
vec3 star_background(vec2 uv) {
    vec2 st = uv * vec2(1024.0, 1024.0); // Kovakoodattu resoluutio
    float stars = step(0.999, random(st));
    return vec3(stars);
}

// Convert UV to spherical coordinates for 3D effect
vec2 sphere_uv(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    float r = length(uv);
    float theta = atan(uv.y, uv.x);
    float phi = acos(uv.y / r);
    return vec2(theta / (2.0 * 3.14159), phi / 3.14159);
}

// Output color
out vec4 FragColor;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;

    // Sphere coordinates for planet
    vec2 sphereCoords = sphere_uv(uv);

    // Starry background
    vec3 color = star_background(uv);

    // Add the Jupiter-like planet
    float radius = 0.5;
    vec2 center = vec2(0.5, 0.5);
    float dist = length(uv - center);

        vec2 sphereUV = sphereCoords * 5.0;
        vec3 planetColor = jupiter_turbulence(sphereUV);
    //if (dist < radius) 
    {
        color = mix(color, planetColor, smoothstep(radius, radius - 0.01, dist));
    }

    gl_FragColor = vec4(planetColor, 1.0);
}

