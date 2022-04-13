uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
/*

    Packed Rectangle Spiral Tunnel
    ------------------------------
    
    There are two ways to code a packed rectangle texture2D. There is a relatively 
    simple way involving lines on a backbuffer, and an excrutiatingly annoying way 
    that involves rendering each rectangle's distance field. The line version, which 
    you can find a couple of examples of on Shadertoy looks pretty cool in its own 
    right. However, it's not useful for a lot of the things that I'd imagine people 
    would want them for, like individual rectangle coloring, height maps, bump 
    mapping, etc.

	I wouldn't say randomly packed rectangle textures are easy to code outside a 
    pixelshader environment, but it's a lot easier when you're not restricted by pixel 
    access order, that's for sure. There's also wrapping, linear interpolation, screen 
    size changes, etc, to consider, so you're probably wondering why anyone would bother.
    
    The simple answer is, they look cool, plus they're a pretty useful pattern to have
    in your toolkit. This scene doesn't quite do the pattern justice, but gives the
    general idea. I'm also hoping it demonstrates that you're not restricted to in-house 
    textures, or distance patterns that you can code in realtime.
    
    The packed rectangle texture2D constuction is all you'll need to understand this, and
    that's contained in the "Cube A" tab. To see the standalone texture2D, uncomment the
    DISPLACEMENT_MAP define. Everything else involves simple, but lengthy, applications 
    of well known techniques, like bump mapping, coloring, lighting, etc.
     
    Anyway, this is yet another proof of concept, just to show that it's possible. I 
    have a couple of more interesting examples involving this. There are a few defines
    to play around with in the "Common" and "Image" tabs for anyone interested. 
     



	Related exmaples:
    
	// Fabrice's worm textures are great if you haven't seen them.
    // This is one of my favorite ones.
    maze worms / graffitis 4  - FabriceNeyret2 
    https://www.shadertoy.com/view/lsjyzw
    
    // A cheap trick I came up with ages ago to emulated
    // random packed rectangle textrues.
    Asymmetric Blocks - Shane
    https://www.shadertoy.com/view/Ws3GRs
    
    // Another packed texture2D based on a common subdivision
    // scheme. This one has rectangle information.
    Box Divide ID - Shane
    https://www.shadertoy.com/view/WlsSRs


*/
 

// Just the displacement map on its own. Actually, this is a bump mapped
// version.
//#define DISPLACEMENT_MAP

// Color scheme - SPECTRUM PASTEL: 0, BROWN: 1, PURPLE: 2.
#define COLOR 0

// Put a bit of specular sheen on things. I've also raised the tiles
// to show that there is a distance field involved.
#define SHINY

// Border demarcation scheme.
// 0: No edges, 1: Side rails, 2: Squares, 3: Rails and squares. 
#define EDGES 1


// Max ray distance.
#define FAR 7.

// No forced unroll.
#define ZERO min(0, iFrame)



// Scene object ID to separate the mesh object from the terrain.
float objID;


// Tri-Planar blending function. Based on an old Nvidia tutorial by Ryan Geiss.
vec3 tex3D(sampler2D t, in vec3 p, in vec3 n){ 
    
    n = max(abs(n) - .2, .001); // max(abs(n), 0.001), etc.
    //n /= dot(n, vec3(.8)); 
    n /= length(n);
    
    // Texure samples. One for each plane.
    vec3 tx = texture2D(t, p.yz).xyz;
    vec3 ty = texture2D(t, p.zx).xyz;
    vec3 tz = texture2D(t, p.xy).xyz;
    
    // Multiply each texture2D plane by its normal dominance factor.... or however you wish
    // to describe it. For instance, if the normal faces up or down, the "ty" texture2D sample,
    // represnting the XZ plane, will be used, which makes sense.
    
    // Textures are stored in sRGB (I think), so you have to convert them to linear space 
    // (squaring is a rough approximation) prior to working with them... or something like that. :)
    // Once the final color value is gamma corrected, you should see correct looking colors.
    return mat3(tx*tx, ty*ty, tz*tz)*n; // Equivalent to: tx*tx*n.x + ty*ty*n.y + tz*tz*n.z;

}

// Smooth cube map face 2D texture2D blend.
float getTex1(samplerCube iCh, vec3 q){
    
    // Scaling.
    vec2 p = q.xy/1.;
    
    // Cube map texture2D coordinate conversion.
    p *= cubemapRes;
    vec2 ip = floor(p); p -= ip;
    vec2 uv = fract((ip + .5)/cubemapRes) - .5;
    
    // 2D neighboring texels stored in each of the four texture2D channels.
    vec4 p4 = texture2D(iCh, vec3(.5, uv.y, -uv.x)); 
    
    // Linearly interpolate.
    return mix(mix(p4.x, p4.y, p.x), mix(p4.z, p4.w, p.x), p.y);

}

