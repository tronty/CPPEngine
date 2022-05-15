// Noise functions from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + iMouse.x+0.1) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

float fbm(vec3 x) {
	float v = 0.0;
	float a = 0.5;
	vec3 shift = vec3(100);
	for (int i = 0; i < 5; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

float sdf(vec3 point) {
    float radius = fbm(point*2.0)-fbm((point+300.0)*50.0)/50.0;
    float asteroid = length(point-vec3(0.0, 1.0, -1.0))-radius;
    
    return asteroid;
}

float rayMarch(vec3 rayOrigin, vec3 rayDir) {
    float distMarched = 0.0;
    
    for (int i = 0; i < 100; i++) {
        float dist = sdf(rayOrigin);
        
        rayOrigin += rayDir * dist;
        distMarched += dist;
        
        if (dist < 0.0001 || dist > 100.0) {
            break;
        }
    }
    
    return distMarched;
}

vec3 getNormal(vec3 point) {
    float dist = sdf(point);
    vec3 norm = dist - vec3(
        sdf(point - vec3(0.01, 0.0, 0.0)),
        sdf(point - vec3(0.0, 0.01, 0.0)),
        sdf(point - vec3(0.0, 0.0, 0.01))
    );
    return normalize(norm);
}

float getShadow(vec3 rayOrigin, vec3 lightDir) {
    float distMarched = 0.0;
    float shadow = 1.0;
    
    for (float i = 0.0; i < 100.0; i++) {
        float dist = sdf(rayOrigin);
        
        rayOrigin += lightDir * dist;
        distMarched += dist;
        shadow = min(shadow, 32.0*dist/i);
        
        if (dist < 0.0001 || dist > 100.0) {
            break;
        }
    }
    
    return max(shadow, 0.33);
}

float getLight(vec3 point, vec3 cameraPos) {
    vec3 normal = getNormal(point);
    vec3 lightDir = normalize(vec3(cos(iMouse.x), sin(iMouse.y), 0.0));
    float light = getShadow(point+0.02*normal, lightDir);
    
    return clamp(dot(normal, lightDir) * light, 0.1, 1.0);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord-0.5*iResolution.xy)/iResolution.x;
    
    vec3 cameraPos = vec3(0.0, 1.0, -4.0);
    vec3 rayDir = vec3(uv.x, uv.y, 1.0);

    float dist = rayMarch(cameraPos, rayDir);
    vec3 point = rayDir * dist + cameraPos;
    
    vec3 col = vec3(0.0);
    
    if (dist < 100.0) {
        col = vec3(getLight(point, cameraPos));
    }
    
    fragColor = vec4(col,1.0);
}

