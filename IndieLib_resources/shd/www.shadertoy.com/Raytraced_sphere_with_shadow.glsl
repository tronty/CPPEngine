vec3 light = vec3(2, 4, 15);
vec3 eye = vec3(0, 0, 8);
vec3 screenCenter = vec3(0, 0, 3);

vec3 sky = vec3(0.5, 0.5, 0.6);

float iterations = 30.0;
int depth = 2;
int AA = 2;

bool is_over_plane(vec3 p) {
    return dot(
        p - vec3(0, 0, -1.5),
        vec3(0, 1.0 / sqrt(2.0), 1.0 / sqrt(2.0))
    ) > 0.0;
}

float density(vec3 p) {
    return is_over_plane(p) ? 1.0 / dot(p, p) : 1.0;
}

vec3 normal(vec3 point) {
    if (is_over_plane(point)) {
        return normalize(point);
    } else {
        return vec3(0, 1.0 / sqrt(2.0), 1.0 / sqrt(2.0));
    }
}

vec3 intersection_3(vec3 p1, vec3 p2) {
    for (float i = 0.0; i <= iterations; i += 1.0) {
        vec3 p = mix(p1, p2, i / iterations);
        if (density(p) >= 1.0) {
            return p;
        }
    }
}

vec3 intersection_2(vec3 p1, vec3 p2) {
    for (float i = 0.0; i <= iterations; i += 1.0) {
        vec3 p = mix(p1, p2, i / iterations);
        if (density(p) >= 1.0) {
            return intersection_3(mix(p1, p2, (i - 1.0) / iterations), p);
        }
    }
}

vec3 intersection_1(vec3 p1, vec3 p2) {
    for (float i = 0.1; i <= iterations; i += 1.0) {
        vec3 p = mix(p1, p2, i / iterations);
        if (density(p) >= 1.0) {
            return intersection_2(mix(p1, p2, (i - 1.0) / iterations), p);
        }
    }
    
    return vec3(0);
}

float max_density_2(vec3 p1, vec3 p2) {
    float mv = 0.0;

    for (float i = 0.0; i <= iterations; i += 1.0) {
        vec3 p = mix(p1, p2, i / iterations);
        mv = max(mv, density(p));
    }
    
    return min(mv, 1.0);
}

float max_density_1(vec3 p1, vec3 p2) {
    float mi = 1.0;
    float mv = 0.0;

    for (float i = 2.0; i <= iterations; i += 1.0) {
        vec3 p = mix(p1, p2, i / iterations);
        float v = density(p);
        if (v > mv) {
            mi = i;
            mv = v;
        }
    }
    
    return max_density_2(
        mix(p1, p2, (mi - 1.0) / iterations),
        mix(p1, p2, (mi + 1.0) / iterations)
    );
}


void main( ) { 
	float screenUnit = min(iResolution.x, iResolution.y) / 2.0;
    
    float l = 15.0;
    float brightness = 0.0;
    
    
    for (int ray = 0; ray < AA * AA; ray++) {
        vec2 shift = vec2(
        	float(ray / AA) / float(AA),
            float(ray % AA) / float(AA)
        );
        vec3 p1 = vec3((xlv_TEXCOORD0 - iResolution.xy * 0.5 + shift) / screenUnit, 0) + screenCenter;
        vec3 p2 = eye + normalize(p1 - eye) * l;

        float multiplier = 1.0;

        for (int k = 0; k < depth; k++) {
            vec3 point = intersection_1(p1, p2);

            if (dot(point, point) != 0.0) {
                vec3 normal = normal(point);
                bool is_object = is_over_plane(point);

                float shadow = 1.0 - pow(max_density_1(point, light), 8.0);
                float reflection = pow(max(0.0, dot(
                    normalize(reflect(light - point, normal)),
                    normalize(p2 - p1)
                )), 100.0) * 0.25;

                brightness += ((
                    max(0.0, dot(normal, normalize(light - point)) * 0.5)
                    + reflection
                ) * shadow + 0.01) * multiplier;

                multiplier *= 0.1;
                p2 = normalize(reflect(point - p1, normal)) * l + point;
                p1 = point;
            } else {
                brightness += 0.33 * multiplier;
                break;
            }
        }
    }

    gl_FragColor = vec4(vec3(pow(brightness / float(AA * AA), 1.0 / 2.4)), 1);
}

