/** 
    License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

    Soothing Motion II
    05/01/22 | byt3_m3chanic

    Daily shading - Playing around more with just motion and ended up here. 
    The balls are an illusion with the ID's rolling up 1 in the y while the
    animation loops in place as you move over the scene.
    
    
*/

#define MIN_DIST    .0001
#define MAX_DIST    75.

#define PI          3.1415926
#define PI2         6.2831853

//global
vec3 FC = vec3(0.596,0.812,0.941);
vec3 hp=vec3(0),hitPoint=vec3(0);
vec2 gid=vec2(0), sid=vec2(0);
float glow;
float zoom = 10.;
float maptime,fractTime,modTime,iss;
mat2 crt1,crt2;

//constants
const float space = 9.5;
const float hlf = space/2.;
const float dbl = space*2.;
const vec3 bsize = vec3(3,5,3);
const vec3 wsize = vec3(1,2,1.65);
    
const float c = 1.5;
const float c2=c/2.;
    
// Helper functions for building smin stairs
// http://mercury.sexy/hg_sdf/
float fOpUnionStairs(float a, float b, float r, float n) {
	float s = r/n, u = b-r;
	return min(min(a,b), 0.5 * (u + a + abs ((mod (u - a + s, 2. * s)) - s)));
}
float fOpDifferenceStairs(float a, float b, float r, float n) {return -fOpUnionStairs(-a, b, r, n);}

float hash21( vec2 p ) {return fract(sin(dot(p,vec2(23.43,84.21)))*4832.3234);}

mat2 rot(float a){ return mat2(cos(a),sin(a),-sin(a),cos(a)); }

