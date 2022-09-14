
// Back to Fractal City
// welcome to intricate concrete jungle
// there are some lipschitz discontinuity turbulences ahead
//
// previous fractal city iterations:
// https://www.shadertoy.com/view/ftG3D1
// https://www.shadertoy.com/view/NdyXDt
// https://www.shadertoy.com/view/WdlyWH

// parameters
const float speed = .05;
const float falloff = 1.75;
const float range = .5;
const float count = 11.;

// globals
vec3 pp, ppp;
float mat, glow;

float map (vec3 p)
{
    float dist = 100.;
    float shape = 100.;
    vec3 pos = p;
    pp = p;
    ppp = p;
    
    // travel
    float px = p.x-iTime*speed;
    float ix = floor(px);
    p.x = repeat(px,1.);
    
    // city
    float a = 1.;
    float angle = 196.+ix;
    for (float i = 0.; i < count; ++i)
    {
        p.xz *= rot(angle/a);
        p = abs(p)-range*a;
        shape = max(p.x,max(p.y,p.z));
        mat = shape < dist ? i : mat;
        pp = i == 7. ? p : pp;
        ppp = i == 6. ? p : ppp;
        dist = min(dist, shape);
        a /= falloff;
    }
    dist = -dist;
    
    // columns
    shape = max(length(pp.xz)-.001, dist - .005);
    dist = min(dist, shape);

    // lights
    shape = max(length(ppp.xy)-.001, dist - .001);
    glow += .0001/max(.0005,shape);
    
    // crop
    dist = max(dist, abs(pos.y-.1)-.2);
    
    return dist;
}

// from Inigo Quilez
float shadow (vec3 pos, vec3 at, float k) {
    vec3 dir = normalize(at - pos);
    float maxt = length(at - pos);
    float f = 1.;
    float t = .002;
    for (float i = 0.; i <= 1.; i += 1./30.) {
        float dist = map(pos + dir * t);
        if (dist < .001) return 0.;
        f = min(f, k * dist / t);
        t += dist;
        if (t >= maxt) break;
    }
    return f;
}

void main( )
{
    gl_FragColor = vec4(0,0,0,0);
    
    const float frames = 3.;
    for (float frame = 0.; frame < frames; ++frame)
    {
        // coordinates
        vec2 uv = xlv_TEXCOORD0/R.xy;
        vec2 p = 2.*(xlv_TEXCOORD0-R.xy/2.)/R.y;
        
        // anti aliasing
        float aa = 6.28*frame/frames;
        p += vec2(cos(aa),sin(aa))/R.xy;
        
        vec3 pos = vec3(.04,-.02,.09);
        vec3 ray = look(pos, vec3(0,0,0), p, 4.);
        vec3 rng = hash(uvec3(xlv_TEXCOORD0, float(iFrame) + frame*196.));

        // init globals
        glow = 0.;
        mat = 0.;

        // raymarch
        float total = 0.;
        float steps = 0.;
        const float count = 100.;
        const float max_dist = 4.;
        for (steps = count; steps > 0.; --steps) {
            float dist = map(pos);
            if (dist < .001 * total || total > max_dist) break;
            dist *= .9 + .1 * rng.z;
            total += dist;
            pos += ray * dist;
        }
        
        vec4 result;

        if (total < max_dist)
        {
            // lighting
            float shade = steps/count;
            vec3 sun = normalize(vec3(0,1,.2));
            vec3 palette = .5+.5*cos(vec3(1,2,3)*5. + pos.z * 6. + pos.y * 6.);
            vec3 color = vec3(1.5);
            color *= sin(mat+1.)*.25+.75;
            color *= step(.01, ppp.z)*.25+.75;
            color *= step(pp.y, -.005)*.25+.75;
            color += palette*clamp(.1/abs(mat-10.), 0., 1.)*.9;
            color += palette*glow;
            color *= shade;
            color *= .4+.6*shadow(pos, sun, 100.);
            result = vec4(color, 1);
        }
        else
        {
            // sky
            vec3 color = .5+.5*cos(vec3(1,2,3)*5.+uv.y*2.+2.);
            result = vec4(color, 1);
        }
        
        // sum it up
        gl_FragColor += result/frames;
    }        
}
