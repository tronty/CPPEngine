float sdRectangle(vec3 p) {
    // Define the half–dimensions of the rectangle.
    // (This rectangle extends from –1.0 to +1.0 in x and –0.5 to +0.5 in y.)
    vec2 halfSize = vec2(1.0, 0.5);
    
    // Compute the difference between the absolute point coordinates and the half–dimensions.
    // This gives a 2D vector d:
    //   - If p.xy is inside the rectangle, d will have negative components.
    //   - If p.xy is outside, d will be positive in the directions in which p exceeds the bounds.
    vec2 d = abs(p.xy) - halfSize;
    
    // The distance is computed by combining two terms:
    // 1. For the part outside the rectangle, we use the Euclidean distance.
    // 2. For the inside, we take the maximum negative offset.
    float outsideDist = length(max(d, vec2(0.0)));
    float insideDist  = min(max(d.x, d.y), 0.0);
    
    return outsideDist + insideDist;
}

// Raymarching function
float raymarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    for (int i = 0; i < 64; i++) {
        vec3 p = ro + t * rd;
        float d = sdRectangle(p);
        if (d < 0.001) return t;
        t += d;
        if (t > 100.0) break;
    }
    return -1.0;
}

// Compute normals using finite differences
vec3 computeNormal(vec3 p) {
    float d = sdRectangle(p);
    vec2 e = vec2(0.001, 0.0);
    vec3 n = d - vec3(
        sdRectangle(p - e.xyy),
        sdRectangle(p - e.yxy),
        sdRectangle(p - e.yyx)
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

    // Rotate the tower around the y-axis
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

