struct Circle {
    vec3 center;
    float radius;
    vec4 color;
};

float Clamp(float a, float b, float x) {
    return max(a, min(b, x));
}

vec4 DrawCircle(vec2 uv, Circle c, bool AA) {
    float d = length(c.center.xy - uv) - c.center.z;
    float blur = AA ? smoothstep(1.0, 0.985, 1. - clamp(0., 1., length(d - c.radius) / c.radius)) : 1.;
    return d <= c.radius ? vec4(c.color.xyz, blur * c.color.w) : vec4(0.);
}

float N(vec2 i) {
    float t = tan(sin(i.x)) + cos(i.y);
    return fract(sin(t * 3456.) * 6547.);
}

vec4 depthSort(vec4 c1, float d1, vec4 c2, float d2) {
    c1.w = clamp(0., 1., c1.w);
    c2.w = clamp(0., 1., c2.w);
    if(length(c1) != 0. && length(c2) != 0.)
    {
        return d1 < d2 ? c1 + c2 * (1. - c1.w) : c2 + c1 * (1. - c2.w);
    }
    return c1 + c2;
}

vec4 FresnelCircle(vec2 uv, Circle c, float ammount) {
    float d = length(c.center.xy - uv);
    float r = c.radius + c.center.z;
    if(d > r) return vec4(0.);
    return vec4(pow(d / (r), ammount));
}

vec4 bloomCircle(vec2 uv, Circle c, float scale, float intensity, bool hollow) {
    float d = length(c.center.xy - uv);
    if(hollow && d < c.radius) return vec4(0.);
    d = clamp(0., 1., 1. - min(c.radius * scale, d - c.radius) / (c.radius * scale));
    return vec4(vec3(1.) * intensity * d * d, intensity * d * d);
}

vec4 combineColors(vec4 c1, vec4 c2) {
    return vec4(c1.xyz * c1.w + c2.xyz * c2.w, clamp(0., 1., c1.w + c2.w));
}

//Based on https://www.shadertoy.com/view/lsyfWD
vec3 PlanetTexture(vec2 uv, float distToEdge, float tscale)
{
    float speed = 0.5;
    vec2 scale = vec2(6., 2.) * tscale;
    vec2 p = uv * scale / iResolution.xy;
    
    for(float i = 1.; i < 10.; i += 1.) 
    {
        p.x += 0.3 / i * sin(i * 3.* p.y + iTime * speed) + iMouse.x / 1000.;
        p.y += 0.3 / i * cos(i * 3.* p.x + iTime * speed) + iMouse.y / 1000.;
    }
    
    float r = cos(p.x + p.y + 1.) * .3 + .5;
    float g = sin(p.x + p.y + 1.) *.5 + .5;
    float b = (sin(p.x + p.y) + cos(p.x + p.y)) * .8 + .5;
    vec3 color = 0.4 + vec3(r,g,b) * 0.25;
    return color * 3. * distToEdge;
}

float shadeCircle(vec2 uv, Circle c, vec2 dir, float scale, float offset) {
    dir = normalize(dir);
    float dist = c.radius;
    vec2 center = vec2(c.center.xy - dist * -offset * dir);
    return (scale / 2.) + max(1., scale) * max(0.0, (1. / dist - (length(uv - center) * dot(normalize(uv - center), dir) / dist)));
}

void main( )
{
    float t = mod(iTime / 2., 3.);
    vec2 uv = xlv_TEXCOORD0;
    float Z = iResolution.y / 10.;
    vec2 lightDir = vec2(-1., -1.);
    lightDir *= iMouse.z > 0. ? normalize(vec2(iMouse.x - iResolution.x / 2., iMouse.y - iResolution.y / 2.)) : vec2(1.);

    //Moon orbit motion
    float t_moon = t * 0.5;
    vec3 g = vec3(0., -1., 0.) * iResolution.y;
    vec3 A = g;
    vec3 Vi = vec3(1.0, 2.2 , -0.1) * iResolution.y;
    vec3 Pi = vec3(0.1, -0.1, 0.5) * vec3(iResolution.xy, Z);
    
    vec3 Vel = Vi + A * t_moon;
    vec3 Pos = vec3(Pi + Vel * t_moon + 0.5 * A * t_moon * t_moon);
    
    //Moon
    Circle m;
    m.center = Pos.xyz;
    m.radius = 0.1 * iResolution.y;
    float moonD = -m.center.z / Z;
    m.color = vec4(0.55, 0.5, 0.7, 1.) * (clamp(0., 1., 1.4 - moonD));
    vec4 moon = DrawCircle(uv, m, false);
    float moonR = m.radius + m.center.z;
    moon = combineColors(moon, 1.5 * FresnelCircle(uv, m, 5.) *  vec4(0.2, 0.4, 0.5, 0.5));
    
    float distToEdge1 = 2. * FresnelCircle(uv, m, 4.).w;
    moon.xyz *= 0.5 + PlanetTexture(uv - m.center.xy, distToEdge1, 10. * distToEdge1).xzy / 2.;
    m.radius = moonR;
    moon.xyz *= shadeCircle(uv, m, lightDir, 0.0, 0.2) / 2.;
    moon += bloomCircle(uv, m, 0.3, 0.9, true) * vec4(0.1, 0.15, 0.25, 0.2) * pow(shadeCircle(uv, m, lightDir, 0.0, 0.8), 2.);
    
    //Planet
    Circle w;
    w.center = vec3(0.5, -1., 0.0) * vec3(iResolution.xy, 65.);
    float planetD = -w.center.z / Z;
    w.radius = 1.5 * iResolution.y;
    w.color = vec4(0.5, 0.4, 0.3, 1.) * 1.;
    vec4 planet = DrawCircle(uv, w, true);
    
    Circle Atmosphere;
    Atmosphere.center = w.center - vec3(0., -2., 0.);
    Atmosphere.radius = w.radius;
    vec4 planetAtmosphere = FresnelCircle(uv, Atmosphere, 80.) + (bloomCircle(uv, Atmosphere, 0.015, 1., true));
    planetAtmosphere *= vec4(0.4, 0.6, 0.9, 1.);
    
    planet.xyz *= shadeCircle(uv, w, lightDir, 0.2, 0.2);
    planet.w = sign(planet.x);
    
    vec4 planetBloom = bloomCircle(uv, w, 0.4, 0.5, false) * vec4(0.4, 0.6, 0.9, 0.1);
    planetBloom *= shadeCircle(uv, Atmosphere, lightDir, 0.0, 0.9) * 1.;
    float distToEdge = FresnelCircle(uv, w, 12.).w;
    planet.xyz *= PlanetTexture(uv.yx, distToEdge, 2. * distToEdge) / 1.;
    planet += planetAtmosphere * max(0.1, shadeCircle(uv, Atmosphere, lightDir, 0.1, 0.0)) * 1.8;
    
    
    moon += planetBloom * moon / 2.;
    float shadow = Clamp(0.15, 1., 1. -dot(normalize(vec3(0, 1., 0.)), vec3(lightDir, 0.)) / 1.);
    moon.xyz *= shadow;
    
    //Stars
    vec4 stars = vec4(0.0);
    float n = N(uv);
    if(n >= 0.999)
    {
        stars += vec4(n * (.8 + sin(4. * iTime * N(uv.yx)) * 0.2));
    }
    
    vec4 planet_moon = depthSort(moon, moonD, planet, planetD);
    vec4 planets_bloom = depthSort(planet_moon, 0.0, planetBloom, 1.);
    vec4 planets_stars = depthSort(planets_bloom, 0.0, stars, 1.);
    gl_FragColor = planets_stars;
}
