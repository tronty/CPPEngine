// We won't rely on a varying from a vertex shader for this demo.
// Instead, we use gl_FragCoord to define our ray directions.
// (If you have a matching vertex shader that passes in vTexCoord,
//  you can adapt accordingly.)

// ---------------------------------------------------------------------
// 1) Basic "hash" and "value noise" utilities, plus a simple fBM
// ---------------------------------------------------------------------
float hash2D(vec2 p)
{
    // Dot with "magic" constants, fractional part of sin => pseudo-random
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float noise2D(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash2D(i + vec2(0.0, 0.0));
    float b = hash2D(i + vec2(1.0, 0.0));
    float c = hash2D(i + vec2(0.0, 1.0));
    float d = hash2D(i + vec2(1.0, 1.0));

    vec2 f2 = f * f * (3.0 - 2.0 * f);

    float y1 = mix(a, b, f2.x);
    float y2 = mix(c, d, f2.x);
    return mix(y1, y2, f2.y);
}

float fbm(vec2 p)
{
    float sum = 0.0;
    float amp = 0.5;
    for(int i=0; i<4; i++)
    {
        sum += amp * noise2D(p);
        p   *= 2.02;
        amp *= 0.5;
    }
    return sum;
}

// ---------------------------------------------------------------------
// 2) Axis-Aligned Box Intersection
//    We'll treat the "cube" as spanning [-1,+1] in x,y,z. 
//    Returns tHit < 0 if no intersection. Otherwise returns the nearest t.
// ---------------------------------------------------------------------
struct HitInfo {
    float t;
    vec3  normal;
};

HitInfo intersectBox(vec3 ro, vec3 rd)
{
    // ro, rd: ray origin & direction
    // The box is from -1 to +1 along each axis.
    // We'll do a slab intersection test.

    // If rd.x == 0 and ro.x < -1 or ro.x > 1 => no hit, etc.
    // For simplicity, we do the typical approach:
    HitInfo hit;
    hit.t = -1.0;
    hit.normal = vec3(0.0);

    // param range
    float tmin = -10000.0;
    float tmax =  10000.0;

    // For each axis:
    for(int i=0; i<3; i++)
    {
        float rdi = (i==0) ? rd.x : (i==1)? rd.y : rd.z;
        float roi = (i==0) ? ro.x : (i==1)? ro.y : ro.z;
        float lo  = -1.0;
        float hi  =  1.0;

        if(abs(rdi) < 1e-6)
        {
            // Ray is parallel to the slab in this axis.
            if(roi < lo || roi > hi) {
                // No intersection at all
                return hit; // t stays -1 => no hit
            }
        }
        else
        {
            float t1 = (lo - roi)/rdi;
            float t2 = (hi - roi)/rdi;
            // Ensure t1 <= t2
            if(t1 > t2) { float tmp=t1; t1=t2; t2=tmp; }
            if(t1 > tmin) tmin = t1;
            if(t2 < tmax) tmax = t2;
            if(tmin > tmax) {
                // No intersection
                return hit; 
            }
        }
    }

    // If we get here, we have [tmin, tmax] as the intersection range.
    // We'll pick the first positive intersection if possible:
    float tHit = (tmin > 0.0) ? tmin : tmax;
    if(tHit < 0.0) {
        // Ray goes away from box
        return hit; // no intersection
    }

    // Compute intersection normal by checking which face is hit.
    vec3 p = ro + rd * tHit;
    
    // We detect which coordinate of p is near ±1 to pick the normal:
    // find the axis of largest magnitude
    vec3 d = abs(p) - vec3(1.0);
    // whichever dimension is ~0 => that's the face
    if     (abs(d.x) < 1e-3 && p.x > 0.0) hit.normal = vec3( 1, 0, 0);
    else if(abs(d.x) < 1e-3 && p.x < 0.0) hit.normal = vec3(-1, 0, 0);
    else if(abs(d.y) < 1e-3 && p.y > 0.0) hit.normal = vec3( 0, 1, 0);
    else if(abs(d.y) < 1e-3 && p.y < 0.0) hit.normal = vec3( 0,-1, 0);
    else if(abs(d.z) < 1e-3 && p.z > 0.0) hit.normal = vec3( 0, 0, 1);
    else                                  hit.normal = vec3( 0, 0,-1);

    hit.t = tHit;
    return hit;
}

// ---------------------------------------------------------------------
// 3) Face-Local UV for Tiling
//    Given the intersection point p in [-1,+1], and the face normal,
//    we choose two coordinates from p for the "face UV", in [0..1] range.
// ---------------------------------------------------------------------
vec2 getFaceUV(in vec3 p, in vec3 n)
{
    // We treat each face as a 2D plane. 
    // For face normal near ±Z, map (x,y) => uv
    // For ±X, map (z,y) => uv
    // For ±Y, map (x,z) => uv
    vec2 uv;
    if(n.z > 0.5 || n.z < -0.5)
    {
        uv = p.xy; // map x,y
    }
    else if(n.x > 0.5 || n.x < -0.5)
    {
        uv = p.zy; // map z,y
    }
    else
    {
        uv = p.xz; // map x,z
    }
    
    // p is in [-1,+1], so shift and scale to [0,1]
    uv = uv * 0.5 + 0.5;
    return uv;
}

// ---------------------------------------------------------------------
// 4) Bumpy Tile Material (similar to the prior snippet), but 2D-based
//    We'll do a small function that returns: 
//      - the base color from tile+noise
//      - an approximate normal that includes "bump" from noise
// ---------------------------------------------------------------------

// For face-based bump mapping, we define a local tangent frame:
//   N  = face normal
//   Tx = tangent direction in-plane (dUV/dx axis)
//   Ty = bitangent direction in-plane (dUV/dy axis)
// We'll read the same fbm-based approach, do partial differences in UV
// space, then transform that to world space for shading.
struct TileResult {
    vec3 color;
    vec3 normal;
};

// Utility: build tangent frame from face normal
// (We pick a "primary axis" and build two orthogonal directions.)
void buildFrame(in vec3 N, out vec3 T, out vec3 B)
{
    // Any vector not parallel to N can serve as "T"
    if(abs(N.y) < 0.999) {
        T = normalize(cross(N, vec3(0,1,0)));
    } else {
        T = normalize(cross(N, vec3(1,0,0)));
    }
    B = cross(N, T);
}

// The tile-shading function, adapted from the previous snippet
TileResult getTileShading(in vec2 uv, in vec3 faceNormal)
{
    TileResult tr;
    
    // 1) Tiling
    float tileCount = 4.0;
    vec2 tiledUV    = uv * tileCount;
    vec2 tileIdx    = floor(tiledUV);
    vec2 tileFrac   = fract(tiledUV);
    
    // 2) Grout lines
    float groutThickness = 0.02;
    bool inGrout = (tileFrac.x < groutThickness || tileFrac.y < groutThickness ||
                    tileFrac.x > (1.0 - groutThickness) || tileFrac.y > (1.0 - groutThickness));
    if(inGrout)
    {
        // Dark color, normal unchanged
        tr.color  = vec3(0.05);
        // Just use the face normal for the grout (no bump)
        tr.normal = faceNormal;
        return tr;
    }

    // 3) Bump from noise
    float bumpScale = 0.2;

    // We'll offset uv by tile index to give each tile unique pattern:
    // multiply a bit for more "grain"
    vec2 noiseCoord = tileIdx + tileFrac*4.0;
    float bumpValue = fbm(noiseCoord * 2.0);

    // Local partial derivatives in uv space
    float eps  = 0.002;
    float valC = fbm(noiseCoord * 2.0);
    float valX = fbm((tileIdx + (tileFrac + vec2(eps,0.0))*4.0)*2.0);
    float valY = fbm((tileIdx + (tileFrac + vec2(0.0,eps))*4.0)*2.0);

    float dx = (valX - valC);
    float dy = (valY - valC);

    // Face normal in world space is faceNormal.
    // Build tangent frame:
    vec3 T, B;
    buildFrame(faceNormal, T, B);

    // Combine partial derivatives into a final normal
    // The "z" slope is "1/bumpScale" (small tilt => big z).
    vec3 bumpN = normalize(
        faceNormal * (1.0/bumpScale)
      + T          * dx
      + B          * dy
    );

    // 4) Base tile color
    vec3 baseColor = mix(vec3(0.4, 0.35, 0.3),
                         vec3(0.8, 0.75, 0.7),
                         bumpValue);

    tr.color  = baseColor;
    tr.normal = bumpN;
    return tr;
}

// ---------------------------------------------------------------------
// 5) Main Shader: Raytrace the box, compute shading with orbiting sun
// ---------------------------------------------------------------------
void main()
{
    // A) Set up a (fake) resolution so we can define UV from gl_FragCoord.
    //    No uniforms, so we just hardcode something.  Let's pick 800x600
    //    with an aspect ratio of 4/3.  In real usage, pass actual resolution.
    vec2 resolution = vec2(800.0, 600.0);
    float aspect    = resolution.x / resolution.y;

    // B) Compute normalized device coordinates [-1..+1] for the pixel
    vec2 ndc = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    ndc.x *= aspect;  // correct for aspect ratio

    // C) Define a simple pinhole camera:
    //    Eye at (0,0,2), looking toward (0,0,0), focal length ~1
    vec3 ro = vec3(0.0, 0.0, 2.0);  // Ray Origin
    float focal = 1.0;
    // Ray direction in camera space:
    vec3 rd = normalize(vec3(ndc.x, ndc.y, -focal));

    // D) Intersect the axis-aligned box that goes from -1..+1 in x,y,z
    HitInfo hit = intersectBox(ro, rd);

    if(hit.t < 0.0)
    {
        // No intersection => draw a sky color
        gl_FragColor = vec4(0.4, 0.7, 1.0, 1.0);
        return;
    }

    // Intersection point
    vec3 p = ro + rd*hit.t;

    // E) Get face-local UV
    vec2 uv = getFaceUV(p, hit.normal);

    // F) Evaluate the bumpy tile material
    TileResult tile = getTileShading(uv, hit.normal);

    // G) “Time” hack from gl_FragCoord to move the sun around the camera
    float t = 0.01 * (gl_FragCoord.x + gl_FragCoord.y);
    // Place the sun in a circle around the camera, slightly above
    // Eye is at (0,0,2). Let the sun revolve in XZ around that.
    vec3 lightPos = vec3(0.0, 0.5, 2.0) + vec3(cos(t), 0.0, sin(t))*3.0;

    // Vector from intersection to light
    vec3 L = normalize(lightPos - p);

    // H) Cast a shadow ray to see if we hit the box again.
    //    If the light is outside and we are inside the box,
    //    any re-hit indicates we're in shadow.
    float shadow = 1.0; // 1 => lit, 0 => in shadow
    {
        // slightly offset start
        vec3 shadowOrigin = p + tile.normal * 0.001;
        HitInfo shadowHit = intersectBox(shadowOrigin, L);
        // If we have a valid t < something large => shadowed
        // We only check if 0 < t < bigNumber. The box is of finite size.
        if(shadowHit.t > 0.0 && shadowHit.t < 1000.0)
        {
            // We re-hit the box => shadow
            shadow = 0.0;
        }
    }

    // I) Phong-like shading 
    //    Ambient + Diffuse + Specular from tile.normal
    vec3 N = tile.normal; // bumped normal
    vec3 V = normalize(-rd); // view dir (from intersection toward eye)
    
    float ambient  = 0.15;
    float diff     = max(dot(N, L), 0.0) * shadow;
    
    // specular
    vec3 R = reflect(-L, N);
    float specAngle = max(dot(R, V), 0.0);
    float specular  = pow(specAngle, 20.0) * shadow * 0.5;

    vec3 color = tile.color * (ambient + diff) + vec3(1.0)*specular;

    // J) Output final color
    gl_FragColor = vec4(color, 1.0);
}

