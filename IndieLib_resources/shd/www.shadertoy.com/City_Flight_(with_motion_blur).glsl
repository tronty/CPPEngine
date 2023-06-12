sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel3;
#define texelFetch(a1,a2,a3) texture2D(a1,a2)
// Fork of "City Flight" by athibaul. https://shadertoy.com/view/WstcRf
// 2022-07-10 08:07:51

// -----------
// CITY FLIGHT
// -----------
// by Alexis THIBAULT
//
// Flying through a city using simple raymarching


// This shader was inspired by the Disney Animation paper:
//
// Norman Moses Joseph, Brett Achorn, Sean D. Jenkins, and Hank Driskill.
// Visualizing Building Interiors using Virtual Windows.
// In ACM SIGGRAPH Asia 2014 Technical Briefs, December 2014.


// Stuff that uses similar ideas
// (marching a regular grid by raytracing the boxes, and
//  putting stuff inside)
//
// "Cubescape" by iq
// https://www.shadertoy.com/view/Msl3Rr


// Update - adjusted the max number of iterations: it runs even faster now!
//        - Use the "volume" noise texture instead of the 2D one in hash()
//            (which is much more logical).
//        - Added stars in the sky!


// Should the windows be flashing?

//Added motion blur with this line, and the "Blue Noise" texture in iChannel3:
#define iTim (iTime + (iMouse.z>0. ? 0. : texelFetch(iChannel3,ivec2(xlv_TEXCOORD0)%1024,0).r * iTimeDelta))

#define PARTY_MODE 0


vec2 boxIntersect(vec3 ro, vec3 rd, vec3 r, out vec3 normal, out vec3 normal2)
{
    // Compute intersection time t of ray ro+t*rd
    // with a box of half-size r centered at 0.
    ro *= sign(rd);
    vec3 t1 = (-r-ro)/abs(rd);
    vec3 t2 = (r-ro)/abs(rd);
    float tmin = max(t1.x, max(t1.y, t1.z));
    float tmax = min(t2.x, min(t2.y, t2.z));
    normal = -sign(rd) * step(t1.yzx,t1.xyz) * step(t1.zxy, t1.xyz);
    normal2 = -sign(rd) * step(t2.xyz,t2.yzx) * step(t2.xyz, t2.zxy);
    if(tmax < tmin) return vec2(-1.);
    return vec2(tmin, tmax);
}

vec3 hash(vec3 p)
{
    return textureLod(iChannel0, (floor(p)+0.5)*(1./32.), 0.).xyz;
}

#if 0
vec3 sun = normalize(vec3(1.0, 1., 0.2));
vec3 sunCol = vec3(1.,0.5,0.2);
vec3 skyCol = vec3(0.4,0.65,1.0);
vec3 horizonCol = vec3(0.6,0.7,0.8);
#else
vec3 sun = normalize(vec3(1.0, 1., 0.2));
vec3 skyCol = vec3(0.01,0.02,0.07);
vec3 horizonCol = vec3(0.002,0.005,0.02);
#endif

const float T_MAX = 1000.;
const float FLOOR = -80.;
const float HEIGHT = 20.;
const vec3 blockSize = vec3(5.,5.,1000.);

vec3 skyColor(vec3 rd)
{
    #if 0
    vec3 col = skyCol;
    vec3 horiz = mix(horizonCol, 2.5*sunCol, smoothstep(-1.,1.,dot(rd,sun)));
    float horizHeight = 0.1*exp(-2.*(1.3-dot(sun, rd)));
    col = mix(col, horiz, exp(-max(rd.z,0.)/horizHeight));
    col *= exp(min(rd.z,0.)*15.);
    #else
    vec3 col = skyCol;
    float horizHeight = 0.1;
    col = mix(col, horizonCol, exp(-max(rd.z,0.)/horizHeight));
    col *= exp(min(rd.z,0.)*15.);
    #endif
    return col;
}

void getCurrentBuilding(vec3 ro, out vec3 boxC,
                        out vec3 buildingC, out vec3 buildingSize)
{
    boxC = 2.*blockSize*round(ro/(2.*blockSize));
    buildingC = vec3(boxC.xy, -2.*HEIGHT) + vec3(2.,2.,0.)*(2.*hash(boxC.zxy)-1.);
    vec2 maxSize = 4.5-abs(buildingC.xy-boxC.xy);
    buildingSize = vec3(1,1,2.*HEIGHT) + vec3(maxSize.xy-1.,13.)*hash(boxC.yzx);
}


