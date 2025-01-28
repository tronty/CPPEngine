// -----------------------------------------------------------------------------
// 1) Scene Configuration: how many boxes in the wall?
//    We'll do Nx x Ny boxes. You can change these as desired.
// -----------------------------------------------------------------------------
const int Nx = 4;
const int Ny = 3;

// Each box is 2 units wide in local space: half-size = (1,1,1).
// We'll place them on the XZ-plane in a grid, spaced out by 2.5 units
// so they don't overlap.
vec3 boxHalfSize = vec3(1.0);

// Helper: compute the center of box with index idx
// idx runs from 0..(Nx*Ny - 1)
vec3 getBoxCenter(int idx)
{
    int ix = idx % Nx;    // which column
    int iz = idx / Nx;    // which row
    // Shift so the grid is centered near the origin:
    // e.g. if Nx=4 => x from -1.5..+1.5 steps of 1 => multiply by spacing
    float spacing = 2.5;
    float x = (float(ix) - (float(Nx)-1.0)*0.5) * spacing;
    float z = (float(iz) - (float(Ny)-1.0)*0.5) * spacing;
    // We'll keep y=0 so the boxes line up in a "wall" on the XZ plane:
    // If you prefer a 2D "wall" in X-Y, just adapt the coordinate usage.
    return vec3(x, 0.0, z);
}

// -----------------------------------------------------------------------------
// 2) Signed Distance to a single axis-aligned box of half-size 'boxHalfSize'
//    SDF for a box at the origin
// -----------------------------------------------------------------------------
float sdBox(vec3 p, vec3 halfSize)
{
    // Standard box SDF
    //   Let d = abs(p) - halfSize
    //   Outside distance = length(max(d, 0))
//   Inside distance  = min(max(d.x,d.y,d.z), 0)
    vec3 d = abs(p) - halfSize;
    float outsideDist = length(max(d, 0.0));
    float insideDist  = min(max(d.x, max(d.y, d.z)), 0.0);
    return outsideDist + insideDist;
}

// -----------------------------------------------------------------------------
// 3) Scene SDF: union of Nx*Ny boxes
//    We'll store both the minimum distance AND which box is closest.
// -----------------------------------------------------------------------------
struct SceneResult {
    float dist;  // distance to the union
    int   boxID; // which box "won" (i.e., is the closest)
};

SceneResult mapSceneAllBoxes(vec3 p)
{
    SceneResult sr;
    sr.dist = 1e10;  // large initial
    sr.boxID = -1;

    // Loop over all boxes, find the one with minimum distance
    for(int i=0; i<Nx*Ny; i++)
    {
        vec3 center = getBoxCenter(i);
        float dBox  = sdBox(p - center, boxHalfSize);
        if(dBox < sr.dist)
        {
            sr.dist  = dBox;
            sr.boxID = i;
        }
    }

    return sr;
}

// For normal calculation, we only need the distance, ignoring which box is hit
float sdOnlyScene(vec3 p)
{
    float minD = 1e10;
    for(int i=0; i<Nx*Ny; i++)
    {
        vec3 center = getBoxCenter(i);
        float dBox  = sdBox(p - center, boxHalfSize);
        if(dBox < minD) minD = dBox;
    }
    return minD;
}

// -----------------------------------------------------------------------------
// 4) Sphere Tracing: returns the distance t and also which box was hit.
// -----------------------------------------------------------------------------
struct RayHit {
    float t;
    int   boxID;
    vec3  pos;
};

RayHit sphereTrace(vec3 ro, vec3 rd)
{
    RayHit hit;
    hit.t     = -1.0;
    hit.boxID = -1;
    hit.pos   = vec3(0.0);

    const float MAX_DIST = 100.0;
    const int   MAX_STEP = 128;
    const float EPSILON  = 1e-4;

    float tMarch = 0.0;
    for(int i=0; i<MAX_STEP; i++)
    {
        vec3 currentPos = ro + rd * tMarch;
        SceneResult sr = mapSceneAllBoxes(currentPos);
        if(sr.dist < EPSILON)
        {
            // Hit the surface
            hit.t     = tMarch;
            hit.boxID = sr.boxID;
            hit.pos   = currentPos;
            break;
        }
        tMarch += sr.dist;
        if(tMarch > MAX_DIST) break; // no intersection
    }

    return hit;
}

