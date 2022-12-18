int depth = 16;

// Observer params
float focalLength = 2.0;
float distanceToGlobe = 4.0;

// Planet/globe params
float globeRadius = sqrt(3.0);
float seaLevel = 0.10;
float forestLevel = 0.12;
float snowLevel = 0.24;
float shininess = 64.0;
float featuresProminence = 0.33;
float rotationSpeed = 0.05;
vec3 seaColor = vec3(0.2, 0.2, 1.0);
vec3 beachColor = vec3(1.0, 0.75, 0.0);
vec3 forestColor = vec3(0.0, 0.5, 0.0);
vec3 snowColor = vec3(1.0);

// Sun params
vec3 sunDirection = normalize(vec3(1.0, 1.0, 1.0));

// Initial noise params
float spaceScale = sqrt(2.0);
float noiseScale = 1.0 / spaceScale;

// Precalculated values
float globeRadiusSquared = globeRadius * globeRadius; 
float distanceToGlobeSquared = distanceToGlobe * distanceToGlobe;
float c = distanceToGlobeSquared - globeRadiusSquared;
float scale = 
    (1.0 - noiseScale) / 
    (1.0 - pow(noiseScale, float(depth) + 1.0));
    
float PI = 3.1415926535897932384626433832795;

vec4 base(in vec3 v) {
    vec3 alias = 2.0 * fract(0.5 * v) - 1.0;
    vec3 a = abs(alias);
    vec3 s = smoothstep(0.0, 1.0, a);
    vec3 deriv = alias * (1.0 - a);
    vec3 factors = vec3(s.y * s.z, s.z * s.x, s.x * s.y);
    vec3 grad = deriv * factors;
    float w = s.x * factors.x;
    return vec4(grad * 6.0, w);
}

vec4 noise(in vec3 pos, mat3 matrix, vec3 displacement) {
    vec3 v = pos;
    mat3 m = matrix;
    float s = noiseScale;
    vec4 result = base(v);
    for (int i = 1; i < depth; i++) {
        v = spaceScale * matrix * v + displacement;
        vec4 r = base(v);
        result.w += s * r.w;
        result.xyz += r.xyz * m;
        m *= matrix;
        s *= noiseScale;
    }
    return result * scale;
}

vec4 shootGlobe(in vec3 ray) {
    float halfB = -distanceToGlobe * ray.z;
    float delta = halfB * halfB - c;
    float sqrtD = delta > 0.0 ? sqrt(delta) : halfB;
    float hitDistance = max(halfB - sqrtD, 0.0);
    return vec4(hitDistance * ray + vec3(0.0, 0.0, distanceToGlobe), min(hitDistance, 1.0));
}

vec3 tangent(in vec3 normal, in vec3 v) {
    float perpendicular = dot(normal, v);
    return v - perpendicular * normal;
}

vec3 calcNormal(in vec4 hitPosition, in vec4 noise) {
    vec3 normal = normalize(hitPosition.xyz);
    if (noise.w >= seaLevel) {
        vec3 t = tangent(normal, noise.xyz);
        normal = normalize(normal - featuresProminence * t);
    }
    return normal;
}

vec3 calcColor(in vec3 ray, in vec3 normal, in float noiseLevel) {
    return noiseLevel < seaLevel 
        ? seaColor + 2.0 * pow(max(dot(reflect(ray, normal), sunDirection), 0.0), shininess)
        : noiseLevel < forestLevel
            ? beachColor
            : noiseLevel < snowLevel
                ? forestColor
                : snowColor;
}

float calcShade(vec3 normal, float hit) {
    float l = dot(normal, sunDirection);
    return hit * max(l, 0.0);
}

mat3 rotation() {
    float angle = iTime * PI * rotationSpeed;
    float s = sin(angle);
    float c = cos(angle);
    return mat3(
        vec3(  c, 0.0,   s),
        vec3(0.0, 1.0, 0.0),
        vec3( -s, 0.0,   c)
    );
}

vec4 sampleColor(in vec2 xy, mat3 r, mat3 matrix, vec3 displacement) {
    vec3 ray = normalize(vec3(xy, -focalLength));
    vec4 hitPosition = shootGlobe(ray);
    hitPosition.xyz = r * hitPosition.xyz;
    vec4 noise = noise(hitPosition.xyz, matrix, displacement);
    vec3 normal = calcNormal(hitPosition, noise);
    normal *= r;
    vec3 color = calcColor(ray, normal, noise.w);
    float shade = calcShade(normal, hitPosition.w);
    return vec4(color * shade, 1.0);
}

vec2 pixelCoordinates(in vec2 xlv_TEXCOORD0, in float halfPixelSize) {
    float aspect = iResolution.x * halfPixelSize;
    return 2.0 * halfPixelSize * xlv_TEXCOORD0 - vec2(aspect, 1.0);
}

void main() {
    float halfPixelSize = 1.0 / iResolution.y;

    float x = iMouse.x * halfPixelSize * PI;
    float y = iMouse.y * halfPixelSize * PI;
    float rc = cos(x);
    float rs = sin(x);
    float dc = cos(y);
    float ds = sin(y);
    vec3 displacement = vec3(dc, dc + ds, ds);
    vec3 i = normalize(vec3(1.0, rc, rs));
    vec3 k = normalize(cross(i, i.zxy));
    vec3 j = normalize(cross(k, i));
    mat3 matrix = mat3(i, j, k);

    vec2 xy = pixelCoordinates(xlv_TEXCOORD0, halfPixelSize);
    mat3 r = rotation();
    
    gl_FragColor = 0.25 * (
        sampleColor(xy, r, matrix, displacement) + 
        sampleColor(xy + vec2(halfPixelSize, 0.0), r, matrix, displacement) + 
        sampleColor(xy + vec2(0.0, halfPixelSize), r, matrix, displacement) + 
        sampleColor(xy + vec2(halfPixelSize), r, matrix, displacement)
    );
}
