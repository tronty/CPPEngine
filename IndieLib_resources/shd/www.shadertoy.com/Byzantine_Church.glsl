const float infinity = 999.9; // 1.0 / 0.0;
const float pi = atan(1.0, 1.0) * 4.0;
const float epsilon = 0.001;

// https://iquilezles.org/articles/distfunctions/

float ndot(vec2 a, vec2 b){
    return a.x*b.x - a.y*b.y;
}

float cylinder(vec3 p, float r, float h){
    vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(r, h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float box(vec3 p, vec3 b){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float rhombus(vec3 p, float la, float lb, float h){
    p = abs(p);
    vec2 b = vec2(la,lb);
    float f = clamp( (ndot(b,b-2.0*p.xz))/dot(b,b), -1.0, 1.0 );
    vec2 q = vec2(length(p.xz-0.5*b*vec2(1.0-f,1.0+f))*sign(p.x*b.y+p.z*b.x-b.x*b.y), p.y-h);
    return min(max(q.x,q.y),0.0) + length(max(q,0.0));
}

float solidangle( vec3 p, vec2 c, float ra ){
    vec2 q = vec2( length(p.xz), p.y );
    float l = length(q) - ra;
    float m = length(q - c*clamp(dot(q,c),0.0,ra) );
    return max(l,m*sign(c.y*q.x-c.x*q.y));
}

float prism(vec3 p, vec2 h){
    vec3 q = abs(p);
    return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}

float arch(vec3 p, float r, float h, float d){
    return min(cylinder(p.xzy, r, d), box(p + vec3(0.0, h, 0.0), vec3(r, h, d)));
}

float arches(vec3 p){
    float t = infinity;
    t = min(t, arch(p.zyx - vec3(0.0, -0.5, 2.6), 0.6, 1.0, 1.0));
    t = min(t, arch(p.zyx - vec3(1.25, -0.5, 2.6), 0.17, 0.15, 1.0));
    t = min(t, arch(p.zyx - vec3(0.0, 1.5, 1.8), 0.07, 0.1, 1.0));
    t = min(t, cylinder(p.zxy - vec3(0.0, 2.6, 0.5), 0.14, 1.0));
    return t;
}

float material1(vec3 p){
    float t = infinity;
    t = min(t, box(p + vec3(0.0, 0.8, 0.0), vec3(1.8, 0.7, 1.8)));
    t = min(t, box(p, vec3(1.0, 1.1, 1.0)));
    t = min(t, rhombus(p.yzx + vec3(0.1, 0.0, 0.0), 0.6, 1.8, 1.8));
    t = min(t, cylinder(p - vec3(0.0, 0.5, 0.0), 0.9, 1.3));
    t = min(t, prism(p.xzy - vec3(0.0, 0.0, 2.5), vec2(0.03, 0.3)));
    t = min(t, prism(p.xyz - vec3(0.0, 2.6, 0.0), vec2(0.03, 0.2)));
    t = min(t, prism(p.zyx - vec3(0.0, 2.6, 0.0), vec2(0.03, 0.2)));
    p.xz = abs(p.xz);
    if(p.x < p.z) p.xz = p.zx;
    t = min(t, box(p + vec3(0.0, 0.5, 0.0), vec3(1.8, 1.0, 1.0)));
    t = min(t, rhombus(p.yxz - vec3(0.5, 0.0, 0.0), 0.5, 1.0, 1.8));
    t = max(t, -arches(p));
    return t;
}

float slate(vec2 p){
    float n = 0.0;
    n += abs(sin(p.y * 23.0));
    //n += fract(p.x * 7.0) * 3.0;
    n += pow(cos(p.x * 40.0) * 0.5 + 0.5, 0.4) * 0.5;
    return n * 0.02;
}

float roof(vec3 p, float w, float h, float d){
    float s = slate(p.xz);
    return max(
        rhombus(p.yzx, h, w, d) - s,
        -rhombus(p.yzx + vec3(0.07, 0.0, 0.0), h, w, d + 1.0)
    );
}

float material2(vec3 p){
    float t = infinity;
    t = min(t, solidangle(p - vec3(0.0, 1.1, 0.0), vec2(sin(1.0), cos(1.0)), 1.15) - slate(vec2(atan(p.x, p.z), length(p.zx))));
    vec3 q = p;
    q.xz = abs(q.xz);
    if(q.x > q.z) q.xz = q.zx;
    t = min(t, roof(q + vec3(0.0, -0.5, 0.0), 1.05, 0.5, 1.85));
    t = min(t, max(roof(p.xyz + vec3(0.0, 0.1, 0.0), 1.85, 0.6, 1.85), -box(q, vec3(0.95, 1.5, 3.0))));    
    return t;
}

float map(vec3 p){
    float a = material1(p);
    float b = material2(p);
    return min(a, b);
}

vec3 nor(vec3 p){
    float e = 0.001;
    return normalize(
        map(p) - vec3(map(p - vec3(e, 0.0, 0.0)),
                      map(p - vec3(0.0, e, 0.0)),
                      map(p - vec3(0.0, 0.0, e)))
    );
}

vec4 surface(sampler2D t, vec3 p, vec3 n){
    vec4 cx = texture(t, p.yz);
    vec4 cy = texture(t, p.xz);
    vec4 cz = texture(t, p.xy);
    vec3 m = n * n;
    return (cx * m.x + cy * m.y + cz * m.z) / (m.x + m.y + m.z);
}

void mat(vec3 p, out vec3 normal, out vec3 colour){
    float a = material1(p);
    float b = material2(p);
    normal = nor(p);
    colour = a < b ? surface(iChannel0, p, normal).rgb
                   : surface(iChannel1, p, normal).rgb;
}

float march(vec3 origin, vec3 direction){
    float t = 0.0;
    for(int i = 0; i < 100; i++){
        if(t > 10.0) return infinity;
        float d = map(origin + direction * t);
        if(d < epsilon) return t;
        t += d;
    }
    return -1.0;
}

vec3 background(vec3 direction){
    vec3 a = vec3(0.9, 0.9, 0.9);
    vec3 b = vec3(0.6, 0.8, 1.0);
    return mix(a, b, direction.y * 0.5 + 0.5);
}

vec3 gen(vec2 p, float fov, vec3 from, vec3 to){
    p = p / iResolution.xy * 2.0 - 1.0;
    p.x *= iResolution.x/iResolution.y;
    vec3 n = normalize(vec3(p, -1.0 / tan(fov / 2.0))); 
    vec3 z = normalize(from-to);
    vec3 x = normalize(cross(vec3(0.0, 1.0, 0.0), z));
    vec3 y = cross(z, x);
    return x * n.x + y * n.y + z * n.z;
}

void mainImage(out vec4 c, in vec2 p){
    
    float theta, phi;
    if(iMouse.z > 0.0){
        theta = iMouse.x/iResolution.x * 2.0 * pi;
        phi = iMouse.y/iResolution.y * pi;
    }else{
        theta = -iTime/4.0;
        phi = 1.2;
    }

    vec3 origin  = vec3(
        cos(theta) * sin(phi),
        cos(phi),
        sin(theta) * sin(phi)
    ) * 5.0;

    vec3 direction = gen(p, 1.4, origin, vec3(0.0));
        
    float t = march(origin, direction);
    
    if(t < 0.0){
    
        c.rgb = vec3(0.0);
    
    }else if(t < infinity){
    
        vec3 light1 = normalize(vec3(23.0, 20.0, 17.0));
        // light1.xy *= mat2(cos(theta), -sin(theta), sin(theta), cos(theta));
        vec3 light1_col = vec3(1.0, 0.9, 0.8);
        vec3 light2 = normalize(vec3(0.5, 1.0, -0.8));
        vec3 light2_col = vec3(0.5, 0.6, 0.8);
        vec3 light3 = normalize(vec3(-1.0, -0.4, -0.4));
        vec3 light3_col = vec3(0.3, 0.3, 0.4);

        vec3 normal, colour;
        mat(origin + direction * t, normal, colour);
        
        float s = march(origin + direction * (t - 0.05), light1) == infinity ? 1.0 : 0.0;
        c.rgb = colour * max(0.0, dot(normal, light1)) * s * light1_col;
        c.rgb += colour * max(0.0, dot(normal, light2)) * light2_col;
        c.rgb += colour * max(0.0, dot(normal, light3)) * light3_col;
        
    }else{
    
        c.rgb = background(direction);
        
    }
    
    c.a = 1.0;

}

