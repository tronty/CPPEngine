const float DENSITY = 4.5;
float hash(vec3 p3) {
	p3  = fract(p3 * vec3(.1031,.11369,.13787));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}
vec3 grad(vec3 p) {
    return normalize(-1.0 + 2.0 * vec3(hash(p.xyz), hash(p.yxy), hash(p.zyx)));
}
vec3 fade(vec3 p, vec3 corner) {
    vec3 t = abs(p - corner);
    return vec3(1.0) - (6.0*t*t*t*t*t - 15.0*t*t*t*t + 10.0*t*t*t);
}
float perlin(vec3 p) {
    p *= DENSITY;
    vec3 min_corner = floor(p);
    vec3 local = fract(p);
    float ret = 0.0;
    for (int dx=0; dx<=1; ++dx) {
        for (int dy=0; dy<=1; ++dy) {
            for (int dz=0; dz<=1; ++dz) {
                vec3 corner = min_corner + vec3(dx, dy, dz);
                vec3 g = grad(corner);
                vec3 d = local - vec3(dx, dy, dz);
                vec3 f = fade(p, corner);
                ret += dot(g, d) * f.x * f.y * f.z;
            }
        }
    }
    return ret;
}
float fbm(vec3 p) {
    float ret = 0.0;
    float amp = 1.0;
    for (int i=0; i<4; ++i) {
        ret += amp * perlin(p);
        p *= 2.0;
        amp *= 0.5;
    }
    return ret;
}
vec4 rayMarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    vec4 color = vec4(0.);
    for(int i = 0; i < 50; i++) {
        vec3 p = ro + t * rd;
        float sdf = length(p) - 1.;
        float noise = fbm(p * 0.45 + iTime * 0.45);
        noise = (noise + 0.7) / 3.;
        float density = pow(7.2, -1.66*sdf) * noise;

        if(density > 1e-3) {
            vec4 c = vec4(
                mix(
                    vec3(1.0, 0.19, 0.0), 
                    vec3(1.0, 0.91, 0.0),
                density), density);
            c.a *= 0.2;
            c.rgb *= c.a;
            color += c * (1.0 - color.a);
        }
        t += sdf;
    }
    return color;
}

void main( ) {
    vec2 uv = (2.0 * xlv_TEXCOORD0 - iResolution.xy) 
        / min(iResolution.y, iResolution.x);
    
    vec3 ro = vec3(0.0, 0.0, -2.0);
    vec3 rd = normalize(vec3(uv, 1.0));  
    gl_FragColor = rayMarch(ro, rd);
}

