#if 0
#extension GL_OES_standard_derivatives : enable

precision highp float;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#endif

#define PI 3.14159265
#define saturate(x) clamp(x,0.,1.)


float line( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}

float hash1( vec2 p )
{
    p  = 50.0*fract( p*0.3183099 );
    return fract( p.x*p.y*(p.x+p.y) );
}
float noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 w = fract(x);
    #if 1
    vec2 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    #else
    vec2 u = w*w*(3.0-2.0*w);
    #endif

    float a = hash1(p+vec2(0,0));
    float b = hash1(p+vec2(1,0));
    float c = hash1(p+vec2(0,1));
    float d = hash1(p+vec2(1,1));
    
    return -1.0+2.0*(a + (b-a)*u.x + (c-a)*u.y + (a - b - c + d)*u.x*u.y);
}

const mat2 m2 = mat2(  0.80,  0.60,
                      -0.60,  0.80 );
float fbm(vec2 p) {
    float f = 1.9;
    float s = 0.55;
    float a = 0.0;
    float b = 0.5;
    for( int i=0; i<4; i++ )
    {
        float n = noise(p);
        a += b*n;
        b *= s;
        p = f*m2*p;
    }
	return a;
}

float hash1( float n )
{
    return fract( n*17.0*fract( n*0.3183099 ) );
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 w = fract(x);
    
    #if 1
    vec3 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    #else
    vec3 u = w*w*(3.0-2.0*w);
    #endif
    


    float n = p.x + 317.0*p.y + 157.0*p.z;
    
    float a = hash1(n+0.0);
    float b = hash1(n+1.0);
    float c = hash1(n+317.0);
    float d = hash1(n+318.0);
    float e = hash1(n+157.0);
	float f = hash1(n+158.0);
    float g = hash1(n+474.0);
    float h = hash1(n+475.0);

    float k0 =   a;
    float k1 =   b - a;
    float k2 =   c - a;
    float k3 =   e - a;
    float k4 =   a - b - c + d;
    float k5 =   a - c - e + g;
    float k6 =   a - b - e + f;
    float k7 = - a + b + c - d + e - f - g + h;

    return -1.0+2.0*(k0 + k1*u.x + k2*u.y + k3*u.z + k4*u.x*u.y + k5*u.y*u.z + k6*u.z*u.x + k7*u.x*u.y*u.z);
}

const mat3 m3  = mat3( 0.00,  0.80,  0.60,
                      -0.80,  0.36, -0.48,
                      -0.60, -0.48,  0.64 );
                      
float fbm(vec3 p) {
    float f = 1.9;
    float s = 0.55;
    float a = 0.0;
    float b = 0.5;
    for( int i=0; i<4; i++ )
    {
        float n = noise(p);
        a += b*n;
        b *= s;
        p = f*m3*p;
    }
	return a;
}
#define sunpos vec2(.5,.25)

float clouds(vec2 uv) {
    uv.x += time*.001;
    float d = fbm( vec3(uv*vec2(4.,10.)*1.1+50., time*.01) );
    d += abs(cos(uv.x*70.)*.5 + cos(uv.x*160.)*.25)*0.07;
    d += uv.y-1.;
    return d+.2;
}

vec4 background(vec2 uv, vec2 p) {
    vec4 col = vec4(0., 0., 0., 1.);
    
    // greenish sky
    col.rgb = pow( vec3(.5), vec3(5., 2.5*uv.y, 2.*(uv.y*-.1+1.)) );
    
    // clouds
    float c = smoothstep(0.3,.32,clouds(uv));
    float cs = smoothstep(0.3,.32, clouds(uv - normalize(p-sunpos)*.015));
    col.rgb += vec3(.5,.4,.3) * c * (1.-cs);
    col.rgb += vec3(.5,.4,.3)*.75 * c * (cs);
    
    // stars
    col.rgb += vec3(1.) * smoothstep(0.8,1., noise(vec3(uv*300., time*.1))) * (1.-c);
    
    
    // sun
    float d = distance(p, sunpos);
    col.rgb += vec3(1.) * smoothstep(0.16,.15,d);
    col.rgb += vec3(1.,1.,.5) * pow(1./(1.+d), 8.)*1.;
    
    return col;
}

vec4 pyramids(vec2 p, float freq, float proba) {
    vec4 col = vec4(0., 0., 0., 0.);

    float seed = floor(p.x*freq-.5);
    float h = fract(p.x*freq);
    float d = -p.y + abs(h-.5)/freq * step(proba,hash1(seed));
    float m = smoothstep(0.,0.01,d) ;
    
    float ds = -p.y + saturate(h-.5)/freq * step(proba,hash1(seed));
    float ms = smoothstep(0.,0.01,ds) ;
    
    col.rgb = vec3(1.,.5,.0) * smoothstep(0.6,.5, fbm(p*vec2(10.,100.)*freq));
    col.rgb = mix(col.rgb, vec3(1.,.5,.0)*.3, ms);
    
    col.rgb *= smoothstep(0.,0.015, abs(d-0.005)); // outline
    
    col.rgb = saturate(col.rgb);
    
    col.a = max(m, ms);
    
    return col * col.a;
}

