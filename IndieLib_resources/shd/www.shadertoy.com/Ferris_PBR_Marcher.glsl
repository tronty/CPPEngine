#define SURF_DIST .001

// Sky limit
#define MAX_DIST 5000.
#define SKY_DIST -1.

// Iteration limit
#define MAX_ITER 5000.
#define ITER_DIST -1.

// Material properties
#define SMOOTHNESS .5
#define METALLIC .5


struct ray { // Ray
    vec3 o;  // Ray orign
    vec3 d;  // Ray direction (plz normalize this)
};

struct rmr { // Ray March Result
    float d; // Obj Distance
    float i; // Steps taken
    float a; // ~Ang Ray-SDF
};

struct ptlight { // Point light
    vec3 p;      // Position
    float i;     // Intensity
    float s;     // Shadow softness
};

float sdf( vec3 p ) {
    return min(p.y, distance(p,vec3(0,1,0))-1.);
}

rmr march( ray r, float md ) {
    rmr m = rmr(0., 0., 6.);
    
    for(; m.i<MAX_ITER; m.i++) {
        
        vec3 p = r.o + r.d * m.d;
        float d = sdf(p);
        
        m.a = min( m.a, atan(d,m.d));
        
        m.d += d;
        
        if( d < SURF_DIST ) {
            return m;
        }
        
        // Sky limit
        if( m.d > md ) {
            #ifdef SKY_DIST
            m.d = SKY_DIST;
            #endif
            return m;
        }
    }
    
    // Too many iterations
    #ifdef ITER_DIST
    m.d = ITER_DIST;
    #endif
    
    return m;
}

vec3 norm( vec3 p ) {
    const vec2 d = vec2(0, SURF_DIST);
    
    vec3 n = vec3(
        sdf(p + d.yxx) - sdf(p - d.yxx),
        sdf(p + d.xyx) - sdf(p - d.xyx),
        sdf(p + d.xxy) - sdf(p - d.xxy)
    );
    
    return normalize(n);
}

vec3 rendptlight( ptlight l, vec3 p, ray v ) {
    const float esd = SURF_DIST * (1. + SURF_DIST);

    vec3 ld = normalize(l.p - p);
    float d = distance(l.p, p);
    
    vec3 n = norm(p);
    
    float light = 0.0795774 * l.i / (d*d);
    
    ray lr = ray( p + n*esd, ld);
    rmr lm = march(lr, d);
    
    light *= lm.d == SKY_DIST ? 1. : 0.;
    light *= smoothstep(.0, l.s*.1, lm.a); 
    
    //////////////
    //  albedo  //
    //////////////
    
    float albedo = clamp(dot( n, ld ), 0., 1.);
    albedo *= light * (1.-METALLIC);
    
    ///////////////////
    //  blinn-phong  //
    ///////////////////
    
    vec3 hv = normalize(ld - v.d);
    float spec = clamp(dot(hv, n), 0., 1.);
    spec = pow(spec, SMOOTHNESS * 100.);
    spec *= light * METALLIC;
    
    return vec3(albedo + spec);
}

void main( ) {
    vec2 uv = (xlv_TEXCOORD0-.5*iResolution.xy)/iResolution.y;
    vec3 col = vec3(0);

    vec3 lp = vec3(cos(iTime*10.0),0,sin(iTime*10.0))*2.;
    lp.y = 1.5;
    
    ptlight l = ptlight(lp, 20., 1.);
    
    vec3 ro = vec3(0, 1, -5);
    vec3 rd = normalize(vec3(uv,1));
    ray r = ray(ro,rd);
    rmr m = march(r, MAX_DIST);
    
    vec3 p = ro + rd * m.d;
    
    
    col += rendptlight(l, p, r);
    col *= m.d>0. ? 1. : 0.;

    gl_FragColor = vec4(col,1.0);
}

