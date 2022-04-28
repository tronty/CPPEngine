// For easily pasting this into a project I'm working on
#define time iTime

// Raymarching parameters
#define MAX_STEPS 300
#define NEAR_ENOUGH 0.001
#define TOO_FAR 20.0
#define NORMAL_DELTA 0.001

// Lighting parameters
#define DIFFUSE_FACTOR 1.
#define SPECULAR_FACTOR .7
#define SHININESS 6.

#define BACKGROUND_COLOR vec3(0., 0., 0.)
#define SURFACE_COLOR vec3(0.76471,0.78039,0.78039)
#define BLOOM_COLOR vec3(0.84314,0.67843,0.39216)

// Parameters
#define SMOOTH_FACTOR 0.3
#define BLOOM_STEP 0.04

float sphere(vec3 point, vec3 center, float radius) {
    return length(point - center) - radius;
}

float distance_from_everything(vec3 point) {
    // Base sphere
    float s = sphere(point, vec3(0, 0, 2.), 3.);
    // Add some ripples
    s += sin(20.* point.x * (cos(time)*.5+.5)) * sin(1. * point.z) * .1;
    return s;
}

float ray_march(vec3 ray_origin, vec3 ray_direction, out int steps) {
    // How far we've traveled
    float d = 0.0;
    steps = 0;
    for (int i = 0; i < MAX_STEPS; i++) {
        // Where we stand
        vec3 point = ray_origin + ray_direction*d;
        // How far anything is from us
        float current_distance = distance_from_everything(point);
        // March on
        d += current_distance;
        steps++;
        // Check status - have we reached a surface?
        if (current_distance < NEAR_ENOUGH || d > TOO_FAR)
            break;
    }
    return d;
}

// See https://iquilezles.org/articles/normalsSDF
vec3 estimate_normal(vec3 point) {
    vec2 e = vec2(NORMAL_DELTA, 0); // x smol, y none
    // Find normal as tangent of distance function
    return normalize(vec3(
        distance_from_everything(point + e.xyy) - distance_from_everything(point - e.xyy),
        distance_from_everything(point + e.yxy) - distance_from_everything(point - e.yxy),
        distance_from_everything(point + e.yyx) - distance_from_everything(point - e.yyx)
    ));
}

float phong_light(vec3 point, vec3 light_position, vec3 n, vec3 camera) {
    vec3 l = normalize(light_position-point);
    vec3 c = normalize(camera-point);
    float diffuse = max(dot(n, l), 0.);
    float specular = pow(max(dot(reflect(-l, n), -c), 0.), SHININESS);

    return DIFFUSE_FACTOR * diffuse + SPECULAR_FACTOR * specular;
}

vec3 lighting(vec3 point, vec3 camera, vec3 ray_direction, float dist, int steps) {
    // Avoid casting specular highlight from the void
    // (and blend in bloom)
    float bloom = .7* BLOOM_STEP * float(steps + 10);
    if (length(point - camera) > TOO_FAR*0.99)
        return BACKGROUND_COLOR + bloom * BLOOM_COLOR;

    float phong = phong_light(point, vec3(1, 1, -2), estimate_normal(point), camera);
    // Basic attempt at ambient occlusion
    phong = phong * smoothstep(4., 0., float(steps)/10.);
    return phong * SURFACE_COLOR + .7*sqrt(bloom) * BLOOM_COLOR;
}

void main( ) {
    vec2 xy = (xlv_TEXCOORD0 - 0.5*iResolution.xy) / iResolution.y;

    // Ray origin
    vec3 camera = vec3(0., 0, -4.);
    // Ray direction
    vec3 ray_direction = vec3(xy, 1.);

    int steps;
    float d = ray_march(camera, ray_direction, steps);
    
    vec3 point = camera + ray_direction * d;
    
    vec3 base_color = lighting(point, camera, ray_direction, d, steps);

    gl_FragColor = vec4(base_color, 1.);
}