vec2 getUV(vec3 q){

    // Scaling.
    vec2 p = q.xy/1.;
    
    // Cube map texture2D coordinate conversion.
    p *= cubemapRes;
    vec2 ip = floor(p); //p -= ip;
    vec2 uv = ((ip + .5)/cubemapRes);
    
    return uv;
 
}


float getDF(vec2 uv, vec4 rect){
    
    return max(abs(rect.x) - rect.z/2., abs(rect.y) - rect.w/2.);
} 

/* 
// Smooth cube map face 2D texture2D blend.
vec3 hm(vec3 q){
    
    // Smooth distance field.
    float df = getTex1(iChannel2, q);
    
    // Rectangle center and dimensions.
    vec2 uv = getUV(q);
    vec4 tx = texture2D(iChannel2, vec3(-.5, fract(uv.yx) - .5)); 

    // Cube map wrapping.   
    tx.zw = convert(tx.zw);
    // Random color, used for the height.
    float rnd = hash21(tx.xy + .5);
    vec3 col = .5 + .45*cos(6.2831*rnd/1. + vec3(0, 1., 2));
    
    // Applying height information to the 2D distance field.
    float sf = 16./1024.;
    float rDf = df + sf/2.;
    col = mix(vec3(0), col - rDf*4.*1., 1. - smoothstep(0., sf, rDf));
    
    return col;
}
*/

mat2 twist(float y){
   return rot2(y*.8);
}

// The extruded image.
float map(vec3 p){

    
    // Twisting the scene along the Y direction.
    p.xz *= twist(p.y);
    
    // The tunnel center... It's not offset, so this is kind of
    // redundant, but it's usually helpful.
    vec3 cntr = p;
    
    const float ew = 1./14.;
    float sq = max(abs(cntr.x), abs(cntr.z)) - .5;// + 1.;
    
    // Inside of the square tube for the tunnel.
    float tun = -sq;
    
    
    #if EDGES > 0
        // Divider.
        float div = max(abs(sq + ew/2. + ew/8.) - (ew/2.), abs(mod(p.y, 1.) - 1./2.) - ew/2.);

        // Moving the rails to the corners, then moving them in slightly.
        vec2 q = abs(abs(p.xz) - .5 + ew/2. + ew/8.);
        float rails = max(q.x, q.y) - (ew/2. - .006); 
        #if EDGES == 1
        div = rails;//min(div, rails);
        #elif EDGES >= 3
        div = min(div, rails);
        #endif
    #else
        float div = 1e5;
    #endif
    
    // Doubling up.
    //div = abs(div) - ew;
    
    // Wall UV direction. 
    vec3 uvw = abs(cntr.x)>abs(cntr.z)*1.? vec3(-1, 1, 1)*cntr.zyx : cntr;
    
    /*
    // Distance field height application.
    vec3 col = hm(uvw);
    float val = dot(col, vec3(.299, .587, .114));
    tun -= val*.02;
    */
 
    // Encasing boxes inside the rails: I hacked this together in a hurry, but
    // it seems to work. It definitely needs a tidy up.
    const float sc = 14.;
    float dOffs = ew/8.*2./sc;
    vec3 offs = vec3(0, .5/sc, 0); // The edge XZ has been moved in by .25/sc.
    p = abs(mod(p + offs, 1./sc - vec3(dOffs, 0, dOffs)) - (1./sc - vec3(dOffs, 0, dOffs))/2.);
    float cb = max(max(p.x, p.y), p.z) - .5/sc + .006;
    div = min(div + .015, max(div, cb));
 
    // Overall object ID. Either the tunnel or the repeat boxes.
    objID = tun<div? 0. : 1.;
    
    // Just the floor.
    return min(tun, div);
 
}

 

 
// Basic raymarcher.
float trace(in vec3 ro, in vec3 rd){

    // Overall ray distance and scene distance.
    float t = 0., d;//max(.5-ro.z/rd.z, 0.)
    
    for(int i = min(iFrame, 0); i<128; i++){
    
        d = map(ro + rd*t);
        // Note the "t*b + a" addition. Basically, we're putting less emphasis on accuracy, as
        // "t" increases. It's a cheap trick that works in most situations... Not all, though.
        if(abs(d)<.001 || t>FAR) break; // Alternative: 0.001*max(t*.25, 1.), etc.
        
        //t += i<32? d*.75 : d; 
        t += d*.75; 
    }

    return min(t, FAR);
}


