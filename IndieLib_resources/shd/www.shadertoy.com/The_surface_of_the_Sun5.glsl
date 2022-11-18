uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
uniform sampler2D iChannel4;
float sphere (vec3 r, vec3 o, float radius){
    return length(o-r)-radius;
}
float ground (vec3 r){
    return r.y-1.5;
}
vec4 trace (vec3 o, vec3 r, vec3 pos, float radius){
    float t = 0.0;
    vec3 p;
    for(int i = 0; i<32; ++i){
        p = o+r*t;
        float d = sphere(p, pos, radius);
        t += d*0.5;
    }
    return vec4(p, t);
}
vec4 traceGround (vec3 o, vec3 r){
    float t = 0.0;
    vec3 p;
    for(int i = 0; i<32; ++i){
        p = o+r*t;
        float d = ground(p);
        t += d*0.5;
    }
    return vec4(p, t);
}
void main( )
{
    vec2 fragCoord=xlv_TEXCOORD0;
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv = uv * 2. - 1.;
    uv.x *= iResolution.x / iResolution.y;
    vec2 mouse = iMouse.xy / iResolution.xy;
    mouse.x*=3.;
    vec2 ray = vec2(uv.x, uv.y);//direction
    vec3 r = normalize(vec3(ray, 1.));//ray
    vec3 o = vec3(0., 0., 0.);//camera
    vec3 pos = vec3(0., 0., 1.);//position of sphere
    float radius = 0.6;
    vec4 t = trace(o, r, pos, radius);
    vec4 getPixel = texture2D(iChannel4, vec2(t.x-mouse.x, t.y)*1.0);
    float fog = 1.0 / (1.0 + t.w*t.w*0.5);//shadow at distance
    if(sphere(t.xyz, pos, radius)>0.2) {
        t = traceGround(o, r);
        getPixel = texture2D(iChannel1, vec2(t.x*0.1-mouse.x, t.y*50.)*1.0);
        fog = 1.0 / (1.0 + t.w*t.w*0.001);//shadow at distance
    }
    vec3 fc = getPixel.xyz*fog;
    gl_FragColor = vec4(fc, 1);
}
