// render cloud by tomcat7479
/**
    Render complex mountain combined with sky light, mountain fog, cloud
*/

#define ROTATE_MATRIX mat2(0.6, -0.8, 0.8, 0.6)
#define PIXEL_COORD_REDUCTION_FACTOR 0.003         
#define MOUNTAIN_HEIGHT_AMPLIFICATION_FACTOR 120.0
#define RAYMARCH_ITER_COUNT 256
#define RAYMARCH_ITER_PRECISION 1e-4
#define SOFTSHADOW_ITER_COUNT 80



vec2 fix(in vec2 fragCoord) {
    return (2.0 * fragCoord.xy - iResolution.xy) / min(iResolution.x, iResolution.y);
}

mat3 setCamera(vec3 lookAt, vec3 ro, float theta) {
    vec3 z = normalize(lookAt - ro);
    vec3 up = vec3(sin(theta), cos(theta), 0.0);
    vec3 x = normalize(cross(z, up));
    vec3 y = normalize(cross(x, z));
    return mat3(x, y, z);
}

float random(in vec2 p) {
    vec2 q = 55.1876653 * fract(p * 10.1321513);
    return fract((q.x + q.y) * q.x * q.y);
}

// return noise value and noise gradient
vec3 noise(in vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    vec2 du = 6.0 * f * (1.0 - f);

    vec2 offset = vec2(1.0, 0.0);
    float a = random(i);
    float b = random(i + offset.xy);
    float c = random(i + offset.yx);
    float d = random(i + offset.xx);

    float noiseV = a + (b - a) * u.x * (1.0 - u.y) + (c - a) * (1.0 - u.x) * u.y + (d - a) * u.x * u.y;
    vec2 noiseG = du * (vec2(b - a, c - a) + (a - b - c + d) * u.yx);
    return vec3(noiseV, noiseG);
}

// Fractal Brownian motion: generator cloud
float fbm(vec2 p) {
    float res = 0.0;
    float factor = 0.5;
    for (int i = 0; i < 4; i++) {
        res += factor * noise(p).x;
        p = ROTATE_MATRIX * p * 2.0;
        factor *= 0.5;
    }
    return res;
}

// Low precision mountain sdf, use to determine the position of camera
float sdfMountainLowPrecision(in vec2 p) {
    vec2 q = PIXEL_COORD_REDUCTION_FACTOR * p;
    float sdf = 0.0;
    float scale = 1.0;
    vec2 grad = vec2(0.0);
    for (int i = 0; i < 4; i++) {
        vec3 noiseInfo = noise(q);
        grad += noiseInfo.yz;
        sdf += scale * noiseInfo.x / (1.0 + dot(grad, grad));
        q = ROTATE_MATRIX * q * 2.0;
        scale *= 0.5;
    }
    return MOUNTAIN_HEIGHT_AMPLIFICATION_FACTOR * sdf;
}

// Mid precision mountain sdf, use to raymarch 
float sdfMountainMidPrecision(in vec2 p) {
    vec2 q = PIXEL_COORD_REDUCTION_FACTOR * p;
    float sdf = 0.0;
    float scale = 1.0;
    vec2 grad = vec2(0.0);
    for (int i = 0; i < 8; i++) {
        vec3 noiseInfo = noise(q);
        grad += noiseInfo.yz;
        sdf += scale * noiseInfo.x / (1.0 + dot(grad, grad));
        q = ROTATE_MATRIX * q * 2.0;
        scale *= 0.5;
    }
    return MOUNTAIN_HEIGHT_AMPLIFICATION_FACTOR * sdf;
}

// High precision mountain sdf, use to cal normal, enhance mountain detail
float sdfMountainHighPrecision(in vec2 p) {
    vec2 q = PIXEL_COORD_REDUCTION_FACTOR * p;
    float sdf = 0.0;
    float scale = 1.0;
    vec2 grad = vec2(0.0);
    for (int i = 0; i < 16; i++) {
        vec3 noiseInfo = noise(q);
        grad += noiseInfo.yz;
        sdf += scale * noiseInfo.x / (1.0 + dot(grad, grad));
        q = ROTATE_MATRIX * q * 2.0;
        scale *= 0.5;
    }
    return MOUNTAIN_HEIGHT_AMPLIFICATION_FACTOR * sdf;
}

vec3 calNormal(vec3 p, float t) {
    vec2 delta = vec2(1e-3 * t, 0.0);
    float px = sdfMountainHighPrecision(p.xz - delta.xy) - sdfMountainHighPrecision(p.xz + delta.xy);
    float py = 2.0 * delta.x;
    float pz = sdfMountainHighPrecision(p.xz - delta.yx) - sdfMountainHighPrecision(p.xz + delta.yx);
    return normalize(vec3(px, py, pz));
}

float rayMarch(vec3 ro, vec3 rd, float tmin, float tmax) {
    float t = tmin;
    for (int i = 0; i < RAYMARCH_ITER_COUNT; i++) {
        vec3 p = ro + t * rd;
        float h = p.y - sdfMountainMidPrecision(p.xz);
        if (abs(h) < t * RAYMARCH_ITER_PRECISION || t > tmax) {
            break;
        }
        t += 0.4 * h;
    }   
    return t;
}

float softShadow(vec3 ro, vec3 rd, float dis) {
    float minStep = clamp(0.01 * dis, 0.5, 50.0);
    float res = 1.0;
    float t = 0.001;
    for (int i = 0; i < SOFTSHADOW_ITER_COUNT; i++) {
        vec3 p = ro + t * rd;
        float h = p.y - sdfMountainMidPrecision(p.xz);
        res = min(res, 8.0 * h / t);
        t += max(minStep, h);
        if (res < 0.001 || p.y > 200.0) {
            break;
        }
    }
    return clamp(res, 0.0, 1.0);
}

