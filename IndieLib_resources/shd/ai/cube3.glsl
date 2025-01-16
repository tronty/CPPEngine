#define RB

// Signed distance function for a rounded box
float sdRoundBox(vec3 p, vec3 b, float r) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

#ifndef RB
// For a single brick at the origin:
float sdBrick(vec3 p, vec2 brickSize, float thickness, float r)
{
    // Our "box" for a single brick is sized: (brickSize.x, brickSize.y, thickness).
    // We'll pass half-sizes into sdRoundBox, so half of each dimension:
    vec3 halfSize = vec3(brickSize * 0.5, thickness * 0.5);

    // Compute the distance from p to that box:
    return sdRoundBox(p, halfSize, r);
}

// SDF for an infinite repeating brick wall in the XY plane.
float sdBrickWall(vec3 p)
{
    // Size of each brick (width, height).
    vec2 brickSize  = vec2(1.0, 0.5);
    // How thick the wall is (z-dimension).
    float thickness = 0.2;
    // Round corners
    float rounding  = 0.05;

    // Identify which row/column (cell) we are in:
    vec2 cellIndex = floor(p.xy / brickSize);
    
    // Fractional part within the cell:
    vec2 cellFrac  = fract(p.xy / brickSize);

    // For an odd row, we shift the brick by half a brick in X:
    // e.g., row 1, 3, 5, ...
    float isOddRow = mod(cellIndex.y, 2.0);
    
    // We shift X by half a brick if isOddRow=1
    // cellFrac.x in [0..1], so let's shift it left by 0.5 if odd:
    cellFrac.x = cellFrac.x + 0.5 * isOddRow;
    
    // If that goes above 1.0, wrap around:
    if(cellFrac.x > 1.0) {
       cellFrac.x -= 1.0;
       // Also shift the cellIndex.x by +1 when we wrap,
       // if you want to keep track. But for distance only,
       // the local position is what really matters.
    }
    
    // Now cellFrac.x,y is in [0..1] in local coordinates.
    // Center it around (0,0) in order to pass into sdBrick:
    vec2 localXY = (cellFrac - 0.5) * brickSize;  // center at (0,0)

    // We keep z as is (p.z).
    vec3 localPos = vec3(localXY, p.z);

    // SDF to the *rounded box* representing this cell's brick.
    return sdBrick(localPos, brickSize, thickness, rounding);
}

float mapScene(vec3 p)
{
    // For simplicity, just the wall:
    return sdBrickWall(p);
    // Or if you have more geometry, do something like:
    // float dWall = sdBrickWall(p);
    // float dSphere = length(p - vec3(0,2,0)) - 0.5;
    // return min(dWall, dSphere);
}

// Approximate normal via central differencing of the SDF.
vec3 estimateNormal(vec3 p) {
    float d = mapScene(p);
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        mapScene(p + vec3(e.x, e.y, e.y)) - d,
        mapScene(p + vec3(e.y, e.x, e.y)) - d,
        mapScene(p + vec3(e.y, e.y, e.x)) - d
    ));
}
#endif

// Soft shadow computation
float softShadow(vec3 ro, vec3 rd, float k) {
    float res = 1.0;
    float t = 0.02; // Start ray march at a small step
    for (int i = 0; i < 32; i++) {
    #if RB
        float h = sdRoundBox(ro + rd * t, vec3(0.5), 0.1);
    #else
        float h = mapScene(ro + rd * t);
    #endif
        res = min(res, k * h / t);
        t += clamp(h, 0.02, 0.1);
        if (h < 0.001) break;
    }
    return clamp(res, 0.0, 1.0);
}

// Phong lighting model
vec3 phongLighting(vec3 pos, vec3 normal, vec3 lightDir, vec3 viewDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0); // Specular power
    return vec3(0.1) + vec3(1.0) * diff + vec3(1.0) * spec;
}

void main() {
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 ro = vec3(0.0, 0.0, -1.0); // Ray origin
    vec3 rd = normalize(vec3(uv, 1.0)); // Ray direction

    vec3 lightPos = vec3(2.0 * sin(time), 2.0, 2.0 * cos(time));
    // Background color (sky-ish)
    vec3 bgColor = vec3(0.7, 0.8, 1.0); 
    vec3 col     = bgColor;

    // Ray marching
    float t = 0.0;
    float d;
    for (int i = 0; i < 128; i++) {
        vec3 p = ro + rd * t;
#ifdef RB
        d = sdRoundBox(p, vec3(0.5), 0.1);
#else
        d = mapScene(p);
#endif
        if (d < 
#ifdef RB
        0.001
#else
        0.005
#endif
        ) {
#ifdef RB
            // Compute normal at the intersection point
            vec3 normal = normalize(vec3(
                sdRoundBox(p + vec3(0.001, 0.0, 0.0), vec3(0.5), 0.1) - d,
                sdRoundBox(p + vec3(0.0, 0.001, 0.0), vec3(0.5), 0.1) - d,
                sdRoundBox(p + vec3(0.0, 0.0, 0.001), vec3(0.5), 0.1) - d
            ));

            // Lighting calculations
            vec3 lightDir = normalize(lightPos - p);
            vec3 viewDir = normalize(-rd);
            float shadow = softShadow(p + normal * 0.01, lightDir, 8.0);
            col = phongLighting(p, normal, lightDir, viewDir) * shadow;
#else
            // We hit the surface
            vec3 n = estimateNormal(p);
            col = phongLighting(p, n, lightPos, rd);
#endif

            break;
        }
        t += d;
        if (t > 10.0) break;
    }

    col = mix(col, bgColor, exp(-0.05 * t * t)); // Add fog effect
    gl_FragColor = vec4(col, 1.0);
}