// -----------------------------------------------------------------------------
// 5) Estimate Normal via finite differences of 'sdOnlyScene'
// -----------------------------------------------------------------------------
vec3 calcNormal(vec3 p)
{
    const float e = 0.001;
    float c = sdOnlyScene(p);
    float dx = sdOnlyScene(p + vec3(e,0,0)) - c;
    float dy = sdOnlyScene(p + vec3(0,e,0)) - c;
    float dz = sdOnlyScene(p + vec3(0,0,e)) - c;
    return normalize(vec3(dx, dy, dz));
}

// -----------------------------------------------------------------------------
// 6) Determine face-based UV, given the local position in the box
//    We assume 'localP' is in [-1..+1], since boxHalfSize=(1,1,1).
//    We check the largest normal component to pick the face
//    Then map the other two coords from [-1..+1] to [0..1] for tile UV
// -----------------------------------------------------------------------------
vec2 faceUV(vec3 localP, vec3 n)
{
    // largest absolute normal => that axis is the face
    vec3 an = abs(n);
    vec2 uv;
    if(an.z >= an.x && an.z >= an.y)
    {
        // ±Z face => use X,Y
        uv = localP.xy; 
    }
    else if(an.x >= an.y && an.x >= an.z)
    {
        // ±X face => use Z,Y
        uv = localP.zy; 
    }
    else
    {
        // ±Y face => use X,Z
        uv = localP.xz;
    }
    // map from [-1..+1] to [0..1]
    uv = uv * 0.5 + 0.5;
    return uv;
}

