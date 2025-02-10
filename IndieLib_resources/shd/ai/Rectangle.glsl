// Define a rounded rectangle (box with rounded corners)
float sdRoundRectangle(vec3 p) 
{
	float radius=0.05;
	vec3 size=vec3(1.0,0.25,0.05);
    vec3 q = abs(p) - size + radius;
    return length(max(q, 0.0)) - radius;
}

// Define a simple axis-aligned box (rectangle)
float sdRectangle(vec3 p, vec3 size) {
    vec3 q = abs(p) - size;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Raymarching function
float raymarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    for (int i = 0; i < 64; i++) {
        vec3 p = ro + t * rd;
        float d = sdRoundRectangle(p);
        if (d < 0.001) return t;
        t += d;
        if (t > 100.0) break;
    }
    return -1.0;
}

// Compute normals using finite differences
vec3 computeNormal(vec3 p) {
    float d = sdRoundRectangle(p);
    vec2 e = vec2(0.001, 0.0);
    vec3 n = d - vec3(
        sdRoundRectangle(p - e.xyy),
        sdRoundRectangle(p - e.yxy),
        sdRoundRectangle(p - e.yyx)
    );
    return normalize(n);
}

// Diffuse lighting
float diffuse(vec3 n, vec3 l) {
    return max(dot(n, l), 0.0);
}

// Specular lighting
float specular(vec3 n, vec3 l, vec3 v, float shininess) {
    vec3 h = normalize(l + v);
    return pow(max(dot(n, h), 0.0), shininess);
}

// Sky or environment map
vec3 sky(vec3 rd) {
    return mix(vec3(0.5, 0.7, 1.0), vec3(0.1, 0.2, 0.5), rd.y);
}

void main() {
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    // Camera setup
    vec3 ro = vec3(0.0, 1.0, 5.0);
    vec3 rd = normalize(vec3(uv, -1.0));

    // Rotate the rectangle around the y-axis
    float angle = time * 0.5;
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    ro.xz = rot * ro.xz;
    rd.xz = rot * rd.xz;

    // Raymarch through the scene
    float t = raymarch(ro, rd);

    // Shading
    if (t > 0.0) {
        vec3 p = ro + t * rd;
        vec3 n = computeNormal(p);
        vec3 l = normalize(vec3(1.0, 1.0, -1.0));
        vec3 v = normalize(ro - p);

        float diff = diffuse(n, l);
        float spec = specular(n, l, v, 32.0);

        vec3 color = vec3(0.8, 0.7, 0.6) * diff + vec3(1.0, 1.0, 1.0) * spec;
        gl_FragColor = vec4(color, 1.0);
    } else {
        // Handle misses with a sky or environment map
        gl_FragColor = vec4(sky(rd), 1.0);
    }
}
