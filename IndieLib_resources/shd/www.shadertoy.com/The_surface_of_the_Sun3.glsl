const float EARTH_RADIUS = 1.0;
const float MAX = 10000.0;
#define PI 3.14159265358979323844
float iteration = 10.;
float timeScale = 3.;
vec2 zoom = vec2(25.,5.5);
vec2 offset = vec2(0,2.);

//////////////////////////////////////////////////////////////////////

float sdSphere( vec3 p, float s )
{
    return length(p)-s;
}

vec2 scene( in vec3 pos )//reception d'une sphere
{
    vec3 dim = vec3(1, 1, 1);
    
    pos += vec3(0, 0., 0);
    
    float resSphere = sdSphere(pos, 1.3);
    
   
    vec2 res = vec2(resSphere, 2.);
    
    
    return res;
}

vec3 calcNormal( in vec3 pos )
{
    vec3 eps = vec3( 0.001, 0.0, 0.0 );
    vec3 nor = vec3(
                    scene(pos+eps.xyy).x - scene(pos-eps.xyy).x,
                    scene(pos+eps.yxy).x - scene(pos-eps.yxy).x,
                    scene(pos+eps.yyx).x - scene(pos-eps.yyx).x );
    return normalize(nor);
}

vec2 getUV(vec3 pos)
{
    vec3 nor = calcNormal(pos);
    float lon = atan(nor.x,nor.z)/3.14;
    float lat = acos(nor.y)/3.14;
    vec2 r = vec2(lat, lon);
    
    return r;
}


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
#if 1
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
#else
	c0=c;
#endif
    
	return vec3(sqrt(c0.r)*1.2,c0.r*c0.r*.9,c0.r*c0.r*c0.r*.4);
}

#if 1
void main()
{
    vec3 col = vec3(1,0,0);
#if 1
    vec2 p = -1.0 + 2.0 * (xlv_TEXCOORD0.xy) / resolution.xy;
    p.x *= resolution.x/resolution.y;
	vec2 texCoord=xlv_TEXCOORD0.xy / resolution.xy;
    // make us round
    float d = texCoord.x * texCoord.x + texCoord.y * texCoord.y;

    if (d > 1.0)
        discard;

    // we're in, compute the exact Z
    float z = sqrt(1.0 - d);

    // get light intensity
    vec4 point = vec4(texCoord.xy, z, 1.0);

    float l = clamp(dot(point, lightPos), minLight, 1.0);

    // rotate
    point *= rotation;

    // get texture coordinates (I believe this could be replaced with a
    // precomputed texture lookup, if you need more performance)
    float x = (atan(point.x, point.z) + PI) / TWOPI,
          y = (asin(point.y) + PIP2) / PI;
	vec2 uv=vec2(x,y);
	vec3 rp=point.xyz;
	vec3 hitPos=rp;
	float2 chekerUv = getUV(hitPos);
	col = vec3(makeSun(chekerUv));
#endif
	gl_FragColor=vec4(col, 1.0);
}
#endif