// Standard normal function. It's not as fast as the tetrahedral calculation, but more symmetrical.
vec3 getNormal(in vec3 p) {
	
    const vec2 e = vec2(.001, 0);
    
    //vec3 n = normalize(vec3(map(p + e.xyy) - map(p - e.xyy),
    //map(p + e.yxy) - map(p - e.yxy),	map(p + e.yyx) - map(p - e.yyx)));
    
    // This mess is an attempt to speed up compiler time by contriving a break... It's 
    // based on a suggestion by IQ. I think it works, but I really couldn't say for sure.
    float sgn = 1.;
    float mp[6];
    vec3[3] e6 = vec3[3](e.xyy, e.yxy, e.yyx);
    for(int i = ZERO; i<6; i++){
		mp[i] = map(p + sgn*e6[i/2]);
        sgn = -sgn;
        if(sgn>2.) break; // Fake conditional break;
    }
    
    return normalize(vec3(mp[0] - mp[1], mp[2] - mp[3], mp[4] - mp[5]));
}



// Cheap shadows are hard. In fact, I'd almost say, shadowing particular scenes with limited 
// iterations is impossible... However, I'd be very grateful if someone could prove me wrong. :)
float softShadow(vec3 ro, vec3 lp, vec3 n, float k){

    // More would be nicer. More is always nicer, but not really affordable... Not on my slow test machine, anyway.
    const int maxIterationsShad = 32; 
    
    ro += n*.0015;
    vec3 rd = lp - ro; // Unnormalized direction ray.
    

    float shade = 1.;
    float t = 0.;//.0015; // Coincides with the hit condition in the "trace" function.  
    float end = max(length(rd), 0.0001);
    //float stepDist = end/float(maxIterationsShad);
    rd /= end;

    // Max shadow iterations - More iterations make nicer shadows, but slow things down. Obviously, the lowest 
    // number to give a decent shadow is the best one to choose. 
    for (int i = min(iFrame, 0); i<maxIterationsShad; i++){

        float d = map(ro + rd*t);
        shade = min(shade, k*d/t);
        //shade = min(shade, smoothstep(0., 1., k*h/dist)); // Subtle difference. Thanks to IQ for this tidbit.
        // So many options here, and none are perfect: dist += min(h, .2), dist += clamp(h, .01, stepDist), etc.
        t += clamp(d, .01, .15); 
        
        
        // Early exits from accumulative distance function calls tend to be a good thing.
        if (d<0. || t>end) break; 
    }

    // Sometimes, I'll add a constant to the final shade value, which lightens the shadow a bit --
    // It's a preference thing. Really dark shadows look too brutal to me. Sometimes, I'll add 
    // AO also just for kicks. :)
    return max(shade, 0.); 
}


// I keep a collection of occlusion routines... OK, that sounded really nerdy. :)
// Anyway, I like this one. I'm assuming it's based on IQ's original.
float calcAO(in vec3 p, in vec3 n)
{
	float sca = 3., occ = 0.;
    for( int i = ZERO; i<5; i++ ){
    
        float hr = float(i + 1)*.15/5.;        
        float d = map(p + n*hr);
        occ += (hr - d)*sca;
        sca *= .7;
        //if(d>1e8) break; // Fake break.
    }
    
    return clamp(1. - occ, 0., 1.);  
    
    
}



// A global value to record the distance from the camera to the hit point. It's used to tone
// down the height values that are further away, which can sometimes tone down annoying
// Moire artifacts.
//float gT;


