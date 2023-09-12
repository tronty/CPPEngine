// Highlands Castle
// by athibaul, 2023-09-11

#define W (1.5*max(fwidth(pW.x), fwidth(pW.y)))
//#define W 0.
#define BOX(p,r,fg) {vec2 pW = p; float d = sdBox(p, r); c = mix(c, fg, smoothstep(0., -W, d)); }
#define TOW(p,r,fg,slope) q = (p) / (r); {vec2 pW = p; float d = sdTower(p, r, slope); float fg2 = mix(fg,bgc,0.1*abs(q.x)*smoothstep(-0.5,0.5,q.y)); c = mix(c, fg2, smoothstep(0., -W, d)); }
#define TRI(p,q,fg) {float d = sdTriangleIsosceles(p, q); c = mix(c, fg, smoothstep(0., -W, d)); }
#define WINDOWS(s,r,w) {vec2 q0 = q; q.x = abs(q.x) < 1. ? asin(q.x) : 1000.; q = abs(q * (s)) - (r)*(s); q -= min(vec2(0),round(q)); BOX(q, w, fg);}


// cosine based palette adapted from Inigo Quilez:
// https://iquilezles.org/articles/palettes/
vec3 pal(in float t)
{
    vec3 a = vec3(.5), b = vec3(.5), c = vec3(1), d = vec3(0,.1,.2);
    return a + b*cos( 6.28318*(c*t+d) );
}

// 2D sdfs from iquilez - https://iquilezles.org/articles/distfunctions2d/
float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float sdTriangleIsosceles( in vec2 p, in vec2 q )
{
    p.x = abs(p.x);
    vec2 a = p - q*clamp( dot(p,q)/dot(q,q), 0.0, 1.0 );
    vec2 b = p - q*vec2( clamp( p.x/q.x, 0.0, 1.0 ), 1.0 );
    float s = -sign( q.y );
    vec2 d = min( vec2( dot(a,a), s*(p.x*q.y-p.y*q.x) ),
                  vec2( dot(b,b), s*(p.y-q.y)  ));
    return -sqrt(d.x)*sign(d.y);
}


//---

float sinn(in float x)
{
    return sin(x) + 0.62*cos(1.62*x);
}

float sdTower(in vec2 p, in vec2 r, in float slope) {
    float d = sdBox(p, r);
    // Also add a small ring before the end of the tower
    p.y -= r.y - r.x;
    d = min(d, sdBox(p, vec2(r.x * (1.1 + 0.1 * p.y / (r.x*0.3)), r.x*0.3)));
    // And the roof
    float h = slope * r.x;
    p.y -= r.x*0.9 + h;
    d = min(d, sdTriangleIsosceles(p, vec2(r.x*1.2, -h)));
    return d;
}

