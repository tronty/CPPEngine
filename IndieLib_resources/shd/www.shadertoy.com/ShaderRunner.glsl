#define antialiasing(n) n/min(iResolution.y,iResolution.x)
#define S(d,b) smoothstep(antialiasing(1.0),b,d)
#define Rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define SkewX(a) mat2(1,tan(a),0,1)
#define SkewY(a) mat2(1,0,tan(a),1)
#define Skew(a) mat2(1,tan(a.x),tan(a.y),1)
#define B(p,s) max(abs(p).x-s.x,abs(p).y-s.y)

float Hash21(vec2 p) {
    p = fract(p*vec2(234.56,789.34));
    p+=dot(p,p+34.56);
    return fract(p.x+p.y);
}

// thx iq! https://iquilezles.org/articles/distfunctions2d/
float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}

// thx iq! https://iquilezles.org/articles/distfunctions2d/
float sdUnevenCapsule( vec2 p, float r1, float r2, float h )
{
    p.x = abs(p.x);
    float b = (r1-r2)/h;
    float a = sqrt(1.0-b*b);
    float k = dot(p,vec2(-b,a));
    if( k < 0.0 ) return length(p) - r1;
    if( k > a*h ) return length(p-vec2(0.0,h)) - r2;
    return dot(p, vec2(a,b) ) - r1;
}