float sceneIntersect(vec3 ro, vec3 rd, out vec3 normal)
{
    float t = 0.;
    float t0 = 0.;
    vec3 boxC = vec3(0);
    int i;
    vec3 p;
    const int ITER = 40;
    vec3 buildingC, buildingSize;
    vec3 _; // Dummy variable
    for(i=0; i<ITER; i++)
    {
        // Intersect building in current box
        getCurrentBuilding(ro, boxC, buildingC, buildingSize);
    	t = boxIntersect(ro-buildingC,
                         rd, buildingSize, normal, _).x;
        
        // Intersect current box itself
        float t1 = boxIntersect(ro-boxC, rd, blockSize, _, _).y;
        
        // Also intersect a floor plane and a sky plane
        float tfloor = -(ro.z-FLOOR)/rd.z;
        if(tfloor < 0.) tfloor = 1e5;
        float tsky = -(ro.z - 20.)/rd.z;
        if(tsky < 0.) tsky = 1e5;
        
        if(t > 0. && t < t1 && t < tfloor)
        {
            // We hit the building!
        	//p = ro+t*rd;
            return t0+t;
            break;
        }
        else if(tfloor > 0. && tfloor < t1)
        {
            // We hit the floor!
            //p = ro+tfloor*rd;
            return T_MAX;
            normal = vec3(0,0,1);
            return t0+tfloor;
        }
        else if(tsky > 0. && tsky < t1)
        {
            // We hit the ceiling!
            return T_MAX;
        }
        // We hit nothing : march to the next cell
        ro += (t1+0.001)*rd;
        t0 += t1;
        p = ro+t1*rd;
        continue;
    }
    return T_MAX;
}

void getRoom(vec3 p, vec3 rd, out vec3 roomSize, out vec3 roomCenter,
            out vec3 roomHash)
{
    
    vec3 boxC, buildingC, buildingSize;
    getCurrentBuilding(p, boxC, buildingC, buildingSize);

    roomSize = buildingSize/(2.*round(buildingSize)+1.);
    roomCenter = round((p-buildingC)/(2.*roomSize) + 0.1*rd)*2.*roomSize + buildingC;
    roomHash = hash(roomCenter*10.);
}

vec3 someNoise(vec3 p)
{
    p *= 10.;
    vec3 v = hash(p)*0.5;
    p.xyz = p.yzx*1.62;
    v += hash(p)*0.25;
    p.xyz = p.yzx*1.62;
    v += hash(p)*0.125;
    return v;
}


vec3 computeEmission(vec3 p, vec3 rd, float t, vec3 normal,
                    out float isInWindow)
{

    // Window emission depends on the ray direction...
    // because we actually look at what's inside the room!
    vec3 roomSize, roomCenter, roomHash;
    getRoom(p, rd, roomSize, roomCenter, roomHash);
    vec3 roomHash2 = hash(roomCenter.yzx);
    vec3 q = abs(p-roomCenter);
    vec3 inNormal, _;
    vec2 inT = boxIntersect(p-roomCenter, rd, roomSize, _, inNormal);
    vec3 roomP = p+inT.y*rd;
    
    float border = 0.1;
    float muntins = roomHash2.z > 0.5 ? 0.01 : -0.1;
    float w = t/(iResolution.y*dot(normal,-rd)); // A little anti-aliasing
    isInWindow = (smoothstep(q.x-w, q.x+w, roomSize.x-border) * smoothstep(q.x+w, q.x-w, muntins)
                + smoothstep(q.y-w, q.y+w, roomSize.y-border) * smoothstep(q.y+w, q.y-w, muntins))
       		    * smoothstep(q.z-w, q.z+w, roomSize.z-border) * smoothstep(q.z+w, q.z-w, muntins)
                * step(-0.5,-normal.z);

    #if PARTY_MODE
    float thresh = 0.8 + 0.1*sin(6.28*iTim+6.28*roomHash.r);
    float thresh2 = 0.85 + 0.05*sin(6.28*iTim+6.28*roomHash.g);
    #else
    float thresh=0.8, thresh2=0.85;
    #endif
    vec3 emission = vec3(1.,0.7,0.5)*smoothstep(thresh-0.1,thresh+0.2,roomHash.g)
        + vec3(0.5,0.8,1.)*0.8*smoothstep(thresh2-0.1,thresh2+0.1,roomHash.b);
    emission *= emission*3.0;

    //emission = 0.5+0.5*inNormal;
    //emission = (roomCenter - (ro+inT.y*rd));
    vec3 noise = someNoise(roomP);
    vec3 randomColor = mix(roomHash2, 1.-roomHash2.yzx, smoothstep(roomHash.x, roomHash.y,noise.rrr));
    vec3 wallColor = dot(inNormal, 2.*roomHash-1.) > 0. ? vec3(1.0) : randomColor;
    if(inNormal.z > 0.9)
    {
        // Floor is same color as the light, with some pattern
        wallColor = emission*0.3;
        vec3 floorP = roomHash2.y > 0.5 
            ? vec3(roomP.x+roomP.y,roomP.x-roomP.y,roomP.z)*0.7
            : roomP;
        wallColor *= someNoise(floorP).rrr;
    }
    wallColor += 0.5;
    
    // Make ceiling not too bright
    wallColor = mix(wallColor*2., vec3(0), 0.9*smoothstep(-roomSize.z, roomSize.z, roomP.z-roomCenter.z));
    vec3 ligPos = roomCenter + 0.7*vec3(roomHash2.xy*2.-1.,1.)*roomSize;
    float intensity = 0.5/dot(ligPos-roomP,ligPos-roomP);
    vec3 insideLight = emission * clamp(dot(inNormal, normalize(ligPos-roomP)),0.,1.) * intensity * wallColor;
    insideLight += 0.5*roomHash2*roomHash2*emission;
    
    // Some windows have "curtains"... but you can peek through :)
    vec2 curtainW = roomSize.xy - 0.15;
    float curtains = roomHash.x > 0.8 
        ? 0.9*(smoothstep(q.x-w, q.x+w, curtainW.x) + smoothstep(q.y-w, q.y+w, curtainW.y))
        : 0.0;
    emission = mix(emission*(1.+roomHash2)*0.1, insideLight, 1.-curtains);
    
    emission *= isInWindow;
	return emission;
}

