// Number of boxes in the X and Z directions:
const int N = 5;  // number of boxes along the X axis
const int M = 4;  // number of boxes along the Z axis

//
// Signed distance to an axis-aligned box of half-size 'b'.
// p is the point in local box coordinates.
//
float sdBox(vec3 p, vec3 b)
{
    vec3 d = abs(p) - b;
    // 'outside' distance + 'inside' distance
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

//
// Returns the SDF for a single box centered at 'centerPos' with half-size 'boxHalfSize'.
// p is the point in world coordinates.
//
float boxSDF(vec3 p, vec3 centerPos, vec3 boxHalfSize)
{
    // Shift to local coordinates of that box
    vec3 localP = p - centerPos;
    return sdBox(localP, boxHalfSize);
}

//
// Returns the minimum distance from p to any box in an N x M grid.
// Each box is spaced out by 'spacing' units in X and Z,
// all centered roughly around the origin.
//
float sdTile3D(vec3 p)
{
    float spacing = 2.0;            // distance between box centers
    vec3 boxHalfSize = vec3(0.4);   // half-size of each box
    float minDist = 1e10;          // large initial value

    // Loop over the N x M grid
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            // Compute the center of the (i, j)th box
            float xCenter = (float(i) - float(N - 1) * 0.5) * spacing;
            float zCenter = (float(j) - float(M - 1) * 0.5) * spacing;
            vec3 centerPos = vec3(xCenter, 0.0, zCenter);

            // Distance to this box
            float d = boxSDF(p, centerPos, boxHalfSize);
            minDist = min(minDist, d);
        }
    }
    return minDist;
}

//
// Simple ray-march through the SDF field
//
vec3 raymarch(vec3 ro, vec3 rd)
{
    float t = 0.0;
    const float tMax = 50.0;  // maximum marching distance
    for (int i = 0; i < 128; i++)
    {
        vec3 pos = ro + rd * t;
        float dist = sdTile3D(pos);

        // If we are close enough, consider it a "hit"
        if (dist < 0.001)
        {
            // Color the hit based on the distance traveled
            return vec3(1.0, 0.9, 0.7) * (1.0 - 0.02 * t);
        }
        
        // Advance the ray
        t += dist * 0.5;

        // If we go too far, no hit
        if (t > tMax)
        {
            break;
        }
    }
    
    // If we never hit anything, return a background color
    return vec3(0.2, 0.3, 0.5);
}

void main()
{
    // Normalized pixel coordinates (range: -1 to 1)
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y; // correct for aspect ratio

    // Camera setup:
    //  - ro: camera (ray origin) is above the scene looking toward -Z
    //  - rd: ray direction, aiming slightly down.
#if 0
    vec3 ro = vec3(0.0, 3.5, 6.0);
    vec3 rd = normalize(vec3(uv.x, -0.3, -1.0));
#else
    vec3 ro = vec3(0.0, 0.0, -1.0); // Ray origin
    vec3 rd = normalize(vec3(uv, -1.5)); // Ray direction
#endif
    // Perform the raymarch
    vec3 color = raymarch(ro, rd);

    // Output final color
    gl_FragColor = vec4(color, 1.0);
}