//@Dave_Hoskins 
//https://www.shadertoy.com/view/4djSRW
float hash12(vec2 p){
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

//@iq for all the SDF's
float box( vec3 p, vec3 b ) {
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float cyl(vec3 p, float r, float h) {
	float d =length(p.xy)-r;
	return max(d,abs(p.z)-h);
}

vec2 map(vec3 p, float sg) {
	vec2 res = vec2(1e5,0);
    vec3 q = p-vec3(0,-1.25,maptime);
    vec3 r = q;
    
    float flr = q.y;
    if(flr<res.x) {
        res = vec2(flr,1.);
        hp=q;
    }
    
    float qd = floor((q.z+hlf)/space);
    q.z=mod(q.z+hlf,space)-hlf;

    float pole = min(cyl(q.zxy-vec3(4,5,0),.045,4.),length(q-vec3(5,4,4))-.1 );
    if(pole<res.x) {
        res = vec2(pole,3.);
        hp=q;
    }
    
    vec3 nq=q;
    float wave = .5*sin(nq.z*2.35+maptime);
    float flo = clamp(2.-(nq.z*.5),0.,1.);
    wave = mix(0.,wave,flo);
    nq.x+=wave;
    
    float flag = box(nq-vec3(5,3.35,3),vec3(.03,.5,1));
    if(flag<res.x) {
        res = vec2(flag,6.);
        hp=nq;
        gid=vec2(qd,1.);
    }

    q.x = abs(q.x)-bsize.x;

    float wall = box(q,vec3(.25,bsize.y,bsize.z));
    float roof = box(q-vec3(-(bsize.x-.25),bsize.y-.5,0.),vec3(3.,.1,bsize.z));
    
    vec3 q2 = q, q3 = q;
    
    q2.z = abs(q2.z)+.6;
    float window = box(q2-vec3(0,wsize.y/2.,0),vec3(.4,wsize.x,wsize.z));
    window=min(cyl(q2.zyx-vec3(0,wsize.y,0.),wsize.z,.4),window);
    wall = min(box(q2-vec3(.3,bsize.y/2.,bsize.z),vec3(.1,bsize.y/2.,.05)),wall);

    float lits = length(q2-vec3(.575,4.65,2.5))-.05;
    if (lits<res.x && sg==1.) {
        res = vec2(lits,4.);
    	glow += .0005/(.000025+lits*lits);
    }

    float qi = floor((q3.z + c2)/c);
    q3.z = q3.z-c*clamp(round(q3.z/c),-2.,2.);
    float tip1 = box(q3-vec3(-.1,bsize.y,0),vec3(.08,.45,.5));

    float tip=min(cyl(q2.zyx-vec3(0,bsize.y,-.1),1.,.08),tip1);
    roof = min(tip,roof);
    
    float mainwall = fOpDifferenceStairs(wall,window,.25,3.);
    float building = fOpUnionStairs(mainwall,roof,.25,3.);

    if(building<res.x) {
        res = vec2(building,2.);
        hp=q3;
        gid=vec2((tip1>res.x)?qd:qd-.5,1.);
    }
    
    vec2 id = vec2(
        floor((r.x+space)/dbl),
        floor((r.z+hlf)/space)
        );
        
    float hs,fs;
    if(modTime<2.){
      	hs = hash21(id.yy);
        r.x += (hs<.5) ? fractTime : -fractTime; 
    } else {
        r.z += -fractTime;
    }
    
    id = vec2(floor((r.x+space)/dbl),floor((r.z+hlf)/space));
    id.y-=iss;
    r.xz=vec2(mod(r.x+space,dbl)-space,mod(r.z+hlf,space)-hlf);
    r-=vec3(0,1,0);
    
    if(modTime<2.){
        r.xy*= (hs<.5) ? crt1 :  crt2 ;
    } else {
        r.zy*= crt2 ;
    }
    
    float ball = length(r)-.9;
    if(ball<res.x) {
        res = vec2(ball,5.);
        hp=r;
        gid=vec2(id);
    }
    return res;
}

vec3 normal(vec3 p, float t) {
    t*=MIN_DIST;
    float d = map(p,0.).x;
    vec2 e = vec2(t,0);
    vec3 n = d - vec3(
        map(p-e.xyy,0.).x,
        map(p-e.yxy,0.).x,
        map(p-e.yyx,0.).x
    );
    return normalize(n);
}

vec2 marcher(vec3 ro, vec3 rd, inout vec3 p, inout bool hit, int steps) {
    hit = false; float d=0., m = 0.;
    for(int i=0;i<steps;i++) {
        p = ro + rd * d;
        vec2 t = map(p,1.);
        if(abs(t.x)<d*MIN_DIST) hit = true;
        d += i<32? t.x*.5:t.x;
        m  = t.y;
        if(d>MAX_DIST) break;
    } 
    return vec2(d,m);
}

vec3 hue(float t) { 
    vec3 d = vec3(0.141,0.447,0.941);
    return .45+.4*cos( PI2*t*vec3(0.984,0.980,0.914)+d ); 
}

vec3 render(inout vec3 ro, inout vec3 rd, inout vec3 ref, int bnc, inout float d, vec2 F) {
        
    vec3 C=vec3(0);
    vec3 p = ro;
    float m = 0.;
    bool hit = false;
    
    vec2 ray = marcher(ro,rd,p,hit,80);
    d = ray.x;
    m = ray.y;
    hitPoint = hp;
    sid=gid;
    if(hit) {
  
        vec3 n = normal(p,d);
        vec3 lpos = vec3(5.5,8,8.5);
        vec3 l = normalize(lpos);
        float diff = clamp(dot(n,l),0.,1.);
       
        float shdw = 1.;
        for( float t=.01;t<18.; ) {
            float h = map(p + l*t,0.).x;
            if( h<MIN_DIST ) { shdw = .001; break; }
            shdw = min(shdw, 14.*h/t);
            t += h;
            if( shdw<MIN_DIST || t>18. ) break;
        }
        diff = mix(diff,diff*shdw,.35);

        vec3 ret = reflect(normalize(lpos),n);
        float spec = .75 * pow(max(dot(normalize(p-ro),ret),0.),24.);
        vec3 h = vec3(.25);

        if(m==1.) {
            vec2 f = hitPoint.xz;
            h=FC;
            
            if(abs(f.x)<4.5) {
                h=vec3(.5);
                vec2 ff=fract(hitPoint.xz*.77)-.5;
                if(ff.x*ff.y>0.)h=vec3(.2);
            }
            
            if(abs(f.x)>4.5&&abs(f.x)<5.5) h=vec3(.15);

            ref = h*.3;
        }
        
        if(m==2.||m==6.) {
            h=hue(hash21(sid+m)*.982);
            ref = h*.3;
        }
        
        if(m==5.) {
            hitPoint.xy *= rot(1.5707);
            vec3 f = fract(hitPoint*2.)-.5;
            f.x=abs(f.x)-.2;
            h=mod(sid.y,2.)<1. ? vec3(.01):vec3(.9);
            ref=vec3(.1);
            if(f.x*1.>0.) {h=hue(hash21(vec2(sid.y,25.))*3.13);  ref = h;}
           
        }
        
        if(m==6.) {
            vec2 f = fract(hitPoint.zy*3.5)-.5;
            h=vec3(.009);
            if(f.x*f.y>0.)h=hue(hash21(sid+12.)*3.5);
            ref = h;
        }
        
        C = (m!=2.) ? h * diff + min(shdw,spec):h * diff;
        C = mix(C,vec3(.90,.49,.82)*vec3(glow*1.15),clamp(.0,1.,glow)); 
        //if(bnc<2) C = mix(C,FC, 1.-exp(-.001*d*d*d));//slows down some
        ro = p+n*.1;
        rd = reflect(rd,n);
        
    } else {
        C=FC;
    }

    return clamp(C,vec3(.0001),vec3(1));
}

void main( ) { 
    vec3 col = vec3(.00);
    //precal
    float fTime = iTime*.5; 
    float ms = 3.;
    float mt = 1./ms;
    modTime = mod(fTime,ms);
    float os = floor(modTime);
    float ios = floor(fTime+1.);
    iss = floor(ios*mt)-1.;

    fractTime = modTime<2.? (fract(fTime) * (space))+(space*os) : fract(fTime) * (space);
    maptime = fTime*5.;
    //ball rotations
    crt1 = rot(modTime*PI2);
    crt2 = rot(-modTime*PI2);
    
    vec2 uv = (-iResolution.xy + 2. * (xlv_TEXCOORD0)) / iResolution.x;
    
    // mouse //
    float x = iMouse.xy==vec2(0) ? 0. : -(iMouse.y/iResolution.y*4.-2.)*PI;
    float y = iMouse.xy==vec2(0) ? 0. : -(iMouse.x/iResolution.x*2.-1.)*PI;
    zoom+=x;
    // ro + rd
    vec3 ro = vec3(uv*zoom,-zoom-15.);
    vec3 rd = vec3(0,0,1.);
    
    FC=hue((uv.y+125.)*.25);
    
    mat2 rx =rot(.425);
    mat2 ry =rot(-.98+y);
    ro.zy*=rx;rd.zy*=rx;
    ro.xz*=ry;rd.xz*=ry;

    vec3 C=vec3(0), RC=vec3(0), ref=vec3(0), fill=vec3(1);
    vec3 p = ro;
    float m = 0., d = 0., fA = 0.;

    int bnc = 2;
    for(int i = 0; i < bnc; i++){
        RC = render(ro,rd,ref,bnc-i,d,xlv_TEXCOORD0);
        C += RC*fill;
        fill *= ref; 
        if(i==0) fA += d*.2;
    }

    C = mix(C,FC, 1.-exp(-.002*fA*fA*fA));
    clamp(C,vec3(0),vec3(1));
    C = pow( C, vec3(0.4545) );

    gl_FragColor = vec4(C, 0);
}

