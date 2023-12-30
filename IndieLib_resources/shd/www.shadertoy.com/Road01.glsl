
const float speed = 7.0;




float map_z(float z)
{
    return z + iTime * speed;
}
float get_x(vec3 p)
{
    return p.x + 1.2 * cos(map_z(p.z) * 0.1);
}

float f(float x, float z)
{
    return 0.5 * sin(map_z(z) * 0.2);
}

bool castRay(vec3 ro, vec3 rd, out float resT)
{
    const float mint = 0.001;
    const float maxt = 50.0;
    const float dt = 1.0;
    float lh = 0.0;
    float ly = 0.0;
    
    float t = mint;
    
    for (float t = mint; t < maxt; t += dt)
    {
        vec3 p = ro + rd * t;
        float h = f(p.x, p.z);
        if (p.y < h)
        {
            resT = t - dt + dt * (lh - ly) / (p.y - ly - h + lh);
            return true;
        }
        lh = h;
        ly = p.y;
    }
    
    return false;
}

vec3 getNormal(vec3 p)
{
    const float eps = 0.02;
    vec3 n = vec3(
        f(p.x - eps, p.z) - f(p.x + eps, p.z),
        2.0 * eps,
        f(p.x, p.z - eps) - f(p.x, p.z + eps));
    return normalize(n);  
}

vec3 getShading(vec3 p, vec3 n)
{
    return dot(n, vec3(0.0, 1.0, 0.0)) * vec3(0.2, 0.7, 0.3);
}

vec3 terrainColor(vec3 ro, vec3 rd, float t)
{
    vec3 p = ro + rd * t;
    vec3 p1 = vec3(p.xy, map_z(p.z));
    float z = p1.z;
    
    vec3 n = getNormal(p1);
    vec3 s = getShading(p1, n);
    
    float waver = abs(get_x(p));
    float midline = step(0.7, waver);
    float mix_road = 1.0 - midline;
    float mix_line = (1.0 - smoothstep(0.0, 0.0025, pow(waver, 2.5))) * step(0.7, mod(z, 1.5));
    s = mix(s, vec3(0.2, -0.1, 0.2), mix_road * 0.7);
    s = mix(s, vec3(0.7, 0.7, 0.7), mix_line * 0.7);
    return s;
}

vec3 skyColor(vec2 xlv_TEXCOORD0)
{
    vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
    return vec3(0.4, 0.6, 0.9 * uv.y);
}

void main( )
{
    float aspect = iResolution.x / iResolution.y;
    const float hfov = radians(30.0);
    const float tanhfov = tan(hfov);
    const float near = 0.1;
    const float far = 1.0;
    
    vec2 uv = xlv_TEXCOORD0.xy / (iResolution.xy * 0.5) - vec2(1.0, 1.0);
    float dx = tanhfov * uv.x / aspect;
    float dy = tanhfov * uv.y;
    
    vec3 viewRayDir = normalize(vec3(dx, dy, 1.0) * (far - near));
    
    float z = map_z(0.0);
    float bob = -0.3 + 0.025 * cos(z * 0.2);
    float swivel = 0.05 * sin(z * 0.1);
    mat4 inverseViewMatrix = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        swivel, bob, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0
        );
    
    float x = get_x(vec3(0.0, 0.0, 0.0));
    vec3 ro = vec3(-x, 2.0, 0.0);
    vec3 rd = (inverseViewMatrix * vec4(viewRayDir, 0.0)).xyz;
    float resT;
    
    if (castRay(ro, rd, resT))
    {
        gl_FragColor = vec4(terrainColor(ro, rd, resT), 1.0);
    }
    else
    {
        gl_FragColor = vec4(skyColor(xlv_TEXCOORD0), 1.0);
    }
}