void main( )
{
    vec2 uv = (xlv_TEXCOORD0 * 2. - iResolution.xy) / iResolution.y;
    //float W = fwidth(uv.y);
    
    // I want to use palette values ranging from 0.0 to 0.5
    // 0.5 night sky
    // 0.4 pitch black
    // 0.2-0.1 windows
    // 0.0 bright highlights
    
    float blk = 0.4; // pitch black
    float bgc = 0.5 + 0.05*uv.y; // background gradient for the sky
    float c = bgc;
    // Hills in the background
    float voff = 0.8, phi = 1.2, vscale = 0.3;
    for(float i=20.; i>2.; i/=1.8) {
        float d = uv.y - (0.4*sinn(uv.x*i + 10.2*i +1.) - 0.6)/i - 0.3;
        vec2 pW = uv;
        c = mix(c, mix(bgc, blk, exp(-0.5*i)), smoothstep(0., -W, d));
    }
    // Front hill
    {
        float d = uv.y - 0.2*sinn(uv.x*1.5 + 1.) + 0.5;
        vec2 pW = uv;
        c = mix(c, blk, smoothstep(0., -W, d));
    }
    // Castle towers and windows
    vec2 p = 2.2*(uv - vec2(-0.35,-0.25));
    {
        vec2 q = vec2(0);
        float fg = 0.;
        //W *= 2.;
        BOX(p, vec2(0.75,0.5), blk); // main block
        BOX(p - vec2(-1.,-0.5) - 0.1*p.y, vec2(0.8,0.5), blk); // left block
        BOX(p - vec2(0.5,-0.2) + 0.2*p.y, vec2(0.8,0.5), blk); // right block
        BOX(p - vec2(-0.2,0.5) - 0.1*p.y, vec2(0.3,0.2), blk); // chapel beneath left tower
        BOX(p - vec2(0.2,0.4), vec2(0.2, 0.5), blk); // left reinforcement of high tower
        BOX(p - vec2(0.5, 0.6) - vec2(0.,1.)*p.x, vec2(0.1, 0.1), blk); // right flank of high tower
        TOW(p - vec2(-0.12, 0.6), vec2(0.13, 0.6), blk, 2.5); // left tower beside high tower
        fg = 0.1 + abs(q.x)*0.2; q.y -= 0.7; WINDOWS(vec2(1.5,3.), vec2(1.,0.2), vec2(0.2))
        TOW(p - vec2(0.2, 0.9), vec2(0.25,0.8), blk, 3.5); // high tower
        fg = abs(q.x)*0.3 - 0.2*q.y; WINDOWS(3., vec2(1.,0.83), vec2(0.2))
        TOW(p - vec2(-0.35, 0.7), vec2(0.05, 0.2), blk, 6.); // chapel tower
        fg = bgc; q.y -= 0.9; WINDOWS(vec2(1.5,5.), vec2(1.,0.), vec2(0.4))
        fg = 0.2; TOW(p - vec2(0.5, 1.3), vec2(0.1, 0.15), blk, 4.); // right tower beside high tower
        q -= vec2(0.5,-0.2); WINDOWS(vec2(1.,1.), vec2(0.2,0.2), vec2(0.1))
        TOW(p - vec2(-1., 0.3), vec2(0.16, 0.45), blk, 2.1); // middle left tower flank
        fg = 0.3; q.y -= 0.85; WINDOWS(1.5, vec2(1.,0.), vec2(0.1,0.15))
        TOW(p - vec2(-0.7, 0.4), vec2(0.2, 0.5), blk, 2.); // middle left tower
        fg = 0.1; q.y -= 0.8; WINDOWS(2., vec2(1.,0.), vec2(0.1,0.2))
        TOW(p - vec2(0.65, 0.2), vec2(0.1, 0.55), blk, 5.); // middle right tower
        fg = 0.1; q.y -= 0.9; WINDOWS(2., vec2(1.,0.), vec2(0.1,0.1))
        TOW(p - vec2(-1.5, 0.), vec2(0.3, 0.5), blk, 1.5); // left tower
        fg = 0.3; q.y -= 0.8; WINDOWS(2., vec2(1.,0.), vec2(0.2))
        TOW(p - vec2(1.05, 0.3), vec2(0.25, 0.4), blk, 1.2); // right tower
        fg = 0.3; q.y -= 0.8; WINDOWS(2., vec2(1.,0.), vec2(0.2))
        //float d = sdBox(p, vec2(0.5,0.5));
        //c = mix(c, blk, smoothstep(0., -W, d));
    }
    
    // Chapel windows
    {
        vec2 q = p - vec2(-0.2,0.6);
        float fg = 0.15 - q.x + q.y;
        float qy = max(q.y,0.);
        q = abs(q/0.05) - (vec2(7,1)*0.5-0.5);
        q -= min(vec2(0), round(q));
        float width = 0.2 - 100.*qy*qy;
        BOX(q, vec2(width, 2.0), fg);
    }
    
    
    // Hall windows
    {
        vec2 q = p - vec2(-0.2,0.6);
        float fg = 0.15 - q.x + q.y;
        float qy = max(q.y,0.);
        q = abs(q/0.05) - (vec2(7,1)*0.5-0.5);
        q -= min(vec2(0), round(q));
        float width = 0.2 - 100.*qy*qy;
        BOX(q, vec2(width, 2.0), fg);
    }
    
    // Flying flag
    {
        vec2 q = p - vec2(0.2, 0.9) - vec2(0.,0.8 + 3.5*.25);
        float h = 0.2;
        q.x = h-q.x;
        q.y += 0.2*h*sin(16.*q.x+3.*iTime) * smoothstep(h,0.,q.x);
        vec2 pW = q;
        TRI(q.yx, vec2(0.03,h), blk)
    }
    vec3 col = pal(c);
    // Output to screen
    gl_FragColor = vec4(col,1.0);
}

