// Define some constants
const float PI = 3.141592653589793;
const float TWO_PI = 6.283185307179586;
const float STAR_RADIUS = 0.2;
const float ORBIT_RADIUS = 0.4;
const float ROTATION_SPEED = 0.5;
const float SPOT_SIZE = 0.05;
const float SPOT_INTENSITY = 0.5;

// Simple pseudo-random function
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// 2D Noise function
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Function to generate starspots
float starspot(vec2 uv, float time) {
    float spots = 0.0;
    for (int i = 0; i < 5; i++) {
        vec2 spotPos = vec2(
            sin(time * 0.1 + float(i) * 123.456),
            cos(time * 0.1 + float(i) * 654.321)
        ) * 0.5;
        float dist = length(uv - spotPos);
        spots += smoothstep(SPOT_SIZE, 0.0, dist) * SPOT_INTENSITY;
    }
    return spots;
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0); // Normalize coordinates
    uv = uv * 2.0 - 1.0; // Center coordinates

    float time = mod(float(int(gl_FragCoord.x) + int(gl_FragCoord.y)), 1000.0) * 0.01;

    // Calculate the positions of the two stars
    vec2 star1Pos = vec2(cos(time * ROTATION_SPEED), sin(time * ROTATION_SPEED)) * ORBIT_RADIUS;
    vec2 star2Pos = -star1Pos;

    // Calculate the distance to each star
    float dist1 = length(uv - star1Pos);
    float dist2 = length(uv - star2Pos);

    // Render the stars
    float star1 = smoothstep(STAR_RADIUS, STAR_RADIUS - 0.01, dist1);
    float star2 = smoothstep(STAR_RADIUS, STAR_RADIUS - 0.01, dist2);

    // Add starspots
    float spots1 = starspot((uv - star1Pos) / STAR_RADIUS, time);
    float spots2 = starspot((uv - star2Pos) / STAR_RADIUS, time);

    // Combine the stars and spots
    float color = star1 + star2 - spots1 - spots2;

    // Output the final color
    gl_FragColor = vec4(vec3(color), 1.0);
}

