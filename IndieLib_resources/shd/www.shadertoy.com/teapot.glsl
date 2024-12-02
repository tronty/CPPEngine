uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
//-----------------------------------------------------
// Created by sebastien durand - 2021
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//-----------------------------------------------------
// inspired by nguyen nhut work https://twitter.com/nguyenhut_art
// Teapot alone: https://www.shadertoy.com/view/XsSGzG
//-----------------------------------------------------

#define PI 3.14159265

#define WITH_AO

#define ZERO (min(0, iFrame))
.
#define TEAPOT 6.
#define METAL 8.

#define WITH_SPIRAL

float gTime;

mat2 rot, rot1, rot2;

#define U(a,b) (a.x*b.y-b.x*a.y)

vec2 A[15];
vec2 T1[5];
vec2 T2[5];

float smin(float a, float b, float k){
    float h = clamp(.5+.5*(b-a)/k, 0., 1.);
    return mix(b,a,h)-k*h*(1.-h);
}

vec2 min2(vec2 a, vec2 b) {
    return a.x<b.x ? a : b; 
}

#ifdef WITH_SPIRAL
float sdSpiral(vec3 p, vec2 sz){
    float d = length(p.xy)/sz.x + atan(-p.y,p.x)*.5;  
    d -= PI*clamp(round(d/PI),1.,4.);
    return max(sz.x*(abs(d)-.77), abs(p.z)-sz.y); 
}
#endif

// Distance to Bezier
// inspired by [iq:https://www.shadertoy.com/view/ldj3Wh]
// calculate distance to 2D bezier curve on xy but without forgeting the z component of p
// total distance is corrected using pytagore just before return
vec2 sdBezier(vec2 m, vec2 n, vec2 o, vec3 p) {
	vec2 q = p.xy;
	m-= q; n-= q; o-= q;
	float x = U(m, o), y = 2. * U(n, m), z = 2. * U(o, n);
	vec2 i = o - m, j = o - n, k = n - m, 
		 s = 2. * (x * i + y * j + z * k), 
		 r = m + (y * z - x * x) * vec2(s.y, -s.x) / dot(s, s);
	float t = clamp((U(r, i) + 2. * U(k, r)) / (x + x + y + z), 0.,1.); // parametric position on curve
	r = m + t * (k + k + t * (j - k)); // distance on 2D xy space
	return vec2(sqrt(dot(r, r) + p.z * p.z), t); // distance on 3D space
}

//-----------------------------------------------------------------------------------
// iq - https://www.shadertoy.com/view/ldj3Wh
vec2 sdBezier(in vec3 p,in vec3 b0,in vec3 b1,in vec3 b2 ) {
    b0 -= p; b1 -= p; b2 -= p;
    vec3 b01 = cross(b0,b1), b12 = cross(b1,b2), b20 = cross(b2,b0),
         n =  b01+b12+b20;
    float a = -dot(b20,n), b = -dot(b01,n), d = -dot(b12,n), m = -dot(n,n);
    vec3  g =  (d-b)*b1 + (b+a*.5)*b2 + (-d-a*.5)*b0;
    float t = clamp((a*.5+b-.5*(a*a*.25-b*d)*dot(g,b0-2.*b1+b2)/dot(g,g))/m, 0., 1.);
    return vec2(length(mix(mix(b0,b1,t), mix(b1,b2,t),t)),t);
}

