// ChatGPT-o1-preview: 3D textureless, vector-based, bufferless color filled animation in a GLSL 1.3 fragment shader without uniform shader variables?

#ifdef GL_ES
precision highp float;
#endif

float sphereSDF(vec3 p, float r) {
    return length(p) - r;
}

vec3 animatedPosition(vec3 pos, float time) {
    return pos + vec3(sin(time), 0.0, cos(time)) * 2.0;
}

// The scene: sphere at animated position
float map(vec3 p) {
    vec3 spherePos = animatedPosition(vec3(0.0, 0.0, 3.0), time);
    return sphereSDF(p - spherePos, 1.0);
}

// Raymarching function
float rayMarch(vec3 ro, vec3 rd) {
    float dO = 0.0;  // distance from origin
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + rd * dO;
        float dS = map(p);  // distance to the nearest surface
        if (dS < 0.001) return dO; // hit surface
        dO += dS;
        if (dO > 100.0) break;  // too far
    }
    return -1.0;  // no hit
}

// Simple lighting model (Lambertian)
vec3 getNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        map(p + e.xyy) - map(p - e.xyy),
        map(p + e.yxy) - map(p - e.yxy),
        map(p + e.yyx) - map(p - e.yyx)
    ));
}

vec3 getColor(vec3 p, vec3 normal) {
    vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0));
    float diff = max(dot(normal, lightDir), 0.0);
    return vec3(1.0, 0.5, 0.2) * diff;  // Orange diffuse color
}

void main() {
    vec2 uv = (xlv_TEXCOORD0.xy - 0.5 * resolution.xy) / resolution.y;

    vec3 ro = vec3(0.0, 0.0, -5.0);  // Ray origin
    vec3 rd = normalize(vec3(uv, 1.0));  // Ray direction

    // Perform raymarching
    float d = rayMarch(ro, rd);
    if (d > 0.0) {
        vec3 p = ro + rd * d;  // Intersection point
        vec3 normal = getNormal(p);
        vec3 color = getColor(p, normal);
        gl_FragColor = vec4(color, 1.0);
    } else {
        gl_FragColor = vec4(0.0);  // Background
    }
}

