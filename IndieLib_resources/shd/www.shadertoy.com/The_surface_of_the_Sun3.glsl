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

void main()
{
	vec4 fragColor=vec4( 0.0, 0.0, 0.0, 1.0 );
	vec2 fragCoord=xlv_TEXCOORD0;

    float scale = 0.85;
    vec2 res = iResolution.xy;
    vec2 xy = (2. * fragCoord.xy - res ) / res.y / scale;
    
    float r = sqrt(dot(xy,xy));
    if( r > 1.0 )
    {
    	//fragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
        return;
    }
    
    vec3 p = vec3( xy, sqrt( 1.0 - r*r ));
    float angle = iTime * 0.5;
    vec3 axis = vec3(0.2, 1, 0.6);
    
    // rotational matrix
    float ca = cos(angle);
    float sa = sin(angle);
    axis = normalize(axis);
    
    mat3 rot = mat3(
    ca + axis.x*axis.x*(1.-ca), axis.x*axis.y*(1.-ca) - axis.z*sa, axis.x*axis.z*(1.-ca) + axis.y*sa,
    axis.y*axis.x*(1.-ca) + axis.z*sa, ca + axis.y*axis.y*(1.-ca), axis.y*axis.z*(1.-ca) - axis.x*sa,
    axis.z*axis.x*(1.-ca) - axis.y*sa, axis.y*axis.z*(1.-ca) + axis.x*sa, ca + axis.z*axis.z*(1.-ca));
	
    vec3 rp = rot * p;

#if 0
    float u = 0.5 + atan(rp.z,rp.x) / PI;
    float v = 0.5 - asin(rp.y) / PI;
    fragColor.rgb = texture( iChannel0, vec2( u, v ));
#elif 1
    float u = 0.5 + atan(rp.z,rp.x) / PI;
    float v = 0.5 - asin(rp.y) / PI;
    fragColor.rgb = vec3(makeSun(vec2( u, v )));
#else
    vec3 hitPos=rp;
    float2 chekerUv = getUV(hitPos);
    fragColor.rgb = vec3(makeSun(chekerUv));
#endif
#if 0
    // directional light
    vec3 lightDir = normalize( vec3( -1.1, -0.3, 1.0 )); 
    float mflight = max(dot( p, lightDir), 0.0);
    fragColor.rgb *= mflight;
    

    // point light
    vec3 light1pos = vec3( 0.6, 0.5, 0.6 ); 
    vec3 light1col = vec3( 1.0, 2.0, 3.0 ); 
    float dis = 1.0 - length( p - light1pos ) / 2.0;
    fragColor.rgb += light1col * pow( dis, 8.0);
#endif
	gl_FragColor=fragColor;
}