// Distance to scene
float sdTeapot(vec3 p) {
// Distance to Teapot --------------------------------------------------- 
	// precalcul first part of teapot spout
	vec2 h = sdBezier(T1[2],T1[3],T1[4], p);
	float a = 99., 
    // distance to teapot handle (-.06 => make the thickness) 
		b = min(min(sdBezier(T2[0],T2[1],T2[2], p).x, sdBezier(T2[2],T2[3],T2[4], p).x) - .06, 
    // max p.y-.9 => cut the end of the spout 
                max(p.y - .9,
    // distance to second part of teapot spout (abs(dist,r1)-dr) => enable to make the spout hole 
                    min(abs(sdBezier(T1[0],T1[1],T1[2], p).x - .07) - .01, 
    // distance to first part of teapot spout (tickness incrase with pos on curve) 
                        h.x * (1. - .75 * h.y) - .08)));
    // distance to teapot body => use rotation symetry to simplify calculation to a distance to 2D bezier curve
    vec3 qq= vec3(sqrt(dot(p,p)-p.y*p.y), p.y, 0);
    // the substraction of .015 enable to generate a small thickness arround bezier to help convergance
    // the .8 factor help convergance  
	for(int i=ZERO;i<13;i+=2) 
		a = min(a, (sdBezier(A[i], A[i + 1], A[i + 2], qq).x - .035) * .9); 
    // smooth minimum to improve quality at junction of handle and spout to the body
	return smin(a,b,.02);
}

// Interpolate pos of articulations
vec3 getPos(vec3 arr[9], int it, float kt, float z) {
    it = int(mod(1.0*it,8.0));
    vec3 p = mix(arr[it], arr[it+1], kt);
	return .02*vec3(p.x+floor(gTime/8.)*168., 150.-p.y, p.z*z);
}

//---------------------------------------------------------------------
//    HASH functions (iq)
//---------------------------------------------------------------------

vec2 hash22(vec2 p) {
    p = fract(p * vec2(5.3983, 5.4427));
    p += dot(p.yx, p.xy + vec2(21.5351, 14.3137));
    return fract(vec2(p.x * p.y * 95.4337, p.x * p.y * 97.597));
}

//---------------------------------------------------------------------
//   Modeling Primitives
//   [Inigo Quilez] https://iquilezles.org/articles/distfunctions
//---------------------------------------------------------------------

vec2 sdMan(in vec3 pos){
    vec3 p0 = pos;
    vec2 res = vec2(999,0);
    
    // Teapot
    vec3 posTeapot = 3.*(pos - vec3(-.9,.25,.4)).yxz;
    posTeapot.xy *= rot2;
    posTeapot.yz *= rot;
    posTeapot.yx *= -1.; 
    float dTeapot = .7*sdTeapot(posTeapot-vec3(1.,-1.,0))/3.;

    // Asssociate distance to materials
    res = min2(res, vec2(dTeapot, TEAPOT));
    
    // Distance field is not percise for cap, teapot and hands
    res.x *= .8;
    return res;
}

vec2 map(in vec3 p0){
    // Little stones    
    vec2 size = vec2(35.,20.),
         id = floor((p0.xz + size*0.5)/size);
    vec3 pos = p0;
    pos.xz = mod(p0.xz + size*0.5,size) - size*0.5;
    vec2 h = 1.-2.*hash22(id);
    float r = .15+.25*abs(h.x),
          d = length(pos - vec3(h.x*5.,-r*.4,7.*h.y))-r;
    vec2 res = vec2(d,GROUND);

    d = length(p0-hip1)-2.;
    if (d<0.) {
        return min2(sdMan(p0), res);
    } else {    
        return min2(vec2(d+.1,999.), res);
    }
}

//---------------------------------------------------------------------
//   Ray marching scene if ray intersect bbox
//---------------------------------------------------------------------

vec2 Trace( in vec3 ro, in vec3 rd) {
    vec2 res = vec2(999,0);
    float t = .5;
    for( int i=ZERO; i<128 && t<100.; i++ ) {
        vec2 h = map( ro+rd*t);
        if( abs(h.x)<.0005*t ) { 
            res = vec2(t,h.y); 
            break;
        }
        t += h.x;
    }
    return res;
}

//------------------------------------------------------------------------
// [Shane] - Desert Canyon - https://www.shadertoy.com/view/Xs33Df
//------------------------------------------------------------------------
// Tri-Planar blending function. Based on an old Nvidia writeup:
// GPU Gems 3 - Ryan Geiss: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch01.html
float tex3D(sampler2D tex, in vec3 p, in vec3 n){
    n = max(n*n, .001);
    n /= (n.x + n.y + n.z );  
	return (texture2D(tex, p.yz)*n.x + texture2D(tex, p.zx)*n.y + texture2D(tex, p.xy)*n.z).x;
}