// -----------------------------------------------------------------------------
// 7) Noise and fBM
// -----------------------------------------------------------------------------
float hash2D(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

float noise2D(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash2D(i + vec2(0,0));
    float b = hash2D(i + vec2(1,0));
    float c = hash2D(i + vec2(0,1));
    float d = hash2D(i + vec2(1,1));

    vec2 f2 = f*f*(3.0 - 2.0*f);
    float y1 = mix(a,b,f2.x);
    float y2 = mix(c,d,f2.x);
    return mix(y1,y2,f2.y);
}

float fbm(vec2 p)
{
    float sum=0.0;
    float amp=0.5;
    for(int i=0; i<4; i++)
    {
        sum += amp*noise2D(p);
        p   *= 2.02;
        amp *= 0.5;
    }
    return sum;
}

// -----------------------------------------------------------------------------
// 8) Build a local tangent frame (T, B, N) so we can do normal perturbation
// -----------------------------------------------------------------------------
void buildFrame(in vec3 N, out vec3 T, out vec3 B)
{
    if(abs(N.y) < 0.999)
        T = normalize(cross(N, vec3(0,1,0)));
    else
        T = normalize(cross(N, vec3(1,0,0)));
    B = cross(N, T);
}

// -----------------------------------------------------------------------------
// 9) Bumpy Tile Material: given face UV and the face normal, return color+normal
// -----------------------------------------------------------------------------
struct Material {
    vec3 color;
    vec3 normal;
};

Material getTileMaterial(vec2 uv, vec3 faceN)
{
    Material m;

    // Tiling
    float tileCount = 4.0;
    vec2 tiledUV    = uv * tileCount;
    vec2 tileIdx    = floor(tiledUV);
    vec2 fracUV     = fract(tiledUV);

    // Grout lines
    float grout = 0.02;
    bool inGrout = (fracUV.x < grout || fracUV.y < grout ||
                    fracUV.x > (1.0 - grout) || fracUV.y > (1.0 - grout));
    if(inGrout)
    {
        m.color  = vec3(0.05);
        m.normal = faceN;
        return m;
    }

    // Bump via fBM
    float bumpScale = 0.2;
    vec3 T, B;
    buildFrame(faceN, T, B);

    // Evaluate fBM around (tileIdx + fracUV)
    vec2 noiseCoord = tileIdx + fracUV*4.0;
    float valC = fbm(noiseCoord * 2.0);
    float eps  = 0.002;
    float valX = fbm((tileIdx + (fracUV + vec2(eps,0))*4.0)*2.0);
    float valY = fbm((tileIdx + (fracUV + vec2(0,eps))*4.0)*2.0);

    float dx = (valX - valC);
    float dy = (valY - valC);

    // Bumped normal in world space
    vec3 bumpedN = normalize(faceN*(1.0/bumpScale) + T*dx + B*dy);

    // Base color
    vec3 baseCol = mix(vec3(0.4, 0.35, 0.3), 
                       vec3(0.8, 0.75, 0.7),
                       valC);

    m.color  = baseCol;
    m.normal = bumpedN;
    return m;
}

// -----------------------------------------------------------------------------
// 10) Shadow check (sphere trace) for the union of boxes
// -----------------------------------------------------------------------------
float computeShadow(vec3 origin, vec3 dir)
{
    // small bias
    vec3 p = origin + dir*0.001;

    const float MAX_DIST = 50.0;
    const int   MAX_STEP = 50;
    const float EPSILON  = 1e-3;

    float distSoFar = 0.0;
    for(int i=0; i<MAX_STEP; i++)
    {
        vec3 pos = p + dir*distSoFar;
        float d = sdOnlyScene(pos);
        if(d < EPSILON)
        {
            // re-hit => shadow
            return 0.0;
        }
        distSoFar += d;
        if(distSoFar > MAX_DIST)
        {
            // no intersection => lit
            return 1.0;
        }
    }
    return 1.0; // likely lit
}

// -----------------------------------------------------------------------------
// Main Fragment Shader
// -----------------------------------------------------------------------------
void main()
{
    // A) Hardcode resolution
    vec2 resolution = vec2(800.0, 600.0);
    float aspect = resolution.x / resolution.y;

    // B) NDC [-1..+1]
    vec2 ndc = (gl_FragCoord.xy / resolution)*2.0 - 1.0;
    ndc.x *= aspect;

    // C) Simple camera
    //    Position the camera so we can see the "wall" of boxes
    //    We'll place it a bit above and away on the +Y, +Z side
    vec3 ro = vec3(0.0, 4.0, 10.0);
    vec3 lookAt = vec3(0.0, 0.0, 0.0);
    vec3 forward = normalize(lookAt - ro);
    vec3 right   = normalize(cross(forward, vec3(0,1,0)));
    vec3 up      = cross(right, forward);

    // A pinhole camera with a certain FOV:
    float fov = 1.0;  // a constant "focal" factor
    vec3 rd = normalize(forward + ndc.x*right*fov + ndc.y*up*fov);

    // D) Sphere trace
    RayHit hit = sphereTrace(ro, rd);
    if(hit.t < 0.0)
    {
        // miss => sky
        gl_FragColor = vec4(0.4, 0.7, 1.0, 1.0);
        return;
    }

    // E) Surface normal from SDF
    vec3 N = calcNormal(hit.pos);

    // F) Local position in the box we hit
    vec3 centerHit = getBoxCenter(hit.boxID);
    vec3 localPos  = hit.pos - centerHit;  // now in [-1..+1] if halfSize=1

    // G) Face-based UV for the box
    vec2 uv = faceUV(localPos, N);

    // H) Bumpy tile material
    Material mat = getTileMaterial(uv, N);

    // I) "Orbiting" light from pseudo-time
    float t = 0.01*(gl_FragCoord.x + gl_FragCoord.y); 
    // revolve around the scene
    vec3 lightPos = vec3(8.0*cos(t), 5.0, 8.0*sin(t));

    vec3 L = normalize(lightPos - hit.pos);
    float shadow = computeShadow(hit.pos, L);

    // J) Simple Phong lighting
    float ambient = 0.15;
    float diff    = max(dot(mat.normal, L), 0.0) * shadow;
    vec3 V        = normalize(ro - hit.pos);
    vec3 R        = reflect(-L, mat.normal);
    float spec    = pow(max(dot(R, V), 0.0), 20.0)*0.5*shadow;

    vec3 finalColor = mat.color*(ambient + diff) + vec3(1.0)*spec;

    gl_FragColor = vec4(finalColor, 1.0);
}

