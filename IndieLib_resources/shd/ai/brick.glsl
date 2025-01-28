// Uniform variables with default values
        vec3 position=vec3(0.0, 0.0, 0.5);
uniform vec3 viewPosition = vec3(0.0, 0.0, -0.5);      // Camera position
uniform vec3 lightPosition = vec3(10.0, 10.0, 10.0);  // Light position
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);        // Light color
uniform vec3 brickColor = vec3(0.8, 0.3, 0.1);        // Brick color
uniform vec3 mortarColor = vec3(0.6, 0.6, 0.6);       // Mortar color
uniform float specularPower = 32.0;                   // Shininess factor
uniform float maxSteps = 100.0;                       // Maximum steps for ray marching
uniform float maxDistance = 50.0;                     // Maximum distance for ray marching
uniform float epsilon = 0.001;                        // Precision for ray marching
    vec3 bgColor = vec3(0.7, 0.8, 1.0); // sky-ish

// Signed distance function for a box
float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Combine lighting with brick pattern
vec3 calculateBrickPattern(vec3 p) {
    float scale = 2.0; // Adjust for box size and brick scale
    vec3 scaledCoords = p * scale;
    float pattern = mod(floor(scaledCoords.x) + floor(scaledCoords.y), 2.0);
    return mix(mortarColor, brickColor, pattern);
}

// Ray marching function
float march(vec3 ro, vec3 rd) {
    float dist = 0.0;
    for (int i = 0; i < int(maxSteps); i++) {
        vec3 p = ro + rd * dist;
        float d = sdBox(p, vec3(1.0, 1.0, 1.0)); // Box of size (1,1,1)
        if (d < epsilon) {
            return dist; // Hit
        }
        if (dist > maxDistance) {
            break; // Exceeded max range
        }
        dist += d; // Move along the ray
    }
    return -1.0; // No hit
}

// Normal calculation from distance field
vec3 getNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        sdBox(p + e.xyy, vec3(1.0)) - sdBox(p - e.xyy, vec3(1.0)),
        sdBox(p + e.yxy, vec3(1.0)) - sdBox(p - e.yxy, vec3(1.0)),
        sdBox(p + e.yyx, vec3(1.0)) - sdBox(p - e.yyx, vec3(1.0))
    ));
}

// Phong lighting calculation
vec3 calculateLighting(vec3 p, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(lightPosition - p);

    // Ambient component
    vec3 ambient = 0.2 * lightColor;

    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
    vec3 specular = spec * lightColor;

    return ambient + diffuse + specular;
}

void main() {
    vec3 ro = viewPosition; // Ray origin at the camera
    vec3 rd = normalize(position - viewPosition); // Ray direction

    // Perform ray marching
    float t = march(ro, rd);
    if (t < 0.0) {
        gl_FragColor = vec4(bgColor, 1.0); // No hit: return black
        return;
    }

    // Hit point and normal
    vec3 hitPoint = ro + rd * t;
    vec3 normal = getNormal(hitPoint);

    // Calculate color with brick pattern and lighting
    vec3 baseColor = calculateBrickPattern(hitPoint);
    vec3 lighting = calculateLighting(hitPoint, normal, -rd);

    gl_FragColor = vec4(baseColor * lighting, 1.0);
}

