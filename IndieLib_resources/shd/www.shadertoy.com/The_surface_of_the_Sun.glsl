// Created by Alex Kluchikov

vec2 rot(vec2 p,float a)
{
    float c=cos(a*15.83);
    float s=sin(a*15.83);
    return p*mat2(s,c,c,-s);
}

void main( )
{
    vec2 uv=xlv_TEXCOORD0/iResolution.xx;
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
    
	gl_FragColor=vec4(sqrt(c0.r)*1.2,c0.r*c0.r*.9,c0.r*c0.r*c0.r*.4,1.0);
}