vec3 raycast(vec3 ro, vec3 rd)
{
    vec3 normal, normal2;
    float t = sceneIntersect(ro, rd, normal);
    vec3 p = ro+t*rd;
    // And after one bounce
    vec3 ro2 = p + 0.01*normal, rd2 = reflect(rd, normal);
    float t2 = sceneIntersect(ro2, rd2, normal2);
    
    //return (t < T_MAX) ? 0.5+0.5*normal : vec3(0);
    
    vec3 _; // Dummy variable
    if(t < T_MAX)
    {
        // Let's do some shading!
        float ao = 0.5+0.5*normal.z;
        float darkVoid = smoothstep(FLOOR,0.,p.z);
        ao *= darkVoid;
        
        float isInWindow, _;
        vec3 emission = computeEmission(p, rd, t, normal, isInWindow);
        vec3 emission2 = computeEmission(ro2+t2*rd2, rd2, t+t2, normal2, _);
        emission2 = t2 < T_MAX ? emission2 : skyColor(rd2);
        
        vec3 roomSize, roomCenter, roomHash;
        getRoom(p, rd, roomSize, roomCenter, roomHash);
    	vec3 surfCol = 0.4+0.5*smoothstep(0.5,0.9,roomHash.rrr)-0.2*isInWindow;
        vec3 F0 = 0.04+vec3(0.04,0.1,0.2)*surfCol;
        
    	vec3 fre = F0 + (1.-F0) * pow(clamp(1.-dot(-rd,normal),0.,1.),5.);
        vec3 col = mix(emission, emission2, fre);
        col = mix(vec3(0.), col, isInWindow);
        //vec3 col = emission;
        //col += surfCol* clamp(dot(normal, sun), 0., 1.) * sunCol  * darkVoid;
        //col += surfCol* clamp(dot(normal, -rd), 0., 1.)  * darkVoid * 2./ t;
        //col += surfCol*vec3(0.5,0.7,0.9) * 0.1 * (0.5+0.5*normal.z) * ao;
        //col += surfCol* sunCol*sunCol * 0.1 * clamp(dot(normal.xy, -sun.xy), 0., 1.) * ao;
        
        col += surfCol * ao * 0.2 * (0.5-0.2*normal.z) 
            * smoothstep(10.0,-30.0,p.z) * vec3(1.,0.8,0.6);
		
        col = mix(col, vec3(0.), 1.-exp(-t*0.003));
        col = mix(col, skyColor(rd), 1.-exp(-t*0.01));
        col *= exp(0.02*min(p.z,0.));
        //col +=;
        return col;
    }
    else
    {
        vec3 col = skyColor(rd);
        // Stars
        float stars = smoothstep(0.96,1.01,textureLod(iChannel1,(round(1.5*rd.yz/(rd.x+1.)*iResolution.y)+0.5)/256., 0.).r)
            * smoothstep(0.0,1.0,textureLod(iChannel1,(rd.yz/(rd.x+1.)*450.)/256., 0.).r);
        col += stars*stars*smoothstep(0.0,0.2,rd.z);
        return col;
    }
}

vec3 path(float t)
{
    //return vec3(20.*t, 20.0*sin(0.3*t), 5.0 - 2.*cos(0.5*t));
    return vec3(20.*t, 5.+0.1*cos(0.5*t),10.*cos(0.5*t)*(1.-0.5*sin(0.1*t)));
}

void main( )
{
    vec2 uv = (2.*xlv_TEXCOORD0-iResolution.xy)/iResolution.y;
	
    //vec3 ro = vec3(-3.*cos(th),-3.*sin(th),2.0);
    //vec3 target = vec3(0);
    vec3 ro = path(iTim);
    vec3 target = path(iTim+3.)+vec3(0.,20.*cos(0.3*iTim),-15.);
    vec3 camFwd = normalize(target - ro);
    vec3 camRight = normalize(cross(camFwd, vec3(0.3*cos(0.2*iTim),0,1))); // Camera tilts
    vec3 camUp = cross(camRight, camFwd);
    vec3 rd = normalize(camFwd + 0.7*(uv.x*camRight+uv.y*camUp));
    
    
    vec3 col = raycast(ro, rd);
    
    // Vignette
    col *= smoothstep(1.7,0.5,length(2.*xlv_TEXCOORD0/iResolution.xy-1.));
    // Tone mapping
    col = mix(col, 1.-(4./27.)/(col*col), step(2./3.,col));
    // Gamma correction
    col = pow(col, vec3(0.45));
    
    gl_FragColor = vec4(col,1.0);
}

	
