// ------------------------ PARAMETERS ------------------------

#define MAX_STEPS 100
#define MAX_DIST  100.0
#define EPS       0.001     // SDF/EPS for normal calculation, etc.

// "Rectangular Tower" parameters
const float R = 0.15;   // rounding radius

// Grid dimensions
const int Nx = 4;
const int Ny = 6;
const int Nz = 2;

// Each box’s half size
const vec3 boxHalf = vec3(0.4, 0.4, 0.4);

// Light parameters
const vec3  lightDir = normalize(vec3(0.5, 1.0, 0.3));
const vec3  lightColor = vec3(1.0, 0.95, 0.8); // warm-white

// Material parameters
const vec3  objectColor = vec3(0.4, 0.6, 0.8);
const float shininess   = 32.0;
const float specularStr = 0.3;

// ------------------------ SDF HELPERS ------------------------

// Signed distance to an axis-aligned box of half-size b
float sdBox(vec3 p, vec3 b)
{
    vec3 d = abs(p) - b;
    // Outside distance: length of positive part
    // Inside distance: largest negative component
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// Replace sdRoundTower with a 3D grid (N x M x K) of boxes
float sdRectangularTower(vec3 p)
{
    float d = 1e10;  // large initial distance

    // For each cell in a 3D grid, union the distance to that box
    for(int i = 0; i < Nx; i++)
    {
        for(int j = 0; j < Ny; j++)
        {
            for(int k = 0; k < Nz; k++)
            {
                // Center each box at integer coordinates, shifted by +0.5
                vec3 boxCenter = vec3(float(i)+0.5, float(j)+0.5, float(k)+0.5);
                float boxDist  = sdBox(p - boxCenter, boxHalf);
                d = min(d, boxDist);
            }
        }
    }

    // Round the edges by subtracting R
    return d - R;
}

// Our scene SDF (just one object in this example)
float mapScene(vec3 p)
{
    return sdRectangularTower(p);
}

// Approximate normal by sampling the gradient of the distance field
vec3 calcNormal(vec3 p)
{
    const vec2 e = vec2(EPS, 0.0);
    return normalize(vec3(
        mapScene(p + vec3(e.x, e.y, e.y)) - mapScene(p - vec3(e.x, e.y, e.y)),
        mapScene(p + vec3(e.y, e.x, e.y)) - mapScene(p - vec3(e.y, e.x, e.y)),
        mapScene(p + vec3(e.y, e.y, e.x)) - mapScene(p - vec3(e.y, e.y, e.x))
    ));
}

// Hard shadow by sphere tracing along the ray from intersection to light
float calcShadow(vec3 ro, vec3 rd)
{
    float t = 0.01; // offset a bit from the surface
    for(int i = 0; i < 64; i++)
    {
        if(t > MAX_DIST) break;
        float h = mapScene(ro + rd * t);
        if(h < 0.001)
        {
            // In shadow
            return 0.0;
        }
        t += h;
    }
    return 1.0;
}

// Sphere-trace the scene along ray origin (ro) and direction (rd).
// Returns the distance along the ray to the intersection, or -1.0 if none found.
float raymarch(vec3 ro, vec3 rd)
{
    float t = 0.0;
    for(int i = 0; i < MAX_STEPS; i++)
    {
        vec3 p = ro + rd * t;
        float distToScene = mapScene(p);
        if(distToScene < EPS) {
            return t; // we hit the surface
        }
        t += distToScene;
        if(t > MAX_DIST) break;
    }
    return -1.0; // no hit
}

// ------------------------ MAIN RENDER ------------------------

void main()
{
    // Normalized pixel coordinates (-1..1)
    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;

    // Simple rotating camera
    float ang = iTime * 0.3;
    vec3 ro   = vec3(6.0 * cos(ang), 3.0, 6.0 * sin(ang)); 
    vec3 lookAt = vec3(1.5, 2.0, 0.5); // aim somewhere roughly near the center
    vec3 ww = normalize(lookAt - ro);
    vec3 uu = normalize(cross(ww, vec3(0,1,0)));
    vec3 vv = cross(uu, ww);

    // Generate ray direction using a pinhole camera model
    vec3 rd = normalize(uu * uv.x + vv * uv.y + ww);

    // Raymarch
    float t = raymarch(ro, rd);
    vec3 color = vec3(0.9, 0.95, 1.0); // background sky color

    if(t > 0.0)
    {
        // We hit geometry
        vec3 p = ro + rd * t;
        vec3 n = calcNormal(p);

        // Basic Lambert + specular
        float shadowFactor = calcShadow(p, lightDir);
        float diff = max(dot(n, lightDir), 0.0) * shadowFactor;

        vec3 viewDir    = normalize(-rd);
        vec3 halfVector = normalize(lightDir + viewDir);
        float specAngle = max(dot(n, halfVector), 0.0);
        float spec = pow(specAngle, shininess) * specularStr * shadowFactor;

        vec3 lighting = lightColor * (diff + spec);
        color = objectColor * lighting;
    }

    gl_FragColor = vec4(color, 1.0);
}
