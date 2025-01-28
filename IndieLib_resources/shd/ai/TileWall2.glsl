//-----------------------------------------------------------------------------
// 1) Signed Distance to an Axis-Aligned Box (centered at origin)
//    The box half-size = (1,1,1), so the box is from -1..+1 in x,y,z
//-----------------------------------------------------------------------------
float sdBox(vec3 p, vec3 b)
{
    // Standard box SDF:
    //   Let d = abs(p) - b
    //   Outside distance = length(max(d,0))
    //   Inside distance  = min(max(d.x,d.y,d.z),0)
    vec3 d = abs(p) - b;
    float outsideDist = length(max(d, 0.0));
    float insideDist  = min(max(d.x, max(d.y, d.z)), 0.0);
    return outsideDist + insideDist;
}

//-----------------------------------------------------------------------------
// 2) Scene Distance Function
//    Currently, we only have one box in the scene
//-----------------------------------------------------------------------------
float mapScene(vec3 p)
{
    return sdBox(p, vec3(1.0));  // box half-size = (1,1,1)
}

//-----------------------------------------------------------------------------
// 3) Sphere Trace (Ray March) the Scene
//    Returns:
//      - t < 0.0 if no intersection
//      - intersection distance otherwise
//-----------------------------------------------------------------------------
struct RayHit {
    float t;
    vec3  pos;
};

RayHit sphereTrace(vec3 ro, vec3 rd)
{
    RayHit hit;
    hit.t   = -1.0;   
    hit.pos = vec3(0.0);

    const float MAX_DIST = 100.0; 
    const int   MAX_STEP = 128;
    const float EPSILON  = 1e-4;

    float tMarch = 0.0;
    for(int i=0; i<MAX_STEP; i++)
    {
        vec3 p = ro + rd * tMarch;
        float distToScene = mapScene(p);
        if(distToScene < EPSILON)
        {
            // We've hit the surface
            hit.t   = tMarch;
            hit.pos = p;
            break;
        }
        tMarch += distToScene;
        if(tMarch > MAX_DIST)
        {
            // No hit within our range
            break;
        }
    }

    return hit;
}

//-----------------------------------------------------------------------------
// 4) Estimate Surface Normal via Finite Differences of mapScene()
//-----------------------------------------------------------------------------
vec3 calcNormal(vec3 p)
{
    const float e = 0.001;
    float d = mapScene(p);
    float dx = mapScene(p + vec3(e,0,0)) - d;
    float dy = mapScene(p + vec3(0,e,0)) - d;
    float dz = mapScene(p + vec3(0,0,e)) - d;
    return normalize(vec3(dx, dy, dz));
}

//-----------------------------------------------------------------------------
// 5) Determine which face (±X, ±Y, or ±Z) we're on by checking the largest
//    absolute component of the normal. Then compute a 2D UV in [0..1]
//    using the other two coordinates of the hit position (in [-1..1]).
//-----------------------------------------------------------------------------
vec2 faceUV(vec3 p, vec3 n)
{
    vec3 an = abs(n);

    // Largest absolute normal component => that axis is "the face."
    // We'll pick the other two coordinates from p as the face plane.
    vec2 uv;
    if(an.z >= an.x && an.z >= an.y)
    {
        // ±Z face => use X,Y
        uv = p.xy; 
    }
    else if(an.x >= an.y && an.x >= an.z)
    {
        // ±X face => use Z,Y
        uv = p.zy; 
    }
    else
    {
        // ±Y face => use X,Z
        uv = p.xz; 
    }

    // Map from [-1..1] to [0..1]
    uv = uv * 0.5 + 0.5;
    return uv;
}

//-----------------------------------------------------------------------------
// 6) Noise & fBM for Bumpy Tiles
//-----------------------------------------------------------------------------
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

    vec2 f2 = f*f*(3.0-2.0*f);
    float y1 = mix(a,b,f2.x);
    float y2 = mix(c,d,f2.x);
    return mix(y1,y2,f2.y);
}

// Basic fractal Brownian motion
float fbm(vec2 p)
{
    float sum=0.0;
    float amp=0.5;
    for(int i=0; i<4; i++)
    {
        sum += amp * noise2D(p);
        p   *= 2.02;
        amp *= 0.5;
    }
    return sum;
}

//-----------------------------------------------------------------------------
// 7) Build a local tangent frame for the face normal (N), so we can 
//    perturb the normal with (dHeight/dU, dHeight/dV).
//-----------------------------------------------------------------------------
void buildFrame(in vec3 N, out vec3 T, out vec3 B)
{
    // pick an arbitrary axis not parallel to N for T
    if(abs(N.y) < 0.999) {
        T = normalize(cross(N, vec3(0,1,0)));
    } else {
        T = normalize(cross(N, vec3(1,0,0)));
    }
    B = cross(N, T);
}

