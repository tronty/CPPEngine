// https://www.shadertoy.com/view/sdK3W1
#define MAX_STEP 100
#define MAX_DIST 100.
#define SURF_DIST 0.001
#define Sat(a) clamp(a, 0., 1.)

float smin(float a, float b, float k){
    float h = Sat(0.5 + 0.5 * (b - a) / k);
    return mix(b, a, h) - k * h * (1. - h);
}

float sdSphere(vec3 p, vec4 s){
    return length(p - s.xyz) - s.w;
}

float sdBox(vec3 p, vec3 b){
    p = abs(p) - b;
    return length(max(vec3(0.), p)) + min(max(p.x, max(p.y, p.z)), 0.);
}

float GetDist(vec3 p){
    float sphere = sdSphere(p, vec4(0., 0., 0., 0.8));
    float box = sdBox(p - vec3(sin(iTime * 0.5)* 2., 0., 0.), vec3(0.5));
    float plane = p.y + 1.2;
    
    float d = smin(sphere, box, 0.3);
    
    d = min(plane, d);
    return d;
}

vec3 GetNormal(vec3 p){
    vec2 e = vec2(0.001, 0.);
    return normalize(vec3(
                        GetDist(p + e.xyy) - GetDist(p - e.xyy),
                        GetDist(p + e.yxy) - GetDist(p - e.yxy),
                        GetDist(p + e.yyx) - GetDist(p - e.yyx)
                    ));
}


float RayMarch(vec3 ro, vec3 rd){
    vec3 p = ro;
    float d = 0., sd = 0.;
    for(int i = 0; i < MAX_STEP; i++){
        d = GetDist(p);
        sd += d;
        if(d < SURF_DIST || sd > MAX_DIST) break;
        p = p + d * rd;
    }
    return sd;
}

void main( )
{
    vec2 uv = (xlv_TEXCOORD0 - iResolution.xy * 0.5)/iResolution.y;

    vec3 col = vec3(0);
    
    vec3 ro = vec3(0., 2., -8.);
    
    vec3 rd = normalize(vec3(uv.x, uv.y - 0.5, 2.));
    
    float d = RayMarch(ro, rd);
    
    if(d < MAX_DIST){
        vec3 p = ro + rd * d;
        vec3 lightPos = vec3(3., 4., -5.);
        vec3 N = GetNormal(p);
        vec3 L = normalize(lightPos - p);
        
        float dif = Sat(dot(N, L));
        
        d = RayMarch(p + N * SURF_DIST * 2., L);
        if(d < length(lightPos - p)) dif *= 0.5;
        
        col = vec3(dif);
        
    }

    gl_FragColor = vec4(col,1.0);
}
