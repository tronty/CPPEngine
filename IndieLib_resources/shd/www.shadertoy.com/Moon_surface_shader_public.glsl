const float M_PI2 = 3.14159265359*2.0;
const float M_PI6 = 3.14159265359/6.0;
const float M_PI3 = 3.14159265359/3.0;
const float sqrt32 = 2.0/sqrt(3.0);
const float sr = sqrt(3.0)/2.0;
const vec2 rat = vec2(1.0,sr);

// From https://www.shadertoy.com/view/4djSRW
float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);

}


float hash(vec2 v) { return dot(v,vec2(1., -1178.65)); }
float rnd(float x) { return fract(sin(234.56*x)); }
vec3 rnd3(float x) { return vec3(rnd(x),rnd(x+.1),rnd(x+.2)); }


float HexagonCircles(vec2 uv){
    vec2 uv2 = (uv)/rat;
    float height = 0.0;
    for (int i=0; i<7; i++)
    {
        float r = float(i)*M_PI3-M_PI6;
        vec2 uv3 = uv2-vec2(0.5,0.75)+((i!=6)?vec2(sin(r),cos(r)*sqrt32):vec2(0.0,0.0));
        vec2 hexas = uv3+vec2(floor(uv3.y)*0.5,1.0);      
        vec2 c = uv3+vec2(1.0,1.0)-fract(hexas);
        vec2 seed = floor(hexas);
        vec3 offset = rnd3(seed.y*23.4+seed.x*6.47);
        float n = hash(floor(hexas));
        float radius = 0.125*(rnd(n)*0.5+1.0);
        vec2 dv = (c-uv2)*rat+offset.zx*(0.5-radius);
        float d = length(dv);
        
    
        //float dHeight= min((radius-dmin)*(radius-dmin),(dmin-radius)*(dmin-radius))/(radius*radius);
        //float dHeight= dmin<radius?dmin-radius:radius-(dmin-radius);
        float slopeUp = d/radius;
        float slopeDown = (radius-(d-radius))/radius;
        float dHeight = d<radius?slopeUp*slopeUp:max(slopeDown*slopeDown,0.5);
        //height =height:max(height,dHeight);
        height = radius*2.>d?dHeight:0.5;
        
    }
    
    
    return (min(max(0.0,height),1.0)*2.)-1.0;
}


float Craters(vec2 uv){
    float sum=0.0;
    float times = 3.;
    for(int i=0; i< int(times*2.);i++){
        sum +=HexagonCircles((uv+vec2(1.,1)*float(i))*2.)/(times*4.);
    }
    for(int i=0; i< int(times);i++){
        sum +=HexagonCircles(uv+vec2(1,1)*float(i))/(times*2.);
    }
    sum += HexagonCircles((uv+vec2(3.,3))/2.0)/4.;
    sum += HexagonCircles(uv/2.0)/3.;
    sum += HexagonCircles(uv/4.0)/2.;
    sum += HexagonCircles(uv/16.0)/1.5;
    return sum;
}


vec2 calcuv(vec2 uv){
    float d = (sin(iTime*0.057843));
    float r = iTime*0.054387;
    float sr= sin(r);
    float cr= cos(r);
    float zx= 8.0-6.0*d;
    uv= uv- vec2(iResolution.x/iResolution.y, 1.0)*0.5;
    uv= uv* mat2(sr,cr,-cr,sr)*zx;
    uv= uv + vec2(iResolution.x/iResolution.y,1.0)*0.5;
    return uv;    
}

float Moon(vec2 uv) {
    uv = calcuv(uv);
    
    float craters=Craters(uv);
    return craters+0.5;
}

void main( )
{
    
	vec2 uv = vec2(xlv_TEXCOORD0.xy / iResolution.xy)*
      vec2(iResolution.x/iResolution.y,1.0); //scale to normal width

    vec2 step = 1.0/iResolution.xy;

    float moon = Moon(uv);
    float moondx = Moon(uv+step.x);
    float moondy = Moon(uv+step.y);
    vec2 sand = hash22(floor(calcuv(uv)*iResolution.xy*0.25));
    vec2 moondxy = vec2(moondx-moon, moondy-moon)+0.01*sand;
    
    vec3 light = normalize(vec3(0.75,0.5,0.5));
    
    vec3 moonnormal = normalize(vec3(moondxy,0.0))*0.75;
    float lumina = dot(moonnormal,light);
    float depth =moon;
	float ambient =0.5;
    float lightness= lumina*depth+ambient;
    vec3 color = vec3(0.9,0.86,0.82);
    gl_FragColor = vec4(lightness*color,1.0);
}