// Texture bump mapping. Four tri-planar lookups, or 12 texture2D lookups in total.
vec3 doBumpMap( sampler2D tex, in vec3 p, in vec3 n, float k){
    const float ep = .001;
    vec3 grad = vec3( tex3D(tex, vec3(p.x-ep, p.y, p.z), n),
                      tex3D(tex, vec3(p.x, p.y-ep, p.z), n),
                      tex3D(tex, vec3(p.x, p.y, p.z-ep), n));
    grad = (grad - tex3D(tex, p, n))/ep;             
    grad -= n*dot(n, grad);          
    return normalize(n + grad*k);
}

//---------------------------------------------------------------------
//   Ambiant occlusion
//---------------------------------------------------------------------

#ifdef WITH_AO
float calcAO( in vec3 pos, in vec3 nor ){
	float dd, hr, sca = 1., totao = 0.;
    vec3 aopos; 
    for( int aoi=ZERO; aoi<5; aoi++ ) {
        hr = .01 + .05*float(aoi);
        aopos = nor * hr + pos;
        totao += -(map(aopos).x-hr)*sca;
        sca *= .75;
    }
    return clamp(1. - 4.*totao, 0., 1.);
}
#endif

float textureInvader(vec2 uv) {
	float y = 7.-floor((uv.y)*16.+4.);
	if (y<0. || y>7.) return 0.;
	float x = floor((abs(uv.x))*16.),
	      v=y>6.5? 6.:y>5.5? 40.:y>4.5? 47.:y>3.5?63.:
			y>2.5? 27.:y>1.5? 15.:y>0.5? 4.:8.;
	return floor(mod(v/pow(2.,x), 2.0)) == 0. ? 0.: 1.;
}

vec3 doColor(in vec3 p, in vec3 rd, in vec3 n, in vec2 res){
    // sky dome
    vec3 skyColor =  .5*vec3(.5, .6, .9),
         col = skyColor - max(rd.y,0.)*.5;
  
    float ss = .5, sp = 0.;
if (res.y == TEAPOT) {
        col = vec3(1.,.01,.01);
        sp = .5;
    } else if (res.y == BED) {
        col = vec3(1.,.5,.01);
        ss = 1.;
        sp = .3;
    }
    else {
        return col;
    }
    vec2 d = vec2(res.x, res.y);
    vec3 ld = -normalize(vec3(50,100,-100)-p);
    // IQ sss version
    float sss = ss*0.2*clamp(0.5+0.5*dot(ld,n),0.0,1.0)*(2.0+dot(rd,n));
    vec3 r = reflect(rd,n);
    float diff = max(0.,dot(n,ld)),
         amb = dot(n,ld)*.45+.55,
         spec = pow(max(0.,dot(r,ld)),40.),
         fres = pow(abs(.7+dot(rd,n)),3.),   
         ao = calcAO(p, n);
    // ligthing     
    col = col*mix(1.2*vec3(.25,.08,.13),vec3(.984,.996,.804), mix(amb,diff,.75)) + 
          spec*sp+fres*mix(col,vec3(1),.7)*.4;
    // kind of sub surface scatering      
    col += sss*vec3(1.,.3,.2);
    // sky light reflected from the ground
    col += max(0.,dot(vec3(0,-1,0), n))*.1*skyColor;
    // ambiant occusion
    col *= mix(ao,1.,.5);
    // fade in distance
    return mix( col, skyColor, smoothstep(30.,100., res.x) );
}


//---------------------------------------------------------------------
//   Calculate normal
// inspired by tdhooper and klems - a way to prevent the compiler from inlining map() 4 times
//---------------------------------------------------------------------
vec3 normal(in vec3 pos, vec3 rd, float t ) {
    vec3 n = vec3(0);
    for( int i=ZERO; i<4; i++) {
        vec3 e = .5773*(2.*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.);
        n += e*map(pos+.002*e).x;
    }
	return normalize(n - max(0., dot(n,rd ))*rd);
}

