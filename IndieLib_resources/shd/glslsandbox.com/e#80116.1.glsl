#if 0
#extension GL_OES_standard_derivatives : enable

precision highp float;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#endif

mat2 rotate2D(float r){
    return mat2(cos(r), sin(r), -sin(r), cos(r));
}

float city(vec2 FC, vec2 r, float t) {
    float e=0.0,l=0.0,y=0.0,o=0.0;
    vec3 p,q,d=vec3((FC.xy-.5*r)/r.y,-1);
	//d.z *= time*0.001;
    d.xz*=rotate2D(t*.2);
    e=max(min(++y,-e),y-3.);
	
    for(float i=0.0;i<2e2;i++) {
        i>1e2?l=log(e+=1e-4)/1e2,d/=d,o:o+=exp(-e*1e3)/2e2;
	p=q+=d*e*.7;
        p++;
        y=p.y;
        for (float a2 = 0.; a2 <7.; ++a2) {
            float a = 3. * pow(.6, a2);
            p.xz*=rotate2D(5.);
            p=abs(p)-a;
            e=min(e,max(p.x,(max(p.y,p.z))));
	}
        e=max(min(++y,-e),y-3.);
    }
    o+=l;
    return o;
}

void main( void ) {

	float color = city(xlv_TEXCOORD0.xy, resolution.xy,time);

	gl_FragColor = vec4( vec3(color), 1.0 );

}



