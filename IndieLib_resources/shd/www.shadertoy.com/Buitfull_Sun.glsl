float rand(vec2 co, float seed){
    return fract(sin(dot(co.xy + seed ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 makeJupiter2(vec2 uv)
{
    float time = iTime;
	float timeScale = .5;
	vec2 zoom = vec2(20.,5.5);
	vec2 offset = vec2(2.,1.);

    
    vec2 point = uv * zoom + offset;
    float p_x = float(point.x); 
    float p_y = float(point.y);
    
    float a_x = .2;
    float a_y = .3;
    
    for(int i=1; i<int(10); i++){
        float float_i = float(i); 
        point.x+=a_x*sin(float_i*point.y+time*timeScale);
        point.y+=a_y*cos(float_i*point.x+time*.2);
    }
        
    float r = cos(point.x+point.y+2.)*.5+.5;
    float g = sin(point.x+point.y+2.2)*.5+.5;
    float b = (sin(point.x+point.y+1.)+cos(point.x+point.y+1.5))*.5+.5;
    
    vec3 col = vec3(r,g,b);
    col += vec3(.5);

    return col;
}
#if 0
vec3 makeJupiter(vec2 uv)
{
	float time = iTime;
    vec2 point = uv * zoom + offset;
    float p_x = float(point.x); 
    float p_y = float(point.y);
    
    float a_x = .2;
    float a_y = .3;
    
    for(int i=1; i<int(iteration); i++){
        float float_i = float(i); 
        point.x+=a_x*sin(float_i*point.y+time*timeScale);
        point.y+=a_y*cos(float_i*point.x);
    }
    
    float r = sin(point.y)*.5+.4;
    float g = cos(point.y)*.5+.7;
    float b = cos(point.y)*.5+.8;
    
    r+=.3;
    
    r = cos(point.x+point.y+1.3)*.5+.5;
    g = sin(point.x+point.y+2.)*.5+.5;
    b = (sin(point.x+point.y+1.)+cos(point.x+point.y+1.))*.25+.5;
      
     
    r = pow(r,.8);
    g = pow(g,.8);
    b = pow(b,1.);
    
    vec3 col = vec3(r,g,b);
    col += vec3(.1);
    
    return col;
}
#endif

float timeScale = 1.;

// Created by Alex Kluchikov
vec2 rot(vec2 p,float a)
{
    float c=cos(a*15.83+time*timeScale);
    float s=sin(a*15.83+time*timeScale);
    return p*mat2(s,c,c,-s);
}

vec3 makeSun(vec2 uv_)
{
    vec2 uv=uv_/iResolution.xx;
    uv=vec2(.125,.75)+(uv-vec2(.125,.75))*.015;
    float T=iTime*.5;

    vec3 c = normalize(.75-.25*vec3(
        sin(length(uv-vec2(.1,0))*132.+T*3.3),
        sin(length(uv-vec2(.9,0))*136.-T*2.5),
        sin(length(uv-vec2(.5,1))*129.+T*4.1)
        ));

    vec3 c0=vec3(0);
    float w0=0.;

    float N=80.;
    float wb=.012;
    for(float i=0.;i<N;i++)
    {
        float wt=(i*i/N/N-.2)*.3;
        float wp=0.5+(i+1.)*(i+1.5)*0.001;
    	c.zx=rot(c.zx,1.6+T*0.65*wt+(uv.x+.7)*23.*wp);
    	c.xy=rot(c.xy,c.z*c.x*wb+1.7+T*wt+(uv.y+1.1)*15.*wp);
    	c.yz=rot(c.yz,c.x*c.y*wb+2.4-T*0.79*wt+(uv.x+uv.y*(fract(i/2.)-0.25)*4.)*17.*wp);
    	c.zx=rot(c.zx,c.y*c.z*wb+1.6-T*0.65*wt+(uv.x+.7)*23.*wp);
    	c.xy=rot(c.xy,c.z*c.x*wb+1.7-T*wt+(uv.y+1.1)*15.*wp);
        float w=(1.5-i/N);
        c0+=c*sqrt(w);
        w0+=sqrt(w);
    }
    c0=c0/w0*3.+.5;
    
	return vec3(sqrt(c0.r)*1.2,c0.r*c0.r*.9,c0.r*c0.r*c0.r*.4);
}



float seed = 437585453.123;
float PI = 3.141592653589793;
float res = 5.;

vec3  water         = vec3(        0. ,        0. ,        1.  );

float sandLevel     = 0.5;
vec3  sand          = vec3( 229./255. , 223./255. , 136./255.  );

float grassLevel    = 0.53;
vec3  grass         = vec3(   0./255. , 154./255. ,  23./255.  );

float mountainLevel = 0.69;
vec3  mountain      = vec3(         .7,         .7,         .7 );

float snowLevel     = 0.79;
vec3  snow          = vec3(        1. ,        1. ,        1.  );

float hash21(vec2 p) 
{
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  float f = dot (p, vec2 (127.1, 351.7));
  return fract(sin(f)*a);
}

vec2 hash2(vec2 p) 
{
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  p = vec2(dot (p, vec2 (127.1, 351.7)), dot(p, vec2(245.5, 304.4)));
  return fract(sin(p)*a);
}

vec3 hash23(vec2 p) {
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  vec3 p3 = vec3(dot (p, vec2 (127.1, 351.7)), dot (p, vec2 (261.5, 183.3)), dot(p, vec2(245.5, 304.4)));
  return fract(sin(p3)*a);
}

vec2 hash32(vec3 p) {
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  vec2 p2 = vec2(dot (p, vec3 (127.1, 351.7, 251.45)), dot (p, vec3 (261.5, 183.3, 376.89)));
  return fract(sin(p2)*a);
}
float hash31(vec3 p) {
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  float f = dot (p, vec3 (127.1, 351.7, 251.45));
  return fract(sin(f)*a);
}

float ease(float t)
{
  return t * t * t * (t * (t * 6. - 15.) + 10.);
}
vec2 ease(vec2 t)
{
  return vec2(ease(t.x), ease(t.y));
}
vec3 ease(vec3 t)
{
  return vec3(ease(t.x), ease(t.y), ease(t.z));
}
vec4 ease(vec4 t)
{
  return vec4(ease(t.x), ease(t.y), ease(t.z), ease(t.w));
}

float noise(vec2 point){
    vec2 fr = ease(fract(point));
    vec2 hi = ceil(point);
    vec2 lo = floor(point);
    
    return mix(
        mix(
            hash21(lo), 
            hash21(vec2(hi.x , lo.y)), 
            fr.x
        ), 
        mix(
            hash21(vec2(lo.x , hi.y)),
            hash21(hi),
            fr.x
        ), 
        fr.y
    );
}

float noise(vec3 point){
    vec3 fr = ease(fract(point));
    vec3 hi = ceil(point);
    vec3 lo = floor(point);
    
    return mix(
        mix(
            mix(
                hash31(lo), 
                hash31(vec3(hi.x , lo.yz)), 
                fr.x
            ), 
            mix(
                hash31(vec3(lo.x , hi.y, lo.z)),
                hash31(vec3(hi.xy, lo.z)),
                fr.x
            ), 
            fr.y
        ),
        mix(
            mix(
                hash31(vec3(lo.xy, hi.z)), 
                hash31(vec3(hi.x , lo.y, hi.z)), 
                fr.x
            ), 
            mix(
                hash31(vec3(lo.x , hi.yz)),
                hash31(hi),
                fr.x
            ), 
            fr.y
        ),
        fr.z
    );
}

float octal(vec3 uv, int octaves, float persistence)
{
    float total = 0.;
    float frequency = 1.;
    float amplitude = 1.;
    float maxValue = 0.;
    for(int i=0;i<octaves;i++) {
        total += noise(uv * frequency) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2.;
    }
    return total/maxValue;
}

vec3 Planet(vec2 uv)
{
    float s=uv.x;
    float t=uv.y;
    float h = octal(
        vec3(s * 5. +time, t * 5. + time * .075, 0. ),
        8,
        .5
    );
    vec3                   col = water;
    if(h >     sandLevel ) col = sand;
    if(h >    grassLevel ) col = grass;
    if(h > mountainLevel ) col = mountain;
    if(h >     snowLevel ) col = snow;
    return vec3(col * h);
}

void main( )
{
    vec2 uv = ((xlv_TEXCOORD0 - .5 * iResolution.xy) / (iResolution.x/res));
    float mag = dot(uv,uv);
    if( mag > 1.0 ){
    	gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
        return;
    }
    vec3 norm = vec3( uv, sqrt( 1.0 - mag ));
    
    float time = iTime;
    float s = atan( norm.z, norm.x ) / (PI * 2.);
    float t = asin( norm.y ) / PI;

    vec2 uv_=vec2(s, t);
#if 0
    gl_FragColor = vec4( Planet(uv), 1. );
#else
    gl_FragColor = vec4( makeSun(uv), 1. );
#endif
    
    vec3 lightDir = normalize( vec3( -1, -0.3, 1.0 ));
    float mflight = max(dot( norm, lightDir), 0.0);
    gl_FragColor.rgb *= mflight;
}