// Surface bump function..
float bumpSurf3D(in vec3 p, in vec3 n){
    
     
    
    vec3 txP = p;
    // The objects have been rotated in the distance function,
    // so the normal needs to match... If I had a dollar for every time
    // I've forgotten that... :)
    vec3 txN = n;
    txP.xz *= twist(txP.y);
    txN.xz *= rot2(txN.y);

    //vec2 uv = abs(txN.x)>.5? txP.zy : abs(txN.y)>.5? txP.xz : txP.xy;
    //float faceID = abs(txN.x)>.5? 0. : abs(txN.y)>.5? 1. : 2.;
    
    vec3 uvw = abs(txP.x)>abs(txP.z)*1.? vec3(-1, 1, 1)*txP.zyx : txP;
    
    float df = getTex1(iChannel2, uvw);
 
    float sf = 16./cubemapRes.x;
   
    #ifdef SHINY
    float maxVal = max(1. - df/.1, 0.);
    #else
    const float maxVal = 1.;
    #endif
    float val = mix(0., maxVal, 1. - smoothstep(0., sf, (df + sf*.5)));//tx.x
    
    return val;///(1. + gT*gT*.5);

}


 
// Standard function-based bump mapping routine: This is the cheaper four tap version. There's
// a six tap version (samples taken from either side of each axis), but this works well enough.
vec3 doBumpMap(in vec3 p, in vec3 n, float bumpfactor){
    
    // Larger sample distances give a less defined bump, but can sometimes lessen the aliasing.
    const vec2 e = vec2(1./cubemapRes.x, 0);  
    
    // This utter mess is to avoid longer compile times. It's kind of 
    // annoying that the compiler can't figure out that it shouldn't
    // unroll loops containing large blocks of code.
    mat4x3 p4 = mat4x3(p, p - e.xyy, p - e.yxy, p - e.yyx);
    
    vec4 b4;
    for(int i = ZERO; i<4; i++){
        b4[i] = bumpSurf3D(p4[i], n);
        if(n.x>1e5) break; // Fake break to trick the compiler.
    }
    
    // Gradient vector: vec3(df/dx, df/dy, df/dz);
    vec3 grad = (b4.yzw - b4.x)/e.x; 
   
    
    // Six tap version, for comparisson. No discernible visual difference, in a lot of cases.
    //vec3 grad = vec3(bumpSurf3D(p - e.xyy) - bumpSurf3D(p + e.xyy),
    //                 bumpSurf3D(p - e.yxy) - bumpSurf3D(p + e.yxy),
    //                 bumpSurf3D(p - e.yyx) - bumpSurf3D(p + e.yyx))/e.x*.5;
    
  
    // Adjusting the tangent vector so that it's perpendicular to the normal. It's some kind 
    // of orthogonal space fix using the Gram-Schmidt process, or something to that effect.
    grad -= n*dot(n, grad);          
         
    // Applying the gradient vector to the normal. Larger bump factors make things more bumpy.
    return normalize(n + grad*bumpfactor);
	
}


// IQ's signed distance to a 2D triangle with a few lines
// taken out to make a wedge... It's a less-than-clever 
// hack, but I'll find a better solution later.
float wedge(in vec2 p, in vec2 p0, in vec2 p1, in vec2 p2){
 
    vec2 e0 = p1 - p0, e1 = p2 - p1, e2 = p0 - p2;

	vec2 v0 = p - p0, v1 = p - p1, v2 = p - p2;

	vec2 pq0 = v0 - e0*clamp( dot(v0, e0)/dot(e0, e0), 0., 1.);
	vec2 pq2 = v2 - e2*clamp( dot(v2, e2)/dot(e2, e2), 0., 1.);
    
    float s = sign( e0.x*e2.y - e0.y*e2.x);
    vec2 d =min( vec2(dot(pq0, pq0), s*(v0.x*e0.y - v0.y*e0.x)),
                       
                       vec2(dot(pq2, pq2), s*(v2.x*e2.y - v2.y*e2.x)));

	return -sqrt(d.x)*sign(d.y);
}

