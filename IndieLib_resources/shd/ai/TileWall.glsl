// Define a rounded rectangle (box with rounded corners)
float sdRoundRectangle(vec3 p) 
{
    float radius = 0.05;
    vec3 size = vec3(1.0, 0.25, 0.05);
    vec3 q = abs(p) - size + radius;
    return length(max(q, 0.0)) - radius;
}

// Define a tiled wall with n x m tiles
float sdTileWall(vec3 p, int n, int m) 
{
    // Tile size
    vec2 tileSize = vec2(1.0 / float(n), 1.0 / float(m));
    
    // Repeat the pattern in the x and y directions
    vec2 tileCoord = mod(p.xy, tileSize) - 0.5 * tileSize;
    
    // Create a rounded rectangle for each tile
    return sdRoundRectangle(vec3(tileCoord, p.z));
}

// Raymarching function
float raymarch(vec3 ro, vec3 rd, int n, int m) {
    float t = 0.0;
    for (int i = 0; i < 64; i++) {
        vec3 p = ro + t * rd;
        float d = sdTileWall(p, n, m);
        if (d < 0.001) return t;
        t += d;
        if (t > 100.0) break;
    }
    return -1.0;
}

// Compute normals using finite differences
vec3 computeNormal(vec3 p, int n, int m) {
    float d = sdTileWall(p, n, m);
    vec2 e = vec2(0.001, 0.0);
    vec3 normal = d - vec3(
        sdTileWall(p - e.xyy, n, m),
        sdTileWall(p - e.yxy, n, m),
        sdTileWall(p - e.yyx, n, m)
    );
    return normalize(normal);
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

    // Rotate the wall around the y-axis
    float angle = time * 0.5;
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    ro.xz = rot * ro.xz;
    rd.xz = rot * rd.xz;

    // Number of tiles in x and y directions
    int n = 5; // Number of tiles in x direction
    int m = 5; // Number of tiles in y direction

    // Raymarch through the scene
    float t = raymarch(ro, rd, n, m);

    // Shading
    if (t > 0.0) {
        vec3 p = ro + t * rd;
        vec3 n = computeNormal(p, n, m);
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