// noise and fbm function from https://www.shadertoy.com/view/Xd3GD4
//-----------------------------------------------------------------------------
vec2 hash( vec2 p )
{
    p = vec2( dot(p,vec2(127.1,311.7)),
             dot(p,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise2d( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    
    vec2 i = floor( p + (p.x+p.y)*K1 );
    
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0*K2;
    
    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
    
    vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    
    return dot( n, vec3(70.0) );
}

float fbm(vec2 uv)
{
    float f;
    mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
    f  = 0.5000*noise2d( uv ); uv = m*uv;
    f += 0.2500*noise2d( uv ); uv = m*uv;
    f += 0.1250*noise2d( uv ); uv = m*uv;
    f += 0.0625*noise2d( uv ); uv = m*uv;
    f = 0.5 + 0.5*f;
    return f;
}

vec3 cuctusTex(vec2 p, vec3 col, float m){
    p*=80.0;
    p.x = fract(p.x)-0.5;
    float d = abs(p.x)-0.2;
    d = max(m,d);
    return mix(col,vec3(0.0,0.6,0.0),S(d,-0.05));
}

vec3 cactus(vec2 p, vec3 col){
    p*=2.0;
    
    vec2 id = floor(p)-0.5;
    float n = Hash21(id);
    float h = clamp(n, 0.3, 0.4);
    
    p.x = mod(p.x,1.0)-0.5;
    p.y+=0.58;
    
    vec2 prevP = p;
    
    p.x*=0.7;
    p.y+=0.015;
    float shadow = length(p)-0.08;
    col = mix(col,vec3(0.0),S(shadow,-0.15));
    
    p = prevP;
    
    p*=Rot(radians(10.0));
    float size = 0.05;
    float d = sdSegment(p,vec2(0.0,h),vec2(0.0,0.0))-size;
    if(n<0.5){
        float d2 = sdSegment(p,vec2(0.0,0.07),vec2(0.1,0.07))-size;
        d = min(d,d2);
        d2 = sdSegment(p,vec2(0.0,0.1),vec2(-0.1,0.1))-size;
        d = min(d,d2);
        d2 = sdSegment(p,vec2(0.1,0.2),vec2(0.1,0.07))-size;
        d = min(d,d2);
        d2 = sdSegment(p,vec2(-0.1,0.25),vec2(-0.1,0.1))-size;
        d = min(d,d2);
    } else {
        float d2 = sdSegment(p,vec2(0.0,0.1),vec2(0.1,0.1))-size;
        d = min(d,d2);
        d2 = sdSegment(p,vec2(0.0,0.07),vec2(-0.1,0.07))-size;
        d = min(d,d2);
        d2 = sdSegment(p,vec2(0.1,0.2+(h*0.2)),vec2(0.1,0.1))-size;
        d = min(d,d2);
        d2 = sdSegment(p,vec2(-0.1,0.25),vec2(-0.1,0.07))-size;
        d = min(d,d2);
    }
    
    d = max(-p.y-0.02,d);
    col = mix(col,vec3(0,0.3,0),S(d,-0.05));
    col = cuctusTex(p,col, d);
    return col;
}

vec3 rock(vec2 p, vec3 col){
    p*=4.0;
    p.y+=1.0;
    
    vec2 id = floor(p)-0.5;
    float n = Hash21(id);
    float h = clamp(n, 0.5 ,1.5);
    float h2 = clamp(n, 1.3 ,2.0);
    
    p.y*= 2.5;
    p.y+= sin(h2)*0.3;
    p.x+=0.2;
    p.x = mod(p.x,1.4)-0.7;
    p.x*=h2;
    float d = length(p)-0.2;
    d = max((abs(p.y)-0.15),d);
    
    float a = radians(-30.0*h);
    p.x+=0.16;
    float slice = dot(p,vec2(cos(a),sin(a)));
    d = max(-slice,d);
    a = radians(50.0*h);
    p.x-=0.35;
    slice = dot(p,vec2(cos(a),sin(a)));
    d = max(slice,d);
    
    p.x+=0.2;
    p.y+=0.1;
    float shadow = length(p)-0.3;
    col = mix(col,vec3(0.0),S(shadow,-0.4));
    
    float nc = noise2d(p*20.)*0.2;
    col = mix(col,vec3(0.6,0.5,0.2)+nc,S(d,-0.07));
    
    return col;
}

float sdTri(vec2 p, vec2 s){
    p*=vec2(0.7,1.2);
    p.x-=s.x*0.5;
    float d = B(p,s);
    float a = radians(45.);
    p.y = abs(p.y);
    float mask = dot(p,vec2(cos(a),sin(a)));
    d = max(mask,d);
    return d;
}

vec3 PolyRunner(vec2 p, vec3 col){
    p*=2.0;
    vec2 prevP = p;
    
    // feather back
    p*=Rot(radians(10.*sin(iTime*30.0)));
    p*=SkewX(0.95);
    p.y*=1.1;
    
    float d = sdTri(p-vec2(-0.01,0.0),vec2(0.12));
    col = mix(col,vec3(0.3,0.6,0.8),S(d,0.0));
    
    // head hair
    p = prevP;
    p*=Rot(radians(31.));
    p*=vec2(1.0,2.5);
    p-=vec2(0.,0.68);
    d = sdTri(p,vec2(0.05));
    col = mix(col,vec3(0.1,0.7,0.9),S(d,0.0));
    
    // neck
    p = prevP;
    p.x*=2.0;
    p.x-=0.2;
    p.y-=0.02;
    p*=SkewY(-0.9);
    p*=Rot(radians(90.));
    d = sdTri(p,vec2(0.12));
    col = mix(col,vec3(0.3,0.7,0.8),S(d,0.0));
    
    // mouth
    p = prevP;
    p*=Rot(radians(11.));
    p*=vec2(1.0,1.7);
    p-=vec2(0.142,0.39);
    
    d = sdTri(p,vec2(0.05));
    col = mix(col,vec3(0.9,0.9,0.1),S(d,0.0));
    
    // body
    p = prevP;
    p*=vec2(0.5,1.1);
    p-=vec2(0.02,-0.09);
    p*=Rot(radians(90.));
    d = sdTri(p,vec2(0.07));
    col = mix(col,vec3(0.5,0.7,0.8),S(d,0.0));
    
    // feather front
    p = prevP;
    p*=Rot(radians(12.*sin(iTime*35.0)));
    p*=SkewX(0.95);
    d = sdTri(p-vec2(-0.01,-0.03),vec2(0.08));
    col = mix(col,vec3(0.1,0.4,0.7),S(d,0.0));    
    
    // animated legs
    p = prevP;
    p.y*=1.3;
    p.x-=0.03;
    p.y+=0.23;
    p*=Rot(radians(800.0*iTime));
    d = abs(length(p)-0.12)-0.03;
    d = max(-(abs(p.y)-0.05),d);
    
    col = mix(col,vec3(0.7,0.5,0.5),S(d,-0.05));
    
    p = prevP;
    p.y*=1.3;
    p.x-=0.03;
    p.y+=0.23;
    p*=Rot(radians(750.0*iTime+50.));
    d = abs(length(p)-0.08)-0.03;
    d = max(-(abs(p.y)-0.03),d);
    
    col = mix(col,vec3(0.5,0.5,0.1),S(d,-0.05));
    
    return col;
}

float dCharI(vec2 p){
    float d = B(p,vec2(0.01,0.04));
    return d;
}

float dCharN(vec2 p){
    vec2 prevP = p;
    p.x = abs(p.x)-0.03;
    float d = B(p,vec2(0.01,0.04));
    p = prevP;
    p*=Rot(radians(-30.));
    float d2 = B(p,vec2(0.01,0.06));
    p = prevP;
    d2 = max((abs(p.y)-0.04),d2);
    
    d = min(d,d2);
    return d;
}

float dCharO(vec2 p){
    float d = abs(length(p)-0.03)-0.01;
    return d;
}

float dCharU(vec2 p){
    vec2 prevP = p;
    float d = abs(length(p)-0.03)-0.01;
    d = max(p.y,d);
    p.x = abs(p.x)-0.03;
    p.y-=0.02;
    float d2 = B(p,vec2(0.01,0.02));
    d = min(d,d2);
    return d;
}

float dCharT(vec2 p){
    vec2 prevP = p;
    float d = B(p,vec2(0.01,0.04));
    p.y-=0.03;
    float d2 = B(p,vec2(0.03,0.01));
    return min(d,d2);
}

float dInNOut(vec2 p){
    float d = dCharI(p-vec2(-0.3,0.0));
    float d2 = dCharN(p-vec2(-0.24,0.0));
    d = min(d,d2);
    d2 = B(p-vec2(-0.18,0.0),vec2(0.01));
    d = min(d,d2);
    d2 = dCharN(p-vec2(-0.12,0.0));
    d = min(d,d2);
    d2 = B(p-vec2(-0.06,0.0),vec2(0.01));
    d = min(d,d2);
    d2 = dCharO(p-vec2(0.,0.0));
    d = min(d,d2);
    d2 = dCharU(p-vec2(0.09,0.0));
    d = min(d,d2);
    d2 = dCharT(p-vec2(0.17,0.0));
    d = min(d,d2);
    return d;
}

vec3 inNOut(vec2 p, vec3 col) {
    vec2 prevP = p;
    float d = dInNOut(p);
    col = mix(col,vec3(1.0,0.0,0.0),S(d,0.0));
    
    p-=vec2(-0.08,-0.09);
    p*=Rot(radians(-28.));
    d = sdUnevenCapsule(p,0.02,0.035,0.25);
    p = prevP;
    p-=vec2(0.1,0.09);
    p*=Rot(radians(-80.));
    float d2 = sdUnevenCapsule(p,0.01,0.035,0.3);
    d = min(d,d2);
    p = prevP;
    
    p*=Rot(radians(11.));
    d2 = sdTri(p-vec2(0.09,0.107),vec2(0.04));
    d = min(d,d2);
    
    p = prevP;
    d = max(-(abs(p.y)-0.055),d);
    
    col = mix(col,vec3(1.,0.9,0.0),S(d,0.0));
    return col;
}

vec3 speechBaloon(vec2 p, vec3 col){
    p*=2.0;
    vec2 prevP = p;
    p.x*= 0.6;
    float d = length(p)-0.2;
    float d2 = length(p-vec2(-0.1,-0.25))-0.03;
    d = min(d,d2);
    p = prevP;
    d2 = length(p-vec2(-0.23,-0.33))-0.03;
    d = min(d,d2);
    col = mix(col,vec3(1.0),S(d,0.0));
    p = prevP;
    col = inNOut(p-vec2(0.07,-0.04),col);
    
    return col;
}

float remap(float value, float low1, float high1, float low2, float high2) {
    return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}

void main( )
{
    vec2 uv = (xlv_TEXCOORD0-0.5*iResolution.xy)/iResolution.y;
    vec2 m =  iMouse.xy/iResolution.xy;
    
    float zoom = 1.0;
    
    if(iMouse.z>0.){
        uv*=zoom+0.3-remap(m.y,-0.7,0.7,0.0,0.3);
    } else {
        uv*=zoom;
    }
    
    vec2 prevUV = uv;
    
    float slowestSpeed = 0.5;
    float mediumSpeed = 1.3;
    float fastestSpeed = 3.0;
    
    uv.x+=iTime*slowestSpeed;
    uv*=15.0;
    
    vec3 col = mix(vec3(0.6,0.9,1.0),vec3(0.5,0.9,1.0),uv.y);
    
    // distant mountains
    float n = fbm(uv*0.05)*5.0;
    float nc = noise2d(uv*vec2(0.5,10.))*0.05;
    float d = uv.y+n-3.;
    col = mix(col,vec3(0.5,0.8,1.0)+nc,S(d,0.0));
    
    // sun
    uv = prevUV;
    uv.x+=sin(iTime*0.5)*0.05;
    d = length(uv-vec2(0.5,0.3))-0.1;
    col = mix(col,vec3(1.0),S(d,-0.07));
    
    // clouds
    uv = prevUV;
    uv.y-=0.62;
    uv*=5.0;
    n = fbm(uv - vec2(-iTime*slowestSpeed,iTime*(slowestSpeed*0.5)))*0.6;
    uv*= n;
    float clouds = B(uv,vec2(2.0,0.4));
    col = mix(col,vec3(0.5,0.9,1.0)+n*0.2,S(clouds,-0.2));
    
    uv = prevUV;
    uv.x+=iTime*mediumSpeed;
    uv*=15.0;
    
    // closest mountains
    uv-=vec2(0.3);
    n = fbm(uv*0.1)*3.0+abs(sin(uv.x*0.1)*2.0);
    d = uv.y+n-1.;
    col = mix(col,vec3(0.6,0.5,0.1)-(n*0.05),S(d,0.0));
    
    uv+=vec2(0.3);
    n = fbm(uv*0.1)*3.0+abs(sin(uv.x*0.1)*2.0);
    d = uv.y+n-1.;
    nc = noise2d(uv*vec2(0.5,10.))*0.1;
    col = mix(col,vec3(1.5,0.8,0.5)-(n*0.05)+nc,S(d,0.0));
    
    uv = prevUV;
    d = 0.0;
    float scene = mod(iTime,30.0);
    if(scene >=10. && scene<20.){
        col = mix(col,vec3(0.9,0.4,0.1)*col*1.2,S(d,0.0));
    } else if(scene >= 20.){
        col = mix(col,vec3(0.1,0.4,0.9)*col*1.5,S(d,0.0));
    }
    
    uv = prevUV;
    d = uv.y+0.3;
    col = mix(col,vec3(0.7),S(d,0.0));
    
    uv.y += 0.28;
    d = (abs(uv.y)-0.07);
    uv.x+=iTime*fastestSpeed;
    n = noise2d(uv*100.)*0.05;
    col = mix(col,vec3(0.9,0.9,0.6)+n,S(d,-0.05));
    
    uv = prevUV;
    uv.x+=iTime*fastestSpeed;
    col = rock(uv,col);
    
    uv = prevUV;
    uv.x+=iTime*fastestSpeed;
    col = cactus(uv,col);
    
    uv = prevUV;
    uv.x+=sin(iTime)*0.1;
    uv.y+=0.35;
    uv*=5.0;
    uv.x*=1.5;
    uv*=Rot(radians(12.0));
    uv.y*=mix(1.5,6.0,smoothstep(-1.0,1.0,uv.x));
    float smokeD = B(uv,vec2(0.6,0.01))*fbm(uv - vec2(-iTime*3.0,iTime*0.01));
    col = mix( col, (vec3(0.95)), 1.0-smoothstep(0.05,0.5,smokeD) );
    
    uv = prevUV;
    uv.x+=sin(iTime)*0.1;
    col = PolyRunner(uv-vec2(0.22,-0.3),col);
    
    uv = prevUV;
    uv.x+=iTime*fastestSpeed;
    uv.y+=0.5;
    uv*=2.0;
    uv.x = fract(uv.x)-0.5;
    d = B(uv,vec2(0.2,0.02));
    col = mix(col,vec3(1.),S(d,0.));
    
    uv = prevUV;
    uv.x+=sin(iTime)*0.1;
    col = speechBaloon(uv-vec2(0.47,0.),col);
    
    uv = prevUV;
    uv.x+=mod(iTime,5.)*fastestSpeed;
    col+=noise2d(uv*100.0)*0.05;
    
    // Output to screen
    gl_FragColor = vec4(col,1.0);
}