// A quick hacky loading screen routine: I got a bit carried away. For 
// whatever reason, I decided that I wanted something that matched the 
// scene we were leading into... I probably should have stuck to 
// something simpler and more elegant.
vec3 loadingScreen(vec2 uv, vec3 scBgnd){

    // Cube map face two, which contains the smooth information.
    //vec4 ldTex = texture2D(iChannel2, vec3(-.5, uv2.yx));
    //float df = getTex1(iChannel2, vec3((uv), 0)); //ldTex.x>0.? 0. : 1.
    //vec3 bgCol = vec3(df);

    // Grid partitioning.
    vec2 ldSc = vec2(24);
    float sf = ldSc.x/iResolution.y;
    vec2 ldP = uv*ldSc;
    vec2 ldIP = floor(ldP);
    ldP -= ldIP + .5;

    
    // Shape samples.
    float ldSq = max(abs(ldP.x), abs(ldP.y)) - .5;
    //float ldSq = length(ldP) - .48;
    vec2 ld = normalize(vec2(-1, -2));
    float ldSq2 = max(abs(ldP.x - ld.x*.001), abs(ldP.y - ld.y*.001)) - .5;
    //float ldSq2 = length(ldP - ld.x*.001) - .48;
    
    //ldSq = abs(ldSq + .2) - .2;
    //ldSq2 = abs(ldSq2 + .2) - .2;

    // Bumped highlight value.
    float b = max(ldSq2 - ldSq, 0.)/.001;
    
    

    // Load time factor.
    float ldTf = float(iFrame)/float(maxFrames);

    if(abs(ldIP.x + .5)>ldSc.x/2.*iResolution.x/iResolution.y - 1.) ldSq = 1e5;
    //if(abs(ldIP.x + .5)>ldSc.x/2. - 1.) ldSq = 1e5;
    if(abs(ldIP.y + .5)>ldSc.y/2. - 1.) ldSq = 1e5;

    //if(mod(ldIP.y - (ldSc.y - 1.), ldSc.y*2.)/(ldSc.y*2.)<ldSh) ldSq = 1e5;
    float x = mod(ldIP.x*2. - (ldSc.x - 1.), ldSc.x*2.);
    float y = mod(ldIP.y*2. - (ldSc.y - 1.), ldSc.y*2.);
    float pos = y*ldSc.y + x;

 
    //if(pos/(ldSc.y*ldSc.x*2.)<ldTf) ldSq = 1e5;
    //if(pos/(ldSc.y*ldSc.x*2.)<-hash21B(ldIP) + ldTf*2.) ldSq = 1e5;
    if(hash21B(ldIP + .45)<ldTf) ldSq = 1e5;
    

    vec3 ldSqCol = .5 + .45*cos(6.2831*hash21B(ldIP) + vec3(0, 1, 2));
    ldSqCol = mix(ldSqCol, vec3(1), .1)*max(1.25 - dot(uv, uv), 0.);
    
  
    // Background texture2D.
    vec3 tx = texture2D(iChannel0, uv*2. + hash21B(ldIP + .35)).xyz; tx *= tx;
    tx = .8 + tx*.4;
    
    vec3 ldCol = scBgnd;
    //ldCol = mix(ldCol, ldCol*8., 1. - bgCol);

    //ldCol = bgCol;
    ldCol = mix(ldCol, vec3(0), (1. - smoothstep(0., sf*6., ldSq))*.25);
    //ldCol = mix(ldCol, ldSqCol*1.5, 1. - smoothstep(0., sf, ldSq));
    //ldCol = mix(ldCol, vec3(0), 1. - smoothstep(0., sf, ldSq + .075));
    ldCol = mix(ldCol, ldSqCol*(.8 + b*.5), 1. - smoothstep(0., sf, ldSq + .1));
    
    
    // Texture application.
    ldCol *= tx;
        

    // Time arc calculations.
    float arc = abs(length(uv) - .1) - .03;
    //arc = abs(arc + .0125) - .0125;
    // Loading angle.
    float a = 6.2831*ldTf;
    vec2[3] v; v[0] = vec2(0), v[1] = vec2(0, 1), v[2] = vec2(sin(a), cos(a));

    // Cutting a wege from the arc. Terrible -- I should be using IQ's arc function. 
    float d = a>0.? wedge(uv, v[0], v[1], v[2]) : 1e5;
    if(a>3.14159) d = min(-d, -uv.x);
    arc = max(arc, -d);

    //ldCol = scBgnd;
 
 
    // Rendering the timing arc overlay.
    sf /= ldSc.x;
    vec3 svCol = ldCol;
    ldCol = mix(ldCol, vec3(0), (1. - smoothstep(0., sf*12., arc))*.5);
    ldCol = mix(ldCol, vec3(0), (1. - smoothstep(0., sf, arc))*.9);
    ldCol = mix(ldCol, svCol*.5 + .5, (1. - smoothstep(0., sf, arc + .006)));
    ldCol = mix(ldCol, ldCol*1.5, (1. - smoothstep(0., sf, arc + .0275)));
    
    // Loading screen color.
    return ldCol;


}


