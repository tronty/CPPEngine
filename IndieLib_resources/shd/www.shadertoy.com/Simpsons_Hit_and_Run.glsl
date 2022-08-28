// SDFs and soft shadows by iq (Iñigo Quilez)

// set AA to 0 if the shader is running too slow
#define AA 2
#define ZERO (min(iFrame,0))
#define PI 3.14159

vec2 vmin(vec2 a, vec2 b){ // regular min() compares all components, I don't want to compare material IDs
    return a.x < b.x ? a : b;
}

vec2 smin( vec2 a, vec2 b, float k ){
    float h = max( k-abs(a.x-b.x), 0.0 )/k;
    return vmin( a, b ) - h*h*k*(1.0/4.0);
}

float smin( float a, float b, float k ){
    float h = max( k-abs(a-b), 0.0 )/k;
    return min( a, b ) - h*h*k*(1.0/4.0);
}

float sdSphere( vec3 p, float s ){
  return length(p)-s;
}

float sdCapsule( vec3 p, vec3 a, vec3 b, float r ){
  vec3 pa = p - a, ba = b - a;
  float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
  return length( pa - ba*h ) - r;
}

float sdBox( vec3 p, vec3 b ){
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdCutSphere( vec3 p, float r, float h ){
  // sampling independent computations (only depend on shape)
  float w = sqrt(r*r-h*h);

  // sampling dependant computations
  vec2 q = vec2( length(p.xz), p.y );
  float s = max( (h-r)*q.x*q.x+w*w*(h+r-2.0*q.y), h*q.x-w*q.y );
  return (s<0.0) ? length(q)-r :
         (q.x<w) ? h - q.y     :
                   length(q-vec2(w,h));
}

float sdTorus( vec3 p, vec2 t ){
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float sdEllipsoid( vec3 p, vec3 r ){
  float k0 = length(p/r);
  float k1 = length(p/(r*r));
  return k0*(k0-1.0)/k1;
}

float sdBox2D( in vec2 p, in vec2 b ){
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float sdCircle( vec2 p, float r ){
    return length(p) - r;
}

float dot2( in vec2 v ) { return dot(v,v); }

float sdTrapezoid( in vec2 p, in float r1, float r2, float he ){
    vec2 k1 = vec2(r2,he);
    vec2 k2 = vec2(r2-r1,2.0*he);
    p.x = abs(p.x);
    vec2 ca = vec2(p.x-min(p.x,(p.y<0.0)?r1:r2), abs(p.y)-he);
    vec2 cb = p - k1 + k2*clamp( dot(k1-p,k2)/dot2(k2), 0.0, 1.0 );
    float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
    return s*sqrt( min(dot2(ca),dot2(cb)) );
}

float opExtrussion( in vec3 p, in float sdf, in float h ){
    vec2 w = vec2( sdf, abs(p.z) - h );
  	return min(max(w.x,w.y),0.0) + length(max(w,0.0));
}
vec2 map(in vec3 pos, in float time){ // signed distance in m.x and material ID in m.y
    vec2 wq = pos.xz + vec2(.6, .3);
    wq.y = mod(wq.y, 1.)-.2;
    float wm = sdBox2D(wq, vec2(.4)); // walkway (and driveway) mask
    wq = pos.xz-vec2(0., -3.);
    wq.x = mod(wq.x+.1, 1.)-.5;
    wm = min(wm, sdBox2D(wq, vec2(.4)));
    wm += (pos.x > -1.1) && (pos.z < -2.5) && (pos.x < 7.) ? 0. : 1.; // cut walkway
    wm = min(wm, sdBox2D(pos.xz-vec2(6.5, -2.1), vec2(1.5, 3.)));
    wm = smoothstep(0.0, 0.1, wm);
    vec2 m = vec2(pos.y+1.4+wm*.1, 8.0); // floor
    
    // Homer
    vec2 hm;
    float tmp = .02;
    pos.y += tmp;
    vec3 q = pos - vec3(0.5, 0., -.5); // y .5
    hm = vec2(sdCapsule(q, vec3(0.), vec3(0.01, 0.2, -0.01), 0.15), 1.0); // head
    float HmBV = sdCapsule(pos, vec3(.5, .7, -.5), vec3(.5, -.7, -.5), .31);
    if(HmBV < 1.){
        hm.x = smin(hm.x, sdCapsule(q, vec3(.02, 0., -.02), vec3(.04, -.2, -.04), .1), .02); // neck
        hm.x = smin(hm.x, sdCapsule(q, vec3(0., .1, 0.), vec3(-.07, .1, .15), .03), .02); // nose
        
        // upper and lower jaw are separate, different sized spheres
        
        // smile test
        vec3 w = q;
        float temp = .3;
        w.z -= temp;
        w.y -= w.z*w.z*.4;
        w.z += temp;
        
        //w = q; // uncomment to remove smile test
        
        hm = vmin(hm, vec2(sdCutSphere(q-vec3(-.02, 0., .02), .15, -.05), 3.0)); // stubble (without max op)
        hm = smin(hm, vec2(sdTorus(w-vec3(-.02, -.05, .02), vec2(.12, .03)), 3.0), .02); // lip
        hm = vmin(hm, vec2(sdSphere(q-vec3(-.01, -.03, .02), .13), 3.0)); // lower jaw
        
        q.xz = mat2(24, -7, 7, 24)/25.*q.xz;
        vec3 r = vec3(abs(q.x)-.07, q.y, q.z);
        hm = smin(hm, vec2(sdTorus(r-vec3(0., .2, 0.05), vec2(.04, .03)), 1.0), .02); // eyebrows
        
        r.xy = mat2(63, 16, -16, 63)/65.*r.xy;
        hm = smin(hm, vec2(sdCutSphere(r-vec3(-.035, .17, .1), .06, .04), 1.0), .05); // eyelids
        
        hm = smin(hm, vec2(sdSphere(vec3(abs(q.x)-.07, q.y-.15, q.z-.1), .06), 2.0), .015); // sclera
        hm = vmin(hm, vec2(sdSphere(vec3(abs(q.x)-.07, q.y-.15, q.z-.155), .01), 6.0)); // pupils
        hm = smin(hm, vec2(sdSphere(q-vec3(.12, .05, 0.), .05), 1.0), .02); // ear
        
        // hair
        float hair = sdTorus(pos.yzx-vec3(.315, -.46, .52), vec2(.05, .003));
        hair = min(hair, sdTorus(pos.yzx-vec3(.31, -.48, .56), vec2(.05, .003)));
        hair = min(hair, sdCapsule(pos, vec3(.62, .1, -.4), vec3(.66, .15, -.46), .003));
        hair = min(hair, sdCapsule(pos, vec3(.66, .15, -.46), vec3(.68, .13, -.55), .003));
        hair = min(hair, sdCapsule(pos, vec3(.68, .13, -.55), vec3(.7, .16, -.58), .003));
        hm = vmin(hm, vec2(hair, 6.0));
        
        q.y -= tmp;
        
        // body
        q.y -= .02;
        float bdy = sdSphere(q-vec3(0.03, -.4, -.03), .16); // chest
        bdy = smin(bdy, sdCutSphere(q-vec3(0., -.7, 0.), .3, -.1), .2); // belly
        bdy = smin(bdy, sdTorus(q-vec3(.03, -.28, -.03), vec2(.15, .03)), .05); // collar
        // arms
        r.xy = mat2(63, -16, 16, 63)/65.*vec2(abs(q.x)-.1, q.y);
        bdy = smin(bdy, sdCutSphere(r-vec3(.0, -.48, 0.), .13, -.03), .01); // shoulders
        hm = vmin(hm, vec2(bdy, 2.0));
        bdy = sdCapsule(r, vec3(.03, -.45, .0), vec3(.05, -.6, .03), .072); // upper arms
        bdy = smin(bdy, sdCapsule(r, vec3(.05, -.68, .03), vec3(.02, -.85, .08), .065), .05); // forearms
        hm = vmin(hm, vec2(bdy, 1.0));
        // legs
        bdy = sdSphere(q+vec3(0., .7, 0.), .28);
        bdy = smin(bdy, sdCapsule(q, vec3(.09, -.8, .0), vec3(.12, -1.2, .0), .18*(1.8+q.y)), .005); // up left
        bdy = smin(bdy, sdCapsule(q, vec3(-.09, -.8, .0), vec3(-.14, -1.2, -.05), .18*(1.8+q.y)), .005); // up r
        bdy = smin(bdy, sdCapsule(q, vec3(-.14, -1.225, -.05), vec3(-.12, -1.45, -.08), .1), .01); // down r
        bdy = smin(bdy, sdCapsule(q, vec3(.12, -1.225, .0), vec3(.12, -1.45, -.05), .1), .01); // down l
        hm = vmin(hm, vec2(bdy, 7.0));
        // shoes
        bdy = sdSphere(q-vec3(.12, -1.5, -.05), .12); // left
        bdy = min(bdy, sdSphere(q-vec3(-.12, -1.5, -.08), .12)); // right
        
        bdy = smin(bdy, sdSphere(q-vec3(-.23, -1.5, .05), .08), .12); // right front
        bdy = smin(bdy, sdSphere(q-vec3(.18, -1.5, .1), .08), .12); // left front
        hm = vmin(hm, vec2(bdy, 10.0));
        
    }
    m = vmin(m, hm);
    
    // house
    vec2 hs;
    float HsBV = sdBox(pos-vec3(1.5, 0., -8.), vec3(5., 2.5, .5)); // house bounding volume
    q = pos-vec3(-0.5, 0., -7.);
    hs = vec2(sdBox(q-vec3(0., 1., 0.), vec3(5., 2.5, .5)), 4.0);
    if(HsBV < 5.2){
        hs.x = min(hs.x, sdBox(q-vec3(7.2, -.4, -2.), vec3(2.5, 3., 2.)));
        hs.x = min(hs.x, sdBox(q-vec3(7., 0., -.5), vec3(2., 1.5, 2.5)));
        hs.x = min(hs.x, sdBox(q+vec3(5.2, 0., 1.5), vec3(.25, 5.5, .5))); // chimney
        hs.x = min(hs.x, sdBox(q+vec3(5.2, -5.5, 1.5), vec3(.35, .15, .6))); // chimney border
        // roof
        vec3 r = q-vec3(0., 3.9, -2.2);
        r.z = abs(r.z)-2.;
        r.yz = mat2(35, -12, 12, 35)/37.*r.yz;
        float rf = sdBox(r, vec3(5.5, .1, 2.1));
        rf = min(rf, sdBox(r-vec3(7.5, -1.1, -1.5), vec3(2.5, .1, 2.5)));
        // garage roof
        r = q;
        r.x -= 7.;
        r.x = abs(r.x)-2.;
        r.xz = mat2(-1, 0, 0, -1)*r.xz; // rotate garage roof
        r.xy = mat2(35, -12, 12, 35)/37.*r.xy; // break it in half
        rf = min(rf, sdBox(r-vec3(9.5, 1.5, -.5), vec3(9.5, .1, 2.)));
        hs = vmin(hs, vec2(rf, 5.0));
        
        // add triangular part of garage
        hs.x = min(hs.x, max(sdBox(pos-vec3(6.5, 0., -7.), vec3(2., 2.5, 2.)), r.y-1.5));
        
        float disp = fract(-pos.y*2.5+.54)*.03; // garage door corrugation
        hs = vmin(hs, vec2(sdBox(pos-vec3(6.5, -.4, -5.18+disp), vec3(1.3, 1., .2)), 9.0)); // garage door
        
        HsBV = sdBox(pos+vec3(.5, .2, 5.5), vec3(2.3, .8, .8));
        if(HsBV < 2.65){ // smaller bounding volume for the details of the front of the house
        // could be 1.5 without upper windows, maybe I should separate them -- there's a ~5 fps difference
            // windows
            q = vec3(abs(pos.x+.5)-3., pos.y-.8, pos.z+6.8);
            float rw = pos.y*.5-.32;
            vec2 wi;
            wi.x = opExtrussion(q.xzy, sdTrapezoid(q.xz-vec2(0., .5), .9-rw*2., .5-rw, .4-rw), .2); // upper
                // lower border
            wi.x = min(wi.x,
            opExtrussion(q.xzy+vec3(0., 0., 1.5), sdTrapezoid(q.xz-vec2(0., .5), .9, .5, .4), .1));
            wi.x = min(wi.x, sdBox(q-vec3(-.1, 1.2, 0.), vec3(1.1, .7, .32)));
            wi.y = 5.0;
                // middle things
            wi.x = min(wi.x,
            opExtrussion(vec3(q.x, q.z, abs(q.y+.8)-.8)+vec3(0., 0., 0.5),
            sdTrapezoid(q.xz-vec2(0., .5), .82, .42, .32), .05));
            wi.x = min(wi.x, sdBox(vec3(q.x, q.y+.7, q.z), vec3(.05, .7, .82)));
            wi.x = min(wi.x, sdBox(q-vec3(0., -.8, 0.), vec3(.8, .65, .33)));
            wi.x = min(wi.x, sdBox(q-vec3(0., -.8, .2), vec3(.62, .65, .33)));
            wi.x = min(wi.x, sdBox(q-vec3(0., -.8, .465), vec3(.45, .65, .33)));
            hs = vmin(hs, wi);
                // glass
            wi.y = 15.0;
            wi.x = opExtrussion(q.xzy+vec3(0., 0., .8), sdTrapezoid(q.xz-vec2(0., .5), .8, .4, .3), .7);

            // door
            float dr = sdCircle(pos.xy-vec2(-.5, 0.), .5);
            dr = min(dr, sdBox2D(pos.xy-vec2(-.5, -.75), vec2(.5, .7)));
            float drs = dr;
            dr = min(dr, sdBox2D(q.xy-vec2(-.1, 1.2), vec2(.9, .5))); // also cut hole for upper windows
            hs.x += smoothstep(0.05, .0, dr)*.2; // indent wall
            hs.x -= smoothstep(0.05, .0, drs-.2)*.05;
            
            wi.x = min(wi.x, sdBox(q-vec3(-.1, 1.2, 0.), vec3(1., .6, .31))); // upper windows
            hs = vmin(hs, wi);
            // upper windows middle things
            wi.x = sdBox(q-vec3(-.1, 1.2, 0.), vec3(.05, .6, .32));
            wi.x = min(wi.x, sdBox(q-vec3(-.1, 1.2, 0.), vec3(.95, .05, .32)));
            hs = vmin(hs, vec2(wi.x, 5.0));
            
            dr = sdSphere(pos-vec3(-.85, -.5, -6.575), .08); // knob
            dr = min(dr, sdBox(pos-vec3(-.8, -.55, -6.775), vec3(.1, .2, .2))); // lock thing
            hs = vmin(hs, vec2(dr, 11.0));
            hs = vmin(hs, vec2(sdBox(pos-vec3(-.5, -.5, -7.1), vec3(.6, 1.2, .5)), 12.0)); // door

            // plants
            q = vec3(abs(pos.x+.6), pos.yz);
            vec3 rb = .5*vec3(sin(pos.y*23.51), cos(pos.z*18.11), sin(pos.x*25.168))*.05;
            hs = vmin(hs, vec2(sdBox(q-vec3(1., -1.5, -5.5), vec3(.4, .2, 1.2))-.06, 13.0));
            hs = vmin(hs, vec2(sdEllipsoid(q-vec3(1., -1., -5.5), rb+vec3(.5, .45, 1.3)), 14.0));
        }
    }
    
    // fence
    vec3 r = pos;
    r.x = abs(r.x-1.5)-7.5;
    r.z = mod(r.z, .5);
    float fc = sdBox(r-vec3(0., -1., .25), vec3(.015, 1.2, .2)); // vertical planks
    fc = min(fc, sdBox2D(vec2(abs(pos.x-1.5)-7.5, abs(pos.y+.6)-.6)-vec2(0.), vec2(.05))); // bars
    hs = vmin(hs, vec2(fc, 5.0));
    
    m = vmin(m, hs);
    
    return m;
}

vec2 castRay( in vec3 ro, in vec3 rd, float time ){
    vec2 res = vec2(-1.0, -1.0);
    float tmin = 3.;
    float tmax = 34.;
    
    float t = tmin;
    for(int i=ZERO; i<256 && t<tmax; i++){
        vec2 h = map( ro+rd*t, time );
        //if( h.x < 0.001 ){
        //if( abs(h.x)<(0.001*t) ){
        if( abs(h.x)<0.001 ){ // cone tracing (*t) added shadow artifacts
            res = vec2(t, h.y);
            break;
        }
        t += h.x;
    }
    return res;
}

// change between 0 and 1 to test different normal calculation functions
#if 1
vec3 calcNormal(vec3 q, float time){ // gscene3 from https://www.shadertoy.com/view/NscXR4
    float h = 0.001;
	vec3 n = vec3(0);
	for (int i = ZERO; i < 4; ++i) {
		//vec3 e = vec3((ivec3(i+3, i, i+i)&2) - 1);
		vec3 e = vec3(i+3, i, i+i);//&2) - 1);
		n += map(q + e * h, time).x * e;
	}
	return normalize(n);
}
#else
vec3 calcNormal(vec3 p, float time){
  vec2 e = vec2(1.0, -1.0) * 0.0005;
  return normalize(
    e.xyy * map(p + e.xyy, time).x +
    e.yyx * map(p + e.yyx, time).x +
    e.yxy * map(p + e.yxy, time).x +
    e.xxx * map(p + e.xxx, time).x);
}
#endif

float softShadow(vec3 ro, vec3 rd, float mint, float maxt, float time){
// source: https://www.shadertoy.com/view/ltyXD3
    float res = 1.0;
    float t = mint;
    
    for(int i = ZERO; i < 16; i++) {
        float h = map(ro + rd * t, time).x;
        res = min(res, 8.0*h/t);
        t += clamp(h, 0.02, 0.10);
        if(h < 0.001 || t > maxt) break;
    }
    
    return clamp( res, 0.0, 1.0 );
}

vec3 render( in vec3 ro, in vec3 rd, float time ){
    vec3 col = vec3(0.5, 0.8, 0.9) - max(rd.y,0.0)*0.75;
    vec2 res = castRay(ro, rd, time);
    vec3 pos = ro+res.x*rd;
    vec3 n = calcNormal(pos, time);
    float spe = 0.;
    
    vec3 sunDir = normalize(vec3(0.2, 0.3, 0.2));
    vec3 sunCol = vec3(1.2, 1.1, 0.8);
    vec3 grassCol = vec3(0.05, 0.25, 0.1);
    
    col = pos.y < -.2 ? col : vec3(.05, .25, .05)*dot(sunDir, vec3(0., 1., 0.))*sunCol
    +.02+vec3(0.2, 0.2, 0.9)*.055; // fill grass in
    
    float sunDif = clamp(dot(n, sunDir), 0.0, 1.0);
    if(res.y > 0.5){ // not sky
        col = vec3(1., 0.851, 0.059); // skin
        if((1.5 < res.y) && (res.y < 2.5)){ // Homer's shirt
            col = vec3(1.);
        }else if((2.5 < res.y) && (res.y < 3.5)){ // stubble
            col = vec3(0.973, 0.863, 0.604);
        }else if((3.5 < res.y) && (res.y < 4.5)){ // house walls
            // wall : doorframe
            col = (pos.z < -6.49) || (pos.x > 2.) ? vec3(.95, .55, .55) : vec3(0.4, 0.3, 0.2);
            col *= clamp((pos.y+1.8), 0., 1.);
        }else if((4.5 < res.y) && (res.y < 5.5)){ // roof
            col = vec3(0.4, 0.3, 0.2);
        }else if((5.5 < res.y) && (res.y < 6.5)){ // pupils and hair
            col = vec3(.02);
        }else if((6.5 < res.y) && (res.y < 7.5)){ // pants
            col = vec3(.439, .82, .996);
        }else if((7.5 < res.y) && (res.y < 8.5)){ // floor
            col = pos.y > -1.46 ? vec3(.5, .5, .35) : vec3(.05, .25, .05); // sidewalk : grass
        }else if((8.5 < res.y) && (res.y < 9.5)){ // garage door
            col = vec3(.6, .6, .5);
            col *= fract(pos.y*2.5-.5)+.5; // fake corrugation
        }else if((9.5 < res.y) && (res.y < 10.5)){ // shoes
            col = vec3(.2, .2, .25);
        }else if((10.5 < res.y) && (res.y < 11.5)){ // doorknob
            col = vec3(0.4);
        }else if((11.5 < res.y) && (res.y < 12.5)){ // door
            col = vec3(.75, .3, .3);
        }else if((12.5 < res.y) && (res.y < 13.5)){ // planters
            col = vec3(.4, .1, .05);
        }else if((13.5 < res.y) && (res.y < 14.5)){ // door plants
            col = vec3(.05, .5, .1) + .05*sin(pos*vec3(15.654, 32.13, 25.8));
        }else if((14.5 < res.y) && (res.y < 15.5)){ // glass
            col = vec3(.2, .4, .85);
            float RV = clamp(dot(reflect(sunDir, n), rd), 0., 1.);
            spe = pow(RV, 5.)*10.;
        }
        
        float topOcc = softShadow(pos, vec3(0., 1., 0.), 0.02, .5, time)+0.02;
        vec3 lin = vec3(0.0);
        lin += sunDif*sunCol*softShadow(pos, sunDir, 0.02, 1.5, time); // Sun
        lin += spe;
        lin += .02+(0.05+0.95*clamp(n.y, 0.0, 1.0))*vec3(0.2, 0.2, 0.9)*topOcc; // sky
        
        lin += (0.05+0.95*clamp(-n.y, 0.0, 1.0))*grassCol * 0.5;
        col = col*lin;
    }
    return col;
}

void main( ){
    vec3 tot = vec3(0.);
    float time = iTime;
#if AA > 1
    for( int m=ZERO; m<AA; m++ )
    for( int n=ZERO; n<AA; n++ )
    {
    vec2 o = vec2(float(m),float(n)) / float(AA) - 0.5;
    vec2 uv = ((xlv_TEXCOORD0+o) - .5 * iResolution.xy) / iResolution.y;
#else
    vec2 uv = (xlv_TEXCOORD0 - .5 * iResolution.xy) / iResolution.y;
#endif
    vec3 ro = vec3(0., -.2, 3.);
    vec3 rd = normalize(vec3(uv, -1.));
    rd.yz = mat2(264, 23, -23, 264)/265.*rd.yz;
    rd.xz = mat2(84, 13, -13, 84)/85.*rd.xz;
    
    // top-down view
#if 0
    ro = vec3(10., 15., 15);
    rd.yz = mat2(272, -225, 225, 272)/353.*rd.yz;
    rd.xz = mat2(272, -225, 225, 272)/353.*rd.xz;
#endif
    
    if(iMouse.z > 0.){ // if LMB is being pressed rotate camera
        vec2 mouse = (iMouse.xy - .5 * iResolution.xy) / iResolution.y;
        float cx, cy, sx, sy;
        cx = cos(mouse.x); cy = cos(mouse.y); sx = sin(mouse.x); sy = sin(mouse.y);
        rd.yz = mat2(cy, -sy, sy, cy)*rd.yz;
        rd.xz = mat2(cx, -sx, sx, cx)*rd.xz;
    }
    
    vec3 col = render(ro, rd, time);
    col = pow(col, vec3(0.4545)); // gamma correction
    tot += col;
#if AA > 1
    }
    tot /= float(AA*AA);
#endif
    
    gl_FragColor = vec4(tot, 1.);
}
