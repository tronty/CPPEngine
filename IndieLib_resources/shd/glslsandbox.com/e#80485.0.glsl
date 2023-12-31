#if 0
#extension GL_OES_standard_derivatives : enable

precision highp float;

//uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#endif

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
	
    for(int i = 1000;i>0;i--) {
	octaves --;
	    if(octaves<1)break;
        total += noise(uv * frequency) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2.;
    }
    return total/maxValue;
}

void mainImage_( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = ((fragCoord - .5 * resolution.xy) / (resolution.x/res));
   
    float radius = 1.;
    vec2 R = resolution.xy;
    
	
    float mag = dot(uv,uv);
    if( mag > 1.0 ){
    	fragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
        return;
    }
    
    
    vec3 norm = vec3( uv, sqrt( 1.0 - mag ));
    
    //float time = iTime;
    float s = atan( norm.z, norm.x ) / (PI * 2.);
    float t = asin( norm.y ) / PI;
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
    fragColor = vec4( vec3(col * h), 1. );
    
    
    
    vec3 lightDir = normalize( vec3( -1, -0.3, 1.0 ));
    float mflight = max(dot( norm, lightDir), 0.0);
    fragColor.rgb *= mflight;
}


void main( ) {mainImage_(  gl_FragColor,  xlv_TEXCOORD0.xy );}

