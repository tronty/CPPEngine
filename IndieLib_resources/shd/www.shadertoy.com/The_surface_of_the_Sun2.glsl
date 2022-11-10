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

vec2 rayMarching(in vec3 camPos, in vec3 rayDirection)
{
    float dMin = 1.;
    float dMax = 50.;
    float precis = 0.002;
    float traveledDistance = dMin;
    float color = -1.;
    
    for( int i = 0 ; i < 50 ; i++ )
    {
        vec2 res = scene( camPos + (rayDirection * traveledDistance) );
        
        if( res.x<precis || traveledDistance>dMax )
        {
            break;
        }
        
        traveledDistance += res.x;
        color = res.y;
    }
    
    if( traveledDistance > dMax )
    {
        color = -1.0;
    }
    return vec2( traveledDistance, color );
}

mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
    vec3 cw = normalize(ta-ro);//z (dir)
    vec3 cp = vec3(sin(cr), cos(cr),0.0);//haut
    vec3 cu = normalize( cross(cw,cp) );//x (droite/gauche)
    vec3 cv = normalize( cross(cu,cw) );//y (haut normalisé)
    return mat3( cu, cv, cw );
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

vec2 arrangeCoords(vec2 p)
{
    vec2 q = p.xy/iResolution.xy;
    vec2 r = -1.0+2.0*q;
	r.x *= iResolution.x/iResolution.y;
    return r;
}

#define pi 3.14159265
#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)
void main( )
{
#if 1
	vec2 resolution = iResolution.xy;
	vec2 texCoord = xlv_TEXCOORD0.xy / resolution.xy;
	texCoord = vec2(texCoord.y,texCoord.x);
	vec2 position = ( xlv_TEXCOORD0.xy / resolution.xy );
	
	vec2 center = resolution.xy / 2.;
	float dis = distance(center, xlv_TEXCOORD0.xy);
	float radius = resolution.y / 3.;
	vec3 atmosphereColor = vec3(.7, .6, .5);
	if (dis < radius) {
		// Find planet coordinates
		vec2 posOnPlanet = (xlv_TEXCOORD0.xy - (center - radius));
		vec2 planetCoord = posOnPlanet / (radius * 2.0);
            	gl_FragColor = vec4(vec3(makeSun(planetCoord)), 1);
		return;
	}
	gl_FragColor = vec4(0,0,0,1);
#else
    vec2 p = arrangeCoords(xlv_TEXCOORD0);
     
    vec3 lookAt = vec3(0.);
    vec3 camPos = vec3(5. * sin(iTime*0.3), 3. , -4. * cos(iTime*0.3));
    camPos = vec3(2,2.1,2.);
    
    mat3 camera = setCamera(camPos, lookAt, 0.);
    
    vec3 rayDirection = camera * normalize( vec3(p.xy,2.0) );
	#if 0
    R(rayDirection.yz, -iMouse.y*0.01*pi*2.);
    R(rayDirection.xz, iMouse.x*0.01*pi*2.);
	#endif

    vec2 rayResult = rayMarching(camPos, rayDirection);     
    
    float rayDistance = rayResult.x;
    float rayColor = rayResult.y;
    vec3 hitPos = camPos + rayDirection*rayDistance;
    vec2 chekerUv;
    
    vec3 color;
    
    vec3 sphereColor = vec3(0, 0, 0);  
    
    if(rayColor > 1.)
    {
        color = vec3(1. , 0.4 ,0.1 );
     
        if(rayDistance>1.)
        {
        	chekerUv = getUV(hitPos);
            sphereColor = vec3(makeSun(chekerUv));
        }       
    }
    else
    {
        color = vec3(0., 0., 0.);
    }
   
    vec3 pos = camPos + rayDistance * rayDirection;
    vec3 nor = calcNormal( pos );
    	
    gl_FragColor = vec4(sphereColor, 1);
#endif
}

