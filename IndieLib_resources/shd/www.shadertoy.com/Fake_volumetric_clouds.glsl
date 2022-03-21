
const vec3 sunDir = normalize(vec3(-0.6f, 0.4f, 0.6f));

// hash and noise by iq
float hash( float n ) { return fract(sin(n)*43758.5453123); }

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return 2.0f*mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z)-1.0f;
}

float fbm(in vec3 pos, int layers, float AM, float FM)
{
    float sum = 0.0f;
    float amplitude = 1.0f;
    for(int i=0;i<layers;++i)
    {
        sum += amplitude*noise(pos);
        amplitude *= AM;
        pos *= FM;
    }
    return sum;
}

float cloud(in vec3 p)
{
 	//return 0.01f*noise(10.0f*p);  
    return 0.01f*fbm(0.9f*vec3(0.2f, 0.2f, 0.3f)*(p+vec3(0.0f, 0.0f, 3.0f*iTime)), 7, 0.5f, 4.0f);
}

vec2 renderNoise(in vec3 ro, in vec3 rd)
{
    float tmin=10.0f;
    float tmax = 20.0f;
    float delta = 0.1f;
    float sum = 0.0f;
    float t = tmin;
    for(;t<tmax;t+=delta)
    {
        vec3 pos = ro + t*rd;
        //if(pos.y<-10.0f || pos.y>-1.0f || pos.x>5.0f || pos.x<-5.0f) return vec2(sum, -1.0f);
        
        float d = max(0.0f,cloud(pos));
        sum = sum*(1.0-d)+d;
        if(sum>0.99f) break;
    }
    return vec2(sum, t);
}

float shadeClouds(in vec3 ro, in vec3 rd)
{
    float sum =0.0f;
    float t = 0.0f;
    float delta = 0.1f;
    for(int i=0;i<5;++i)
    {
        vec3 pos = ro + rd*t;
        float d = max(0.0f,cloud(pos));
        sum = sum*(1.0-d)+d;
        if(sum>0.99f) break;
        t += delta;
    }
    return sum;
}

vec3 render(in vec3 ro, in vec3 rd)
{
    const vec3 sky = vec3(0.4, 0.6, 1.0);
    vec3 att = vec3(0.2f, 0.5f, 0.9f);
    vec2 ns = renderNoise(ro, rd);
    vec3 pos = ro+rd*ns.y;
    float shad = 1.0f;//0.9f*(1.0f-shadeClouds(pos+sunDir*0.1f, sunDir));
    float density = ns.x;
    float inv = (1.0f-density);
    
    float w = 1.8f*(0.5f*rd.y+0.5f);
    vec3 cl = shad*w*1.0f*mix(vec3(1.0f), inv*att, sqrt(density));
    if(density<0.1f) return mix(sky, cl, max(0.0f, density)*10.0f);
    //vec3 col = mix(sky, cl, 1.0f-exp(-0.0003*ns.y*ns.y) );
    //if(ns.y<0.0f) return sky;
	return cl; 
}

#define PI 3.14159265359f
vec3 render(vec2 ndc, float aspectRatio)
{
	// camera origin
    vec3 o = vec3(0.0f, 0.0f, 0.0f);
	// camera horizontal field of view
    const float fov = 2.0f*PI / 3.0f;
    const float scaleX = tan(fov / 2.0f);
	// camera right vector
    vec3 right = vec3(1.0f, 0.0f, 0.0f)*scaleX;
	// camera forward vector
    vec3 forward = vec3(0.0f, 0.0f, 1.0f);
	// camera up vector
    vec3 up = vec3(0.0f, 1.0f, 0.0f)*scaleX*aspectRatio;
	// ray direction
    vec3 rd = normalize(forward + ndc.x*right + ndc.y*up);
    return render(o, rd);
}

void main( )
{
	vec2 ndc = 2.0f*xlv_TEXCOORD0.xy / iResolution.xy-1.0f;
    vec3 col = render(ndc, iResolution.y/iResolution.x);
    // approximate gamma correction
    col = sqrt(col);
	gl_FragColor = vec4(col,1.0f);
}
