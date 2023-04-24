// top -> dielectric with blinn ggx
// middle -> metal with blinn ggx
// bottom -> metal with phong ggx

#define AA 2 // antialiasing

// ray sphere intersection function
// thanks to iq: https://iquilezles.org/articles/intersectors/
float sphIntersect(vec3 ro, vec3 rd, vec3 ce, float ra) {
    vec3 oc = ro - ce;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - ra*ra;
    float h = b*b - c;
    
    if(h<0.) return -1.;
    return -b - sqrt(h);
}

// intersection function
float intersect(vec3 ro, vec3 rd, float tmax, out vec3 on, out int oid) {
    float t = tmax;
    
    for (int i=0; i<18; i++) {
        vec3 ce = vec3((i%6)*2-5,i/6*2-2,0); // sphere center
        float h = sphIntersect(ro, rd, ce, .9);
        if (h>0. && h<t) {
            on = normalize(ro-ce + rd*h); // normal
            oid = i;
            t = h;
        }
    }

    return t<tmax ? t : -1.;
}

// my diffuse model
// lambert + blinn + phong + ggx + shlick

// type -> blinn or phong
// n -> normal
// l -> light direction
// rd -> ray direction
// kl -> light color
// kd -> diffuse color
// ks -> specular color
// km -> roughness / microfacet
// kn -> refraction index
vec3 lighting(int type, vec3 n, vec3 l, vec3 rd, vec3 kl, vec3 kd, vec3 ks, float km, float kn) {
    float ndl = clamp(dot(n, l), 0., 1.); // diffuse/lambert light
    
    float ndh;
    if (type==0) { // blinn
        vec3 h = normalize(l - rd); // half vector
        ndh = dot(n, h);
    } else { // phong
        vec3 r = reflect(rd, n); // reflected vector
        ndh = dot(r, l);
    }
    // ggx specular model
    float g = ndh*ndh * (km*km - 1.) + 1.;
    float ggx = km*km / (3.141592*g*g);

    // shlick approximation
    float fre = 1.+dot(rd, n); // fresnel
    // fresnel amount
    float f0 = (kn-1.)/(kn+1.);
          f0 = f0*f0;
    float kr = f0 + (1.-f0)*(1.-km)*(1.-km) * pow(fre, 5.); // reflectivity
    
    return kl*ndl*(kd + ks*kr*ggx); // diffuse + specular
}

// rendering
vec3 render(vec3 ro, vec3 rd) {
    vec3 col = vec3(.5+.5*rd.y)*.02; // background
    
    vec3 n; int id;
    float t = intersect(ro, rd, 1e10, n, id);
    
    if (t>0.) {
        vec3 p = ro + rd*t;
                        
        vec3 lig = vec3(.57735);
        vec3 ligCol = vec3(2);
        
        vec3 difCol;
        vec3 speCol;
        float rou;
        float ior;
        int type;
        
        int row = id/6;
        if (row==2) { // dielectric
            difCol = vec3(.5,.1,.01);
            speCol = vec3(1); // dieletrics have always white specular
            rou = float(id%6)/5.;
            rou = .002+.998*rou*rou;
            ior = 1.65;
            type = 0; // blinn
        } else {
            difCol = vec3(0);
            speCol = vec3(1,.5,.1);
            rou = float(id%6)/5.;
            rou = .002+.998*rou*rou*1.75;
            ior = 0.; // 0 ior for metals (full specular)
            type = 1-row; // blinn or phong
        }
                        
        col = lighting(type, n, lig, rd, ligCol, difCol, speCol, rou, ior) + // key light
              lighting(type, n, normalize(vec3(0,-1,-2)), rd, .2*ligCol, difCol, speCol, rou, ior) + // rim light
              lighting(type, n, normalize(vec3(-2,0,-.5)), rd, .05*ligCol, difCol, speCol, rou, ior); // fill light
    }
        
    return col;
}

// camera function
mat3 setCamera(vec3 ro, vec3 ta) {
    vec3 w = normalize(ta - ro); // forward vector
    vec3 u = normalize(cross(w, vec3(0,1,0))); // up vector
    vec3 v = cross(u, w); // side vector
    return mat3(u, v, w);
}

void main( )
{
    // normalized mouse coordinates
    vec2 mo = (iMouse.xy - .5*iResolution.xy)/iResolution.y;

    float an = .6*iTime + mo.x*3.141592; // camera xz rotation
    vec3 ro = 22.*vec3(sin(an),0,cos(an)); // ray origin
    vec3 ta = vec3(0); // target
    mat3 ca = setCamera(ro, ta); // camera matrix
    
    vec3 tot = vec3(0);
    for (int m=0; m<AA; m++)
    for (int n=0; n<AA; n++) {
        vec2 off = vec2(m,n) / float(AA) - .5; // AA offset
        vec2 p = (xlv_TEXCOORD0+off - .5*iResolution.xy) / iResolution.y;
        
        vec3 rd = ca * normalize(vec3(p,3)); // ray direction
        vec3 col = render(ro, rd); // render
        
        col = 1.-exp(-col*2.); // tonemapping
        col = pow(col, vec3(.4545)); // gamma correction
        
        tot += col;
    }
    tot /= float(AA*AA);
    
    // vignette
    vec2 q = xlv_TEXCOORD0/iResolution.xy;
    tot *= pow(16. * q.x*q.y*(1.-q.x)*(1.-q.y), .1);

    // output
    gl_FragColor = vec4(tot,1.0);
}

