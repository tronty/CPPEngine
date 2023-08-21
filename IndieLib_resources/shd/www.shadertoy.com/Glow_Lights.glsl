// Glow Lights 0.4.230821 by QuantumSuper
// 2.5d arrangement of lights & particles circling an invisible sphere

#define PI 3.14159265359

mat2 rotM(float r){float c = cos(r), s = sin(r); return mat2(c,s,-s,c);} //2D rotation matrix
float hash21(vec2 p){p = fract(p*vec2(13.81,741.76)); p += dot(p, p+42.23); return fract(p.x*p.y);} //pseudorandom generator, cf. The Art of Code on youtu.be/rvDo9LvfoVE

float particle(vec2 p){ //single particle shape
    return smoothstep( .1, .0, length(p)) * smoothstep( .1, .06, length(p-vec2(0.,.02)));
}

float particleLayer(vec2 p){ //pseudo-random 2d particle plane
    float id = hash21(floor(p));
    return smoothstep(0.,1.,id) *
        particle((fract(p)-vec2(.5+.4*cos(id*iTime),.5+.4*sin(.8*id*iTime))) * rotM(id*2.*PI)/vec2(cos(.5*id*iTime),1));
}

void main(){    
    // General initializations
    vec2 uv = (2.*xlv_TEXCOORD0-iResolution.xy) / max(iResolution.x, iResolution.y); //long edge -1 to 1, square aspect ratio
	vec3 col = vec3(0);
    
    // Center orbs 
    vec3 p, camPos = vec3(0,0,-1.5); //vec3 camDir = vec3(0,0,1); 
    float v1, v2, a = 11.;
    for (float n=1.;n<a;n++){            
        v1 = iTime*2.3 + n/a*PI*4.;
        v2 = iTime + n/a*PI;
        p = vec3( cos(v1)*cos(v2), sin(v1)*cos(v2), sin(v2)) * .5; //parametric sphere
        p.yz *= rotM(n); //vary orientation
        col += 1./((p.z-camPos.z)*(p.z-camPos.z)+dot(p.xy,p.xy)) * //vary brightness with distance
            .001 / max( .001, length(uv-camPos.xy-p.xy/(p.z-camPos.z)) - .02/(p.z-camPos.z)) * //orb shape, vary size with distance
            (.5 + clamp( .01/max( .001, length(uv-camPos.xy-p.xy/(p.z-camPos.z)+.005*normalize(p.xy))), .0, .9)) * //light spot
            vec3(mod(n+.3,2.),mod(n*n*.3,2.),mod(n*PI,2.)); //color
    }    

    // Particle layers    
    uv *= rotM(iTime*.1-.5*length(uv)); //rotate inner faster
    float aFrac, amp = 0.; 
    for (float n=0.;n<4.;n++){
        aFrac = fract(-.05*iTime+.25*n)-.02*1.;
        amp += 1.4*(.2+.8*1.)*particleLayer( (uv*length(uv)+n*vec2(.1,.05))*25.*aFrac) * smoothstep(1.,.33,aFrac) * (.1+.9*smoothstep(.33,.66,aFrac));
        
    }
    col *= (1.+amp*40.) * 1.5;
    
    // Finalizations
    col *= .3*hash21(uv*iTime) + .7; //noise
	col -= length(uv) * .005; //vignette
	col = pow(col, vec3(.4545)); //gamma correction    
    gl_FragColor = vec4(col,1.);
}

