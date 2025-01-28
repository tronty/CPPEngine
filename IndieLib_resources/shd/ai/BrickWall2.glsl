// You can make these uniform if you want to move lights around in real-time
const vec3  uLightPos = vec3(-2.0, 5.0, 3.0);   // Point light position
const vec3  uLightCol = vec3(1.0, 0.95, 0.8);   // Light color
const float uAmbient  = 0.1;                   // Ambient factor
const float uKd       = 1.0;                   // Diffuse factor
const float uKs       = 0.5;                   // Specular factor
const float uShininess = 32.0;                 // Phong shininess

// Grid setup
const int N = 5;  // number of boxes along X
const int M = 4;  // number of boxes along Z

//
// --- 1) SDF Primitives ---
//

// SDF for an axis-aligned box of half-size b:
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// Single box at 'centerPos' with half-size 'boxHalfSize'
float boxSDF(vec3 p, vec3 centerPos, vec3 boxHalfSize) {
    vec3 localP = p - centerPos;
    return sdBox(localP, boxHalfSize);
}

//
// --- 2) SDF for the Tiled Boxes ---
//
float sdTile3D(vec3 p) 
{
    float spacing = 2.0;            // distance between box centers
    vec3 boxHalfSize = vec3(0.4);   // half-size of each box
    float minDist = 1e10;

    // Loop over the N x M grid
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            // Center of box (i, j)
            float xCenter = (float(i) - float(N - 1) * 0.5) * spacing;
            float zCenter = (float(j) - float(M - 1) * 0.5) * spacing;
            vec3 centerPos = vec3(xCenter, 0.0, zCenter);

            float d = boxSDF(p, centerPos, boxHalfSize);
            minDist = min(minDist, d);
        }
    }
    return minDist;
}

//
// --- 3) Calculate Normal via Central Differences ---
//
vec3 calcNormal(vec3 p) 
{
    // Finite difference step
    const float eps = 0.001;
    float dx = sdTile3D(vec3(p.x + eps, p.y, p.z)) - sdTile3D(vec3(p.x - eps, p.y, p.z));
    float dy = sdTile3D(vec3(p.x, p.y + eps, p.z)) - sdTile3D(vec3(p.x, p.y - eps, p.z));
    float dz = sdTile3D(vec3(p.x, p.y, p.z + eps)) - sdTile3D(vec3(p.x, p.y, p.z - eps));
    return normalize(vec3(dx, dy, dz));
}

//
// --- 4) Soft Shadows ---
//     A secondary ray march from the hit point towards the light.
//     'k' defines how soft the shadows appear.
//
float softShadow(vec3 ro, vec3 rd, float k)
{
    float res = 1.0;
    float t   = 0.02;    // start a bit away from surface
    const int SHADOW_STEPS = 64;
    const float tMax = 20.0;

    for (int i = 0; i < SHADOW_STEPS; i++)
    {
        vec3 pos = ro + rd * t;
        float h  = sdTile3D(pos);
        // If inside the geometry, fully in shadow
        if (h < 0.001) {
            return 0.0;
        }
        // Accumulate penumbra
        res = min(res, k * h / t);
        // Advance
        t += clamp(h, 0.01, 0.2);
        // End if we pass the light or go too far
        if (t > tMax) break;
    }
    return res;
}

//
// --- 5) Phong Lighting ---
//     p  = surface position
//     ro = camera (ray origin)
//     rd = camera ray direction
//
vec3 phongLighting(vec3 p, vec3 ro, vec3 rd)
{
    // Normal at point
    vec3 N = calcNormal(p);

    // Vector from surface point toward light
    vec3 L = normalize(uLightPos - p);

    // Vector from surface point back to camera
    vec3 V = normalize(ro - p);

    // Soft shadow factor
    float shadowFactor = softShadow(p + N * 0.002, L, 4.0);

    // Ambient
    vec3 ambient  = uAmbient * uLightCol;

    // Diffuse
    float diff    = max(dot(N, L), 0.0);
    vec3 diffuse  = uKd * diff * uLightCol;

    // Specular (Phong)
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), uShininess);
    vec3 specular = uKs * spec * uLightCol;

    // Combine
    vec3 color = ambient + shadowFactor * (diffuse + specular);
    return color;
}

//
// --- 6) Ray Marcher ---
//
vec3 raymarch(vec3 ro, vec3 rd)
{
    float t     = 0.0;
    const float tMax  = 50.0;
    const int   MAX_STEPS = 128;

    for (int i = 0; i < MAX_STEPS; i++)
    {
        vec3 pos = ro + rd * t;
        float dist = sdTile3D(pos);

        // If close enough, consider it a "hit"
        if (dist < 0.001)
        {
            // Compute shading at the hit point
            return phongLighting(pos, ro, rd);
        }

        // Advance
        t += dist * 0.5;
        if (t > tMax)
        {
            break;
        }
    }

    // If we never hit anything, return a simple sky color
    return vec3(0.5, 0.7, 1.0);
}

void main()
{
    // Convert pixel coords to [-1,1], correcting for aspect
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    // Simple camera setup
    vec3 ro = vec3(0.0, 6.0, -1.0); // camera position
    vec3 rd = normalize(vec3(uv.x, 0.3, 1.0));

    // Ray march the scene
    vec3 color = raymarch(ro, rd);

    // Output
    gl_FragColor = vec4(color, 1.0);
}

