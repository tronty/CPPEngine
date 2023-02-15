// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// Created by S. Guillitte 2021
 

mat2 rot(float a) {
	return mat2(cos(a),sin(a),-sin(a),cos(a));	
}



//reef/waves combined field
vec3 field(in vec3 p) {
	
	float s=2.,e,f,o=1.;
    vec3 q=p,r=p;
    vec3 n = vec3(0);
    mat2 m = rot(1.);
	vec2 l = vec2(2.);
	for(e=f=p.y;s<4e2;s*=1.7)
            p.xz*=m,
            n.xy*=m,
            q=p*s+n,
            r=p*s+n,
            r.x+=iTime*2.,
            e+=abs(dot(sin(r.xz*.1)/s,.8*l)),
            f+=.2+.5*(dot(sin(q.xz*.5)/s,l)),
            n-=cos(q);
	o+=(f>.001 ? (e<0.01 ? -exp(-e*e):f*e):-exp(-f*f));
    return vec3(max(o,0.),e,f);
}


vec3 raycast( in vec3 ro, vec3 rd )
{
    float t = 3.5, dt = .3,c,d,e,f;
    vec3 col= vec3(0.);
    for( int i=0; i<80; i++ )
	{        
        vec3 v = field(ro+t*rd); 
        c=v.x; e=v.y; f=v.z; d=min(min(e,2.*e*f),f);
        t+=dt*d;
        dt *= 1.01;
        e=exp(-e);f=exp(-f);
        col = .93*col+ .4*c*(f*vec3(5,3,2)+e*vec3(1,5,7)+.2/v.y);        
    }    
    return col*exp(-t*.15);
}


void main( )
{
	float t = iTime;
    vec2 q = xlv_TEXCOORD0.xy / iResolution.xy;
    vec2 p = -1.0 + 2.0 * q;
    p.x *= iResolution.x/iResolution.y;
    
    // camera

    vec3 ro = vec3(2.);
   
    ro.yz*=rot(-1.6);
    
    ro.y +=3.;
    ro.xz*=rot(0.1*t);
    
    vec3 ta = vec3( 0.0 , 0.0, 0.0 );
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    vec3 rd = normalize( p.x*uu + p.y*vv + 4.0*ww );
    ro.x -= t*.4;
    
	// raymarch 
    
    vec3 col = raycast(ro,rd);
    	
	// shade
    
    col =  .5 *(log(1.+col));
    col = clamp(col,0.,1.);
    gl_FragColor = vec4( col, 1.0 );
}