//---------------------------------------------------------------------
//   Camera
//---------------------------------------------------------------------

mat3 setCamera( in vec3 ro, in vec3 ta, in float r) {
	vec3 w = normalize(ta-ro),
         p = vec3(sin(r), cos(r),0.),
         u = normalize( cross(w,p) ),
         v = normalize( cross(u,w) );
    return mat3( u, v, w );
}

//---------------------------------------------------------------------
//   Entry point
//---------------------------------------------------------------------
//#define iTime (iTime + 250.) // Direct to big Bone
void main() {

    // Teapot body profil (8 quadratic curves) 
	A[0]=vec2(0,0);A[1]=vec2(.64,0);A[2]=vec2(.64,.03);A[3]=vec2(.8,.12);A[4]=vec2(.8,.3);A[5]=vec2(.8,.48);A[6]=vec2(.64,.9);A[7]=vec2(.6,.93);
    A[8]=vec2(.56,.9);A[9]=vec2(.56,.96);A[10]=vec2(.12,1.02);A[11]=vec2(0,1.05);A[12]=vec2(.16,1.14);A[13]=vec2(.2,1.2);A[14]=vec2(0,1.2);
	// Teapot spout (2 quadratic curves)
	T1[0]=vec2(1.16, .96);T1[1]=vec2(1.04, .9);T1[2]=vec2(1,.72);T1[3]=vec2(.92, .48);T1[4]=vec2(.72, .42);
	// Teapot handle (2 quadratic curves)
	T2[0]=vec2(-.6, .78);T2[1]=vec2(-1.16, .84);T2[2]=vec2(-1.16,.63);T2[3]=vec2(-1.2, .42);;T2[4]=vec2(-.72, .24);

    gTime = iTime*6.;
   
    // Animation
    int it = int(floor(gTime));
    float kt = fract(gTime), dz = 1.;
   
    
    float a = -1.5708*.4;
    rot = mat2(cos(a), sin(a), -sin(a), cos(a));
     
    a = .2*cos(.4*iTime) + .3*cos(.05*iTime);
    rot1 = mat2(cos(a), sin(a), -sin(a), cos(a));
     
    a = .5*cos(.5*3.141592*gTime);
    a = a*a;
    rot2 = mat2(cos(a), sin(a), -sin(a), cos(a));
    
// ------------------------------------
 
    // Screen 
    vec2 q = xlv_TEXCOORD0.xy/iResolution.xy, 
         m = iMouse.xy/iResolution.y - .5,
         p = -1.+2.*q;
    p.x *= iResolution.x/iResolution.y;        
      
    // Camera	
    vec3 ro = vec3(  hip1.x+12.*cos(PI*(.05*iTime+m.x)),
                4.5+2.*(sin(.1*iTime))+4.*(m.y+.3),
                hip1.z+12.*sin(PI*(.05*iTime+m.x)));
    vec3 ta = hip1;
    ta.x +=1.2;
    ta.y = 1.2;
    mat3 ca = setCamera(ro, ta, 0.);
    vec3 rd = ca * normalize(vec3(p.xy,4.5) );
  
    // Ray intersection with scene
    vec2 res = Trace(ro, rd);
    if (rd.y >= 0.) {
       res = min2(res, vec2(999.,100.));
    } else {        
       res = min2(res, vec2(-ro.y / rd.y,GROUND));
    }
    
    // Rendering
    vec3 pos = ro + rd*res.x;
    vec3 n = pos.y<.02 ? vec3(0,1,0) : normal(pos, rd, res.x);
    vec3 col = doColor(pos, rd, n, res);
    col = pow( col, vec3(.4545) );                 // Gamma    
    col *= pow(16.*q.x*q.y*(1.-q.x)*(1.-q.y), .1); // Vigneting
     
	gl_FragColor = vec4(col,1);
}