void main( ){

    
    // Aspect correct screen coordinates. Translation and scale is all that
    // 
	vec2 uv = (xlv_TEXCOORD0 - iResolution.xy*.5)/iResolution.y;
    
    // People get bored very quickly, so it's customary to show a loading
    // screen during precalculations. You can make it as simple or as
    // complicated as you want... I honestly don't know what I was going
    // for when I started coding it, but a rushed ten minutes later there
    // was a rotating bar sitting over a weird bumped pixel dissolve. At
    // least it'll keep people amused. :)
    vec3 scBgnd = vec3(.8, .6, 1)*.03*max(1.25 - dot(uv, uv), 0.);
    if(iFrame<=maxFrames){

        vec3 ldCol = loadingScreen(uv, scBgnd);
        
        gl_FragColor = vec4(sqrt(max(ldCol, 0.)), 1.);
        
        return;
    }
	
    
	// Camera Setup.
    float tm = iTime/2.;
	vec3 ro = vec3(.235, tm/1.5, .25); // Camera position, doubling as the ray origin.
	vec3 lk = ro + vec3(.02, .15, 0); // "Look At" position.
    vec3 lp = ro + vec3(0, 1.25, .1);// Put it a bit in front of the camera. 
    #if EDGES > 1
    lp.xz -= .1;
    #endif
    
    ro.xz *= twist(-(ro.y + 2.*0.));
    lk.xz *= twist(-(lk.y + 2.*0.));
 	lp.xz *= twist(-(lk.y + 2.*0.));
 

    // Using the above to produce the unit ray-direction vector.
    float FOV = 1.; // FOV - Field of view.
    vec3 fwd = normalize(lk - ro); // Forward.
    //if(dot(fwd, vec3(fwd.z, 0, -fwd.x))==0.) fwd = normalize(fwd - vec3(0, 0, .00001));
    vec3 rgt = normalize(cross(vec3(0, 1, 0), fwd));// Right. 
    // "right" and "forward" are perpendicular normals, so the result is normalized.
    vec3 up = cross(fwd, rgt); // Up.
    
    // rd - Ray direction.
    vec3 rd = mat3(rgt, up, fwd)*normalize(vec3(uv, 1./FOV - dot(uv, uv)*.25));
    // Equivalent to:
    //vec3 rd = normalize(uv.x*rgt + uv.y*up + fwd/FOV);
    
 
    // Slight agitation of the camera about the XZ-plane.
	//rd.xz *= rot2(-cos(tm/2.)/2.);

	 
    
    // Raymarch to the scene.
    float t = trace(ro, rd);

    // Object ID.
    float svObjID = objID;
    
   
  
	
    // Initiate the scene color to black.
	vec3 col = vec3(0);
	
	// The ray has effectively hit the surface, so light it up.
	if(t < FAR){
        
  	
    	// Surface position and surface normal.
	    vec3 sp = ro + rd*t;
	    //vec3 sn = getNormal(sp, edge, crv, ef, t);
        vec3 sn = getNormal(sp);
        
        
        if(svObjID<.5) sn = doBumpMap(sp, sn, .02);///(1. + t/FAR*1.)
        
          
        // Obtaining the texel color. 
	    vec3 texCol;   
        
        vec3 txP = sp;
        vec3 txN = sn;

        txP.xz *= twist(txP.y);
        txN.xz *= twist(txP.y);
        vec3 cntr = vec3(txP.xy, txP.z);
        
        vec3 tx = tex3D(iChannel0, txP*2., txN);
        //vec3 tx = texture2D(iChannel0, txPat.xy).xyz; tx *= tx;
        // Texture application.
        tx = .75 + tx*.5;
    


        // The extruded grid.
        if(svObjID<.5){
            
           vec3 uvw = abs(cntr.x)>abs(cntr.z)*1.? vec3(-1, 1, 1)*cntr.zyx : cntr;
    
            vec2 uv2 = fract(getUV(uvw)) - .5;
            // Cube map face one, which contains the rectangle center and
            // dimension information.
            vec4 txPat = texture2D(iChannel2, vec3(-.5, uv2.yx));
            // Cube map face two, which contains the smooth rectangle distance
            // field information.
            vec4 txPat2 = texture2D(iChannel2, vec3(.5, uv2.y, -uv2.x));
            
            // Taking care of cube map wrapping issues... It's one of the annoying
            // downsides of using cube map faces.
            txPat.zw = fract((floor(txPat.zw*cubemapRes) + .5)/cubemapRes);
            vec2 cntr = txPat.zw; // Rectangle center.
            
            
            float rnd = hash21(txPat.zw + .1);
            float rnd2 = hash21(txPat.zw + .2);
                
            // Blinking light variable.    
            float bl = smoothstep(.9, .95, sin(rnd2*6.2831 + iTime/2.)*.5 + .5);
        
            // Rectangle coloring.
            #if COLOR == 0
            vec3 tCol = .5 + .45*cos(6.2831*hash21(cntr) + vec3(0, 1, 2));
            #elif COLOR == 1
            vec3 tCol = .5 + .45*cos(6.2831*hash21(cntr)/4. + vec3(0, 1, 2)/1.5 + .5);
            tCol = mix(tCol, tCol.zyx, step(.75, hash21(cntr + .51)));
            #else
            vec3 tCol = vec3(.15, .015, 1)*(hash21(cntr + .31) + .5);
            tCol = mix(tCol, tCol.yxz, hash21(cntr + .51)*.25);
            tCol = mix(tCol, tCol*4., bl);
            #endif
            
            // Fine tuning then applying the texture2D.
            tCol = mix(tCol*2., vec3(1), .25);
            texCol = tCol*tx;//*(rnd*.8 + .2);//*(rnd*1.95 + .05);//tx*2.
            
            //if(svObjID>.5) texCol = mix(texCol, tx*2., .5);
             
            vec3 svCol = texCol;
           
            // Smooth rectangle distance field value.
            //float val = bumpSurf3D(sp, sn);
            float val = getTex1(iChannel2, uvw);
            
            // Smoothing factor.
            float sf = 2./cubemapRes.x;
            //float df = mix(0., 1., smoothstep(0., sf, (val + sf)));//tx.x

            // Texture coloring overide, just to show it works with those too.
            //texCol = texture2D(iChannel1, cntr).xyz; texCol *= texCol*3.;

            texCol = mix(vec3(0), texCol, 1. - smoothstep(0., sf, val + sf));//.25-txPat.x/.1

            /*
            if(iFrame<maxFrames){

                float df = getTex1(iChannel2, uvw); //ldTex.x>0.? 0. : 1.
                vec3 bgCol = mix(vec3(.1), vec3(.5), df);
                texCol = bgCol;

            }
            */
            


        }
        else {
 
             // Repeat cube objects along the rails, partitions, etc.
            
            // Repeat calculations. Not pretty, so they'll need a tidy up at some stage.
            const float sc = 14.;
            float ew = 1./sc;
            float dOffs = ew/8.*2./sc;
            vec3 offs = vec3(.0/sc, .5/sc, .0/sc); // The edge XZ has been moved in by .25/sc.
            vec3 ip = txP;
            ip = floor((ip + offs)/(1./sc - vec3(dOffs, 0, dOffs)));
            txP = abs(mod(txP + offs, 1./sc - vec3(dOffs, 0, dOffs)) - (1./sc - vec3(dOffs, 0, dOffs))/2.);
            
            // Cube field.
            float cb = min(min(max(txP.x, txP.y), max(txP.x, txP.z)), max(txP.y, txP.z));
            cb -= (.5/sc);
            
            // Unique rand IDs for coloring, blinking, etc.
            float rnd = hash31(ip);
            float rnd2 = hash31(ip + .7);
            
            // Blinking.
            float bl = smoothstep(.9, .95, sin(rnd2*6.2831 + iTime)*.5 + .5);
            
            // ID based colors.
            #if COLOR == 0
            vec3 tCol = .5 + .45*cos(6.2831*rnd + vec3(0, 1., 2));
            #elif COLOR == 1
            vec3 tCol = .5 + .45*cos(6.2831*rnd/4. + vec3(0, 1, 2)/1.5 + .5);
            #else
            vec3 tCol = vec3(.15, .015, 1);
            tCol = mix(tCol, tCol.yxz, step(.75, hash31(ip + .51)));
            tCol = mix(tCol, tCol*8., bl);//step(.8, hash21(txPat.xy + .5))
            #endif
            
            // Color refinement and mild texturing.
            tCol = mix(tCol*1., vec3(1), .25);
            texCol = tCol*tx*2.;
            
            // Applying the colors to the cubes.
            vec3 svCol = texCol;
            #if COLOR < 2
            vec3 cubeCol = tx*2.;
            #else
            vec3 cubeCol = tx/6.;
            #endif
            //
            texCol = mix(vec3(0), cubeCol, 1. - smoothstep(0., .001, abs(cb) - .016));
            texCol = mix(texCol, tCol, 1. - smoothstep(0., .001, (cb + .016 + .007)));
        }
       
    	
    	// Light direction vector.
	    vec3 ld = lp - sp;

        // Distance from respective light to the surface point.
	    float lDist = max(length(ld), .001);
    	
    	// Normalize the light direction vector.
	    ld /= lDist;

        // Light attenuation, based on the distances above.
	    float atten = 1./(1. + lDist*lDist*.25);
        
        // Shadows and ambient self shadowing.
    	float sh = softShadow(sp, lp, sn, 16.);
    	float ao = calcAO(sp, sn); // Ambient occlusion.
        //sh = min(sh + ao*.25, 1.);
        
   
    	
    	// Diffuse lighting.
	    float diff = max( dot(sn, ld), 0.);
        //diff = pow(diff, 4.)*2.; // Ramping up the diffuse.
    	
    	// Specular lighting.
	    float spec = pow(max(dot(reflect(ld, sn), rd ), 0.), 32.); 
      
        // Half vector.
        vec3 hv = normalize(rd + ld);
        // Specular Blinn Phong. The last term is highlight power related.
        float specBF = pow(max(dot(hv, sn), 0.), 4.);
        
        // Fresnel term. Good for giving a surface a bit of a reflective glow.
        float fre = pow(clamp(1. - abs(dot(sn, rd))*.5, 0., 1.), 32.);
 
        
		// Schlick approximation. I use it to tone down the specular term. It's pretty subtle,
        // so could almost be aproximated by a constant, but I prefer it. Feel free to look up
        // the "science," but it essentially takes that annoying central shine out. How
        // much you take out depends on the material, which is controlled by the material
        // constant.
		float Schlick = pow( 1. - max(dot(rd, normalize(rd + ld)), 0.), 5.);
		float freS = mix(.2, 1., Schlick);  //F0 = .2 - Glass... or close enough.        
        
 
        // Combining the above terms to procude the final color.
        col = texCol*(diff*sh + .15);
         
        #ifdef SHINY
        // If shininess is flagged, include the specular term.
        col += spec*freS*8.*sh/(1. + t*t*.5); 
        #endif
        
        // Cube map reflection. When I first learned to code graphics, I loved shiny
        // stuff and went really overboard with it. I still do, but thanks to maturity, 
        // I've learned to hold back on ocassion. :)
        // Requires a cube map in "iChannel3".
        //vec3 refCol = texture2D(iChannel3, reflect(rd, sn)).xyz; refCol *= refCol;
        //col += col*refCol*sh*1.;

        
        // Shading.
        col *= ao*atten;
        
        // Greyscale value, just in case people switch to the Britney video, etc.
        // Stylistically, the example works better with color. The Britney video
        // looks OK, but I'm more of a Shirley Jones kind of guy. :)
        #ifdef GRAYSCALE
        col = vec3(1)*dot(col, vec3(.299, .587, .114));
        #endif
        
        #ifdef MAP_DETAIL
        // Also used for debugging purposes.
        col = vec3(crv*(sh*.9 + .1)); // ao, sh, etc.
        #endif
          
	
	}
    
    
    // Applying fog.
    vec3 fogCol = vec3(0);//mix(vec3(.4, .7, 1.2), vec3(.4, .7, 1.2).yxz, rd.y*.5 + .5)*4.;
    col = mix(col, fogCol, smoothstep(0., .99, t/FAR));
    

    // Just the displacement map on its own. Actually, this is a bump mapped
    // version. The texture2D map itself is quite mundane, which you can see 
    // if you set "b2" to 1.
    #ifdef DISPLACEMENT_MAP
 
    
    // Scaling.
    vec2 p2 = ((xlv_TEXCOORD0)/iResolution.y) + iTime/24.;
  
    
    // Cube map texture2D coordinate conversion.
    p2 *= cubemapRes;
    vec2 ip2 = floor(p2); p2 -= ip2;
    vec2 uv2 = fract((ip2 + .5)/cubemapRes);

            
    // 2D neighboring texels stored in each of the four texture2D channels.
    vec4 tx2 = texture2D(iChannel2, vec3(-.5, uv2.yx - .5)); 
    vec4 tx3 = texture2D(iChannel2, vec3(.5, uv.y - .5, -uv.x - .5)); 
   
    if(iFrame<maxFrames){
        // Line formation -- Not applicable when using a loading screen.
        if(tx2.x>.001) col = vec3(.5);
        else col = vec3(.1);
    }
    else { 
 
        vec4 rect = vec4(uv2 - tx2.zw, tx2.xy);//getRect(uv2, tx2);//

        // Taking care of wrapping issues by snapping to the cube map face
        // pixel positions... I get wrapping issues all the time simply 
        // because I forget to do this.
        tx2.zw = fract((floor(tx2.zw*cubemapRes) + .5)/cubemapRes);

        // Random rectangle coloring.
        col = .5 + .45*cos(6.2831*(hash21(tx2.xy)) + vec3(0, 1, 2));
        col = mix(col, vec3(1), .1);
        // Background texture2D.
        vec3 tx = texture2D(iChannel0, uv2*4.).xyz; tx *= tx;
        tx = .8 + tx*.4;
        col *= tx;
        vec2 u = xlv_TEXCOORD0/iResolution.xy;
        col *= (pow(16.*u.x*u.y*(1.-u.x)*(1.-u.y), 0.125) + .1);
        
        // Texture coloring.
        //col = texture2D(iChannel1, tx2.zw).xyz; col *= col*2.;
        
        float sf = 1./iResolution.y;
       
        // Rectangle distance evaluation. 
        float dff = max(abs(rect.x) - rect.z/2., abs(rect.y) - rect.w/2.);
        vec2 lgt = normalize(vec2(1, 2))/450.;
        // Offset rectangle calculation for highlighting purposes.
        rect.xy += lgt;
        float dff2 = max(abs(rect.x) - rect.z/2., abs(rect.y) - rect.w/2.);

        // Bump calculation.
        float b = max(dff2 - dff, 0.)*450.;//max(.25 - dff/.05, 0.);
        
        
        // Rendering.
        col = mix(vec3(0), col*(.5 + b*.8), 1. - smoothstep(0., sf, (dff + .003)));
       
        // Center circle.
        //col = mix(col, vec3(0), 1. - smoothstep(0., sf, length(uv2 - tx2.zw) - .005));
  
    }
    
   
     
    #endif      


    // 1.5 second fade-in (assuming 60 FPS) after precalculation.
    if(iFrame>maxFrames) col = mix(col, scBgnd, max(float(maxFrames + 90 - iFrame), 0.)/90.);
    
    // Rought gamma correction.
	gl_FragColor = vec4(sqrt(max(col, 0.)), 1);
	
}

