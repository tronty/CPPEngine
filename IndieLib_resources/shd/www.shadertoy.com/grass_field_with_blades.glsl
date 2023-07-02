// credits:
//
//    ollj: suggested lower-iteration fractal terrain hull to speed up traversal
//    iq: suggested breaking shading code out of the raymarch loop for speedup
//        also suggested fresnel with low-iter terrain normal for improved visual

// hash function credit: https://www.shadertoy.com/view/4djSRW
vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy+p3.yzz)*p3.zyx);
}

float hash12(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

vec4 hash42(vec2 p)
{
	vec4 p4 = fract(vec4(p.xyxy) * vec4(.1031, .1030, .0973, .1099));
    p4 += dot(p4, p4.wzxy+33.33);
    return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

float opSubtraction( float d1, float d2 ) { return max(d1,-d2); }

vec2 opRepeat(vec2 p, vec2 period, out vec2 outId)
{
    outId = floor((p+0.5*period)/period);
    return mod(p+0.5*period, period) - 0.5*period;
}

float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}

float sdGrassBlade2d(vec2 p)
{
    float dist = sdCircle(p - vec2(1.7, -1.3), 2.0);
    dist = opSubtraction(dist, sdCircle(p - vec2(1.7, -1.0), 1.8));
    dist = opSubtraction(dist, p.y + 1.0);
    dist = opSubtraction(dist, -p.x + 1.7);
    return dist;
}

float sdSphere(vec3 p)
{
    return length(p) - 1.0;
}

float sdGrassBlade(vec3 p, float thickness)
{
    p -= vec3(0, 1.0, 0);
    float dist2d = max(0.0, sdGrassBlade2d(p.xy));
    return sqrt(dist2d*dist2d + p.z*p.z) - thickness;
}

mat2 rotate2d(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    return mat2(
        c, -s,
        s, c
    );
}

float noise(vec2 p)
{
    vec2 ip = floor(p);
    vec2 fp = fract(p);
    float a = hash12(ip);
    float b = hash12(ip + vec2(1, 0));
    float c = hash12(ip + vec2(0, 1));
    float d = hash12(ip + vec2(1, 1));
    
    vec2 t = smoothstep(0.0, 1.0, fp);
    return mix(mix(a, b, t.x), mix(c, d, t.x), t.y);
}

float fbm(vec2 p, int octaveCount)
{
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < octaveCount; ++i)
    {
        value += amplitude * noise(p);
        p *= rotate2d(0.45);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

float calcTerrainHeight(vec2 p)
{
    return 20.0*fbm(0.02*p, 10);
}

float approxTerrainHeight(vec2 p)
{
    return 20.0*(fbm(0.02*p, 2) + 0.25);
}

vec3 calcTerrainNormal(vec2 p, float heightAtP)
{
    vec2 e = vec2(0, 0.001);
    vec3 forward = vec3(0.0, calcTerrainHeight(p + e.xy) - heightAtP, e.y);
    vec3 right = vec3(e.y, calcTerrainHeight(p + e.yx) - heightAtP, 0.0);
    return normalize(cross(forward, right));
}

vec3 approxTerrainNormal(vec2 p)
{
    vec2 e = vec2(0, 0.001);
    float approxHeightCenter = approxTerrainHeight(p);
    vec3 forward = vec3(0.0, approxTerrainHeight(p + e.xy) - approxHeightCenter, e.y);
    vec3 right = vec3(e.y, approxTerrainHeight(p + e.yx) - approxHeightCenter, 0.0);
    return normalize(cross(forward, right));
}

float map(vec3 p, inout float terrainHeightAtP)
{
    float distToTerrain = 0.5 * (p.y - terrainHeightAtP);

#if 1
    float guard = 1.1;
    if (distToTerrain > guard)
    {
        return distToTerrain - (guard - 1.0);
    }
#endif
    
    terrainHeightAtP = calcTerrainHeight(p.xz);
    distToTerrain = 0.5 * (p.y - terrainHeightAtP);
    vec3 originalP = p;

    vec2 grassId;
    float repeatPeriod = 0.25;
    p.xz = opRepeat(p.xz, vec2(repeatPeriod), grassId);

    float dist = 10e31;


#define NEIGHBOR_CHECK_COUNT 1
    for (int dy = -NEIGHBOR_CHECK_COUNT; dy <= NEIGHBOR_CHECK_COUNT; ++dy)
    {
        for (int dx = -NEIGHBOR_CHECK_COUNT; dx <= NEIGHBOR_CHECK_COUNT; ++dx)
        {
            vec3 neighborP = p - vec3(dx, 0, dy) * repeatPeriod;
            vec2 neighborId = grassId + vec2(dx, dy);
            
            // HORRIBLE HACK:
            //    assume all neighbors share the same terrain height
            //    this produces minimal visual artifact and cuts rendering time more than half
            //float terrainHeight = calcTerrainHeight(neighborId*repeatPeriod);
            float terrainHeight = terrainHeightAtP;
            neighborP.y -= terrainHeight;
#if 1
            // transform randomization
            vec4 rand = hash42(neighborId);
            neighborP.xz *= rotate2d(rand.z*6.28);
            neighborP.xz += (rand.xy - 0.5) * repeatPeriod;
#endif
            dist = min(dist, sdGrassBlade(neighborP/sqrt(rand.w), 0.002));
        }
    }
    
    dist = min(dist, distToTerrain);
    
    return dist;
}

vec3 getSkyCol(vec3 rd)
{
    vec3 skyCol = vec3(0.6, 0.7, 0.8);
    return mix(skyCol, 0.5*skyCol, rd.y);
}

vec3 render(vec2 uv, vec2 pixelSize)
{
#if 0
    float cameraAngle = 3.1415926 + 2.0 * 3.1415926 * iMouse.x / 800.0;
    vec3 ro = 4.0 * vec3(cos(cameraAngle), 0.8, sin(cameraAngle));
    vec3 at = vec3(0, 1.5, 0);
#else
    float time = 45.0 + 15.0 * iTime;
    vec3 ro = vec3(0.0, 18.2, -2.0 + time);
    ro.y = max(approxTerrainHeight(ro.xz) + 5.0, ro.y);
    vec3 at = ro + vec3(0, -0.4, 2.0);
#endif
    float filmDist = 1.3;
    vec3 camera_z = normalize(at - ro);
    vec3 camera_x = normalize(cross(vec3(0,1,0), camera_z));
    vec3 camera_y = cross(camera_z, camera_x);
    vec3 rd = normalize(camera_x * uv.x + camera_y * uv.y + filmDist * camera_z);
    
    vec3 skyCol = 1.1*getSkyCol(rd);
    vec3 sunCol = vec3(1.2);
    vec3 l = normalize(vec3(-0.5, 0.9, -0.0));
    
    // TODO: atmospheric scattering
    vec3 col = skyCol;
    
    bool hit = false;
    float t = 0.0;
    for (int i = 0; i < 256; ++i)
    {
        if (rd.y > 0.0) break; // early-out if ray is going upward since no way it hits the ground
        
        vec3 p = ro + t * rd;
        float terrainHeight = approxTerrainHeight(p.xz);
        // NOTE: map() updates terrainHeight to exact height if ray penetrates terrain guard
        float dist = map(p, terrainHeight);
        
        vec2 projectedPixelSize = pixelSize / filmDist * t;
        if (dist < 0.5*projectedPixelSize.x) // simple LOD: loosen check as point gets farther away
        {
            hit = true;
            break;
        }
        t += dist;
    }
    
    if (hit)
    {
        vec3 p = ro + t*rd;
        vec3 youngGrassCol = vec3(0.3, 0.35, 0.15);
        vec3 oldGrassCol = vec3(0.32, 0.35, 0.15);
        float colBlend = smoothstep(0.4, 0.3, fbm(0.2*p.xz, 2));
        vec3 grassCol = mix(youngGrassCol, oldGrassCol, colBlend);

        float terrainHeight = calcTerrainHeight(p.xz);
        float heightFromTerrain = p.y - terrainHeight;
        vec3 n = calcTerrainNormal(p.xz, terrainHeight);
        float directLight = max(0.0, dot(n, l));
        float ao = 0.3 + 0.7*pow(heightFromTerrain, 0.5);
        vec3 sampleCol = grassCol * (sunCol*directLight + 0.1*skyCol) * ao;
        // fresnel idea suggested by iq
        vec3 fresnel = vec3(0.2,0.2,0.1)*ao*pow(clamp(1.0 + dot(approxTerrainNormal(p.xz), rd),0.0,1.0), 3.0);
        //float tweakA = -iMouse.z/800.0;
        //float tweakB = -iMouse.w/450.0;
        col = 0.74*sampleCol + 0.82*fresnel;
    }
    
    vec3 fogExp = -0.25*vec3(0.01, 0.012, 0.01);
    col = mix(skyCol, col, exp(fogExp*t));
    
#if 1
    // HACK garbage formula color tuning
    col = smoothstep(0.0, 1.0, col);
    col *= 1.0;
#else
    //col = sqrt(col);
#endif
    return col;
}

void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = xlv_TEXCOORD0/iResolution.xy;

    uv = 2.0 * uv - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    vec2 pixelSize = vec2(2.0) / iResolution.xy;

#if 1
    vec3 col = render(uv, pixelSize);
#else
    vec3 col = vec3(0);
    for (int dy = 0; dy <= 1; ++dy)
    {
        for (int dx = 0; dx <= 1; ++dx)
        {
            col += 0.25 * render(uv + pixelSize*vec2(dx, dy), pixelSize);
        }
    }
#endif

    gl_FragColor = vec4(col,1.0);
}