vec3 render(vec2 uv) {
    vec3 col = vec3(0.0);

    vec3 baseSkyCol = vec3(0.3, 0.5, 0.85);
    vec3 baseGraySkyBandCol = vec3(0.7, 0.75, 0.85);
    vec3 baseSunCol1 = vec3(1.0, 0.7, 0.4);
    vec3 baseSunCol2 = vec3(1.0, 0.8, 0.6);
    vec3 baseSunCol3 = vec3(1.0, 0.8, 0.7);
    float sunCol1Weight = 0.25;
    float sunCol2Weight = 0.25;
    float sunCol3Weight = 0.2;

    vec3 cloudCol = vec3(1.0, 0.95, 1.0);
    float cloudSparsity = 0.01;
    float cloudH = 300.0;
    
    vec3 baseMountainCol = vec3(0.67, 0.57, 0.44);
    vec3 baseMountainFogCol = vec3(0.5, 0.75, 1.0);

    vec3 softShadowAmplificationCof = vec3(8.0, 5.0, 3.0); 
    vec3 ambientColComponentCof = vec3(0.4, 0.6, 1.0);
    vec3 backgroundCompoentCof = vec3(0.4, 0.5, 0.6);

    float angle = 0.05 * iTime;
    float r = 300.0;
    float forward_offset = 0.01;
    vec2 px2 = vec2(r * sin(angle), r * cos(angle));
    float h = sdfMountainLowPrecision(px2) + 25.0;
    vec3 lookAt = vec3(r * sin(angle + forward_offset), h, r * cos(angle + forward_offset));
    vec3 ro = vec3(px2.x, h, px2.y);
    float focus_distance = 1.0;
    mat3 camera = setCamera(lookAt, ro, 0.0);
    vec3 rd = normalize(camera * vec3(uv, focus_distance));

    float tmin = 0.01;    // minimum iter distance of raymarch
    float tmax = 1000.0;  // maximum iter distance of raymarch
    float maxH = 300.0;   // maximum height of mountain
    
    // optimize: determine tmax and tmin by maximum height of mountain 
    float curMaxDistance = (maxH - ro.y) / rd.y; // distance of ro to highest mountain position
    if (curMaxDistance > 0.0) {
        if (maxH > ro.y) {
            tmax = min(tmax, curMaxDistance);
        } else {
            tmin = max(tmin, curMaxDistance);
        }
    }
    
    vec3 sunDir = normalize(vec3(0.8, 0.5, -0.2));
    float sunDot = clamp(dot(sunDir, rd), 0.0, 1.0);
    float t = rayMarch(ro, rd, tmin, tmax);
    
    float difCof = 0.1;

    // rendering
    if (t < tmax) {
        // rendering mountain
        vec3 p = ro + t * rd;
        vec3 n = calNormal(p, t);
        vec3 difCol = difCof * baseMountainCol;
        col = difCol; 

        vec3 linearColCof = vec3(0.0);
        float difDot = clamp(dot(n, sunDir), 0.0, 1.0);
        float softShadowCof = softShadow(p + 0.01 * sunDir, sunDir, t);
        float softCofX = softShadowCof;
        float softCofY = softShadowCof * softShadowCof * 0.5 + softShadowCof * 0.5;
        float softCofZ = softShadowCof * softShadowCof * 0.8 + softShadowCof * 0.2;
        float ambientCof = clamp(0.5 + 0.5 * n.y, 0.0, 1.0); // ambient occlusion cof
        float backgroundCof = clamp(0.2 + 0.8 * dot(vec3(-sunDir.x, 0.0, sunDir.z), n), 0.0, 1.0);
        linearColCof += difDot * softShadowAmplificationCof * vec3(softCofX, softCofY, softCofZ);
        linearColCof += ambientCof * ambientColComponentCof * 5.0; 
        linearColCof += backgroundCof * backgroundCompoentCof;

        col *= linearColCof;

        // add fog effect to remote mountains: remote mountain means bigger t
        col = mix(col, 0.75 * baseMountainFogCol, 1.0 - exp(-0.002 * t));
    } else {
        // rendering sky
        col = baseSkyCol - rd.y * rd.y * rd.y * 0.3;
        col = mix(col, 0.85 * baseGraySkyBandCol, pow(1.0 - max(rd.y, 0.0), 4.0));

        // rendering sun
        col += sunCol1Weight * baseSunCol1 * pow(sunDot, 16.0);
        col += sunCol2Weight * baseSunCol2 * pow(sunDot, 256.0);
        col += sunCol3Weight * baseSunCol3 * pow(sunDot, 512.0);

        // rendering cloud
        vec2 cloudPos = ro.xz + rd.xz * (cloudH - ro.y) / rd.y + iTime * 3.0;
        // Effect1: The effect of the cloud is relatively continuous and dense
        // col = mix(col, cloudCol, 0.5 * fbm(cloudSparsity * cloudPos)); 
        
        // Effect2: The effect of the cloud is relatively not continuous and not dense
        col = mix(col, cloudCol, 0.75 * smoothstep(0.4, 0.8, fbm(cloudSparsity * cloudPos)));
    }

    col += 0.3 * vec3(1.0, 0.7, 0.3) * pow(sunDot, 8.0);

    return col;
}

void main() {
    vec2 uv = fix(xlv_TEXCOORD0);
    vec3 col = render(uv);
    gl_FragColor = vec4(col, 1.0);
}