//-----------------------------------------------------------------------------
// 8) Tile Material: determines color & bumped normal from the face UV
//-----------------------------------------------------------------------------
struct Material {
    vec3 color;
    vec3 normal;
};

Material getTileMaterial(vec2 uv, vec3 faceN)
{
    Material m;

    // Tiling
    float tileCount = 4.0;
    vec2  tiledUV   = uv * tileCount;
    vec2  tileIdx   = floor(tiledUV);
    vec2  fracUV    = fract(tiledUV);

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

    // Bump from fractal noise
    float bumpScale = 0.2;
    vec3 T, B;
    buildFrame(faceN, T, B);

    // Evaluate fBM around (tiledUV)
    // We'll shift by tileIdx to vary tile by tile
    vec2 noiseCoord = tileIdx + fracUV*4.0;
    float centerVal = fbm(noiseCoord * 2.0);

    float eps     = 0.002;
    float valX    = fbm((tileIdx + (fracUV + vec2(eps,0))*4.0)*2.0);
    float valY    = fbm((tileIdx + (fracUV + vec2(0,eps))*4.0)*2.0);

    float dx = valX - centerVal;
    float dy = valY - centerVal;

    // Bumped normal in world space
    vec3 bumpedN = normalize(faceN*(1.0/bumpScale) + T*dx + B*dy);

    // Color variation
    vec3 baseCol = mix(vec3(0.4, 0.35, 0.3), 
                       vec3(0.8, 0.75, 0.7),
                       centerVal);

    m.color  = baseCol;
    m.normal = bumpedN;
    return m;
}

//-----------------------------------------------------------------------------
// 9) Shadow Check via Sphere Tracing
//-----------------------------------------------------------------------------
float computeShadow(vec3 origin, vec3 dir)
{
    // Slight bias
    vec3 p = origin + dir*0.001;

    const float MAX_DIST = 50.0;
    const int   MAX_STEP = 50;
    const float EPSILON  = 1e-3;

    float distSoFar = 0.0;
    for(int i=0; i<MAX_STEP; i++)
    {
        vec3 pos = p + dir*distSoFar;
        float d = mapScene(pos);
        if(d < EPSILON)
        {
            // Re-hit the box => in shadow
            return 0.0;
        }
        distSoFar += d;
        if(distSoFar > MAX_DIST) 
        {
            // Light direction escapes => fully lit
            return 1.0;
        }
    }
    // If we exit the loop, likely no intersection => lit
    return 1.0;
}

//-----------------------------------------------------------------------------
// Main Fragment Shader
//-----------------------------------------------------------------------------
void main()
{
    // 1) Hardcode resolution (no uniforms)
    vec2 resolution = vec2(800.0, 600.0);
    float aspect    = resolution.x / resolution.y;

    // 2) Normalized device coords in [-1..1]
    vec2 ndc = (gl_FragCoord.xy / resolution)*2.0 - 1.0;
    ndc.x *= aspect;

    // 3) Simple camera outside the box, looking along -Z
    //    Eye at (0,0,+2.5) => should see the outside of a box near the origin
    vec3 ro = vec3(0.0, 0.0, 2.5);
    float focal = 1.0;
    vec3 rd = normalize(vec3(ndc.x, ndc.y, -focal));

    // 4) Raymarch the scene
    RayHit hit = sphereTrace(ro, rd);
    if(hit.t < 0.0)
    {
        // Miss => sky
        gl_FragColor = vec4(0.4, 0.7, 1.0, 1.0);
        return;
    }

    // 5) Surface normal from SDF
    vec3 N = calcNormal(hit.pos);

    // 6) Determine face-based UV
    vec2 uv = faceUV(hit.pos, N);

    // 7) Evaluate tile material
    Material mat = getTileMaterial(uv, N);

    // 8) Fake "time" from gl_FragCoord for an orbiting sun
    float t = 0.01 * (gl_FragCoord.x + gl_FragCoord.y);
    // Light orbits around the box at some radius, above/below
    vec3 lightPos = vec3(2.0*cos(t), 1.5, 2.0*sin(t));

    vec3 L = normalize(lightPos - hit.pos);

    // 9) Shadow test
    float shadow = computeShadow(hit.pos, L);

    // 10) Simple Phong shading
    float ambient = 0.15;
    float diff    = max(dot(mat.normal, L), 0.0)*shadow;

    // For specular
    vec3 V = normalize(ro - hit.pos);
    vec3 R = reflect(-L, mat.normal);
    float specAngle = max(dot(R, V), 0.0);
    float spec = pow(specAngle, 20.0) * 0.5 * shadow;

    vec3 finalColor = mat.color*(ambient + diff) + vec3(1.0)*spec;

    gl_FragColor = vec4(finalColor, 1.0);
}