float moutainsHeight(vec2 p, float amp, float power) {
    float d = - pow(abs(sin(p.x*5.)*.5+ sin(p.x*2.+2.5)*.25 + sin(p.x*4.+2.)*.125), power) * amp;
    return d;
}

vec4 mountains(vec2 p, float amp, float power) {
    vec4 col = vec4(0., 0., 0., 0.);
    
    float h = -p.y + moutainsHeight(p,amp,power);
    float hs = -p.y +moutainsHeight(p + normalize(p-(sunpos*2.-1.))*.05,amp,power);
    float d = smoothstep(0.,0.01,h);
    float ds = smoothstep(0.,0.01,hs);
    
    col = vec4(1.,1., 1., 1.) * d;
    col.rgb *= vec3(1.,.4,.2)*(smoothstep(0.,-1.,ds-d)*.75+.25);
    col.rgb *= smoothstep(0.,0.02,abs(h-.01)); // outline
    //col.rgb *= (sin(d*50.+fbm(p*vec2(5.,50.)))*.5+.5)*.5+.5;
    col.rgb = saturate(col.rgb);
    return col * col.a;
}

vec4 cactus(vec2 p, float freq) {

    vec4 col = vec4(0., 0., 0., 0.);
    
    
    vec2 ip = floor(p*freq);
    vec2 fp = fract(p*freq)-.5;
    float seed = hash1(ip.x);
    fp.y = p.y*2. + (seed)*.4;
    
    if (hash1(ip.x+1000.) > .3) {
        return vec4(0.);
    }
    
    float d = line(fp, vec2(0.,-.3), vec2(0.,.3));
        
    if (hash1(ip.x+100.) > .5) {
        fp.x = -fp.x;
    }
    
    if (seed > .25) {
        d = min(d, line(fp, vec2(0.,0.), vec2(0.3,.0))*1.8);
        d = min(d, line(fp, vec2(0.3,0.015), vec2(0.3,.2))*1.3);
    }
    
    d = min(d, line(fp, vec2(0.,-.15), vec2(-0.3,-.15))*1.8);
    d = min(d, line(fp, vec2(-0.3,-.14), vec2(-0.3,.05))*1.3);
    d = d-p.y*.3 - fbm(p*300.+5.)*.005;
    
    
    col = vec4(vec3(0.4,1.,0.)*.5 * (smoothstep(0.5,.6, fbm(p*vec2(300.,5.)+5.)*.5+.5)*.25+.75), smoothstep(0.1,0.09,d));
    
    col.rgb *= vec3(1.) * smoothstep(0.007,0.012, abs(d-.098)); // outline
    
    return col * col.a;
}


void main()
{
    vec2 invRes = vec2(1.) / resolution.xy;
    vec2 uv = xlv_TEXCOORD0.xy * invRes;
    vec2 p = (2.*xlv_TEXCOORD0.xy - resolution.xy) / resolution.y;
    vec2 pp = p;
    
    
    // background
    vec4 col = background(uv, pp);
    
    
    // layers
    #define LAYER_SPEED 0.05
    #define LAYER_COUNT 8
    vec4 layer[LAYER_COUNT];
    p.x += time*LAYER_SPEED;    layer[0] = mountains(p*vec2(.5,3.)-vec2(0.,.3), .75, 1.2) * vec4(vec3(.25),1.);
    p.x += time*LAYER_SPEED;    layer[1] = pyramids(p-vec2(0.,-.4), 1., .6);
    p.x += time*LAYER_SPEED;    layer[2] = pyramids(p-vec2(0.,-.39), .5, .6);
    p.x += time*LAYER_SPEED;    layer[3] = mountains(p*vec2(.25,2.25)-vec2(10.,-0.5), 1., 1.2);
    p.x += time*LAYER_SPEED;    layer[4] = cactus(p*1.5-vec2(0.,-.7),3.)*1.;
    p.x += time*LAYER_SPEED;    layer[5] = mountains(p*vec2(.25,2.)-vec2(0.,-0.6), 1., 1.2);
    p.x += time*LAYER_SPEED;    layer[6] = mountains(p*vec2(.15,2.)-vec2(1000.,-0.7), 1., 1.2);
    p.x += time*LAYER_SPEED;    layer[7] = cactus(p*.2-vec2(0.,-.0),3.);
    
    // merge layers with alpha premultiplied
    for(int i=0; i<LAYER_COUNT; i++) {
        col.rgb = col.rgb * (1.-layer[i].a) + layer[i].rgb * (2./(pow(float(i),2.5)+1.));
    }
    
    // flare
    float d = distance(pp, sunpos);
    col.rgb += vec3(1.,1.,.5) * pow(1./(1.+d), 3.)*.1;
    
    
    // color grading
    col.rgb = pow(col.rgb, vec3(1.0,1.5,1.3));
    
    // vignetting
    col.rgb *= pow( uv.x*uv.y*(1.-uv.x)*(1.-uv.y)*100., .1);

    // gamma correction
    col.rgb = pow(col.rgb, vec3(1./2.2));
    
    // output to the screen
    gl_FragColor = vec4(col.rgb * smoothstep(0.,3., time),1.0);
}

