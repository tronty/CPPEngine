/*
"Wall and chimney" by Emmanuel Keller aka Tambako - February 2016
License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
Contact: tamby@tambako.ch
*/

#define pi 3.141593

// Switches, you can play with them!
#define brick_bump
#define specular
#define shadow
#define ambocc
#define reflections
#define procedural_clouds
#define show_chimney
#define show_smoke
#define heat_refraction
#define smoke_turbulence
    
struct DirLamp
{
  vec3 direction;
  vec3 color;
  float intensity;
};

struct RenderData
{
  vec3 col;
  vec3 pos;
  vec3 norm;
  int objnr;
};
    
float aaindex;

// Every object of the scene has its ID
#define SKY_OBJ        0
#define WALL_OBJ       1
#define BRICKS_OBJ     2
#define CHIMNEY_OBJ    3

float currTime;
const float timeFactor = 1.;

// Scene objects sizes and description
const vec3 wallPos = vec3(30, -20, -11.3);
const vec3 wallSize = vec3(20, 46, 50);
const vec3 brickSize = vec3(0.2, 0.34, 2.04);
const vec3 brickStep = vec3(0., 0.44, 2.29);
const float brickBR = 0.05;

const float tubeDiam = 1.8;
const float tubeLen1 = 2.1;
const float tubeclen = 1.6;
const float tubeLen2 = 1.5;
const float tubeDiam2 = 2.6;
const float tubeLen3 = 0.9;
const vec3 chimneyOrig = vec3(-1.3, wallPos.x - wallSize.x - tubeLen1 + 0.2, -17.5);

// Camera options
vec3 campos;
vec3 camtarget = vec3(7., 3., -18);
vec3 camdir;
float fov = 4.5;

// Lighing options
const vec3 ambientColor = vec3(0.1, 0.5, 1.);
const float ambientint = 0.21;
DirLamp sunLamp = DirLamp(vec3(-3., 1.2, 1.7), vec3(1., 0.98, 0.94), 1.5);

// Shading options
float specint[4];
float specshin[4];
const float aoint = 0.9;
const float aoMaxdist = 50.;
const float aoFalloff = 0.2;
const float shi = 0.55;
const float shf = 0.05;
    
// Tracing options
const float normdelta = 0.001;
const float maxdist = 400.;
const float nbrref = 12.;      // This controls the number of diffuse reflections. You can set it higher if you want the chimney appear softer, but the FPS will drop
const float rrefblur = 0.8;

// Smoke options
const float smokeBrightness = 4.0;
const vec3 smokeCol = vec3(0.7, 0.75, 0.8)*smokeBrightness;
const float smokeColPresence = 0.4;
const float smokeColBias = 0.7;
const float smokeScale = 2.;
const float smokeSpeed = 5.7;
const float smokeDens = 5.5;
const float smokeBias = -0.01;
const float smokePow = 1.9;
const float smokeRefInt = 0.0003;
const float smokeTurbulence = 0.05;
const float smokeTurbulenceScale = 2.5;

// Wind options
const float maxWindIntensity = 1.8;
const float maxWindAngle = 0.27;
float windIntensity;
float dWindIntensity;
float windAngle;

// Cloud options
const vec3 cloudSize = vec3(0.07, 0.2, 0.07);
const vec3 cloudSpeed = 0.001*vec3(1., 0.3, 1.2);

// Antialias. Change from 1 to 2 or more AT YOUR OWN RISK! It may CRASH your browser while compiling!
const float aawidth = 0.65;
const int aasamples = 1;

// 1D hash function
float hash( float n ){
	return fract(sin(n)*3538.5453);
}

// 3D hash function
vec3 hash3( vec3 n ){
	return fract(vec3(sin(n.x)*3538.5453, sin(n.y)*2753.8256, sin(n.z)*5296.3728));
}

// 2D Vector rotation
vec2 rotateVec(vec2 vect, float angle)
{
    vec2 rv;
    rv.x = vect.x*cos(angle) - vect.y*sin(angle);
    rv.y = vect.x*sin(angle) + vect.y*cos(angle);
    return rv;
}

// From https://www.shadertoy.com/view/4sfGzS
#ifdef procedural_clouds
float noise(vec3 x)
{
    //x.x = mod(x.x, 0.4);
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0 - 2.0*f);
	
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix(hash(n + 0.0), hash(n + 1.0),f.x),
                   mix(hash(n + 157.0), hash(n + 158.0),f.x),f.y),
               mix(mix(hash(n + 113.0), hash(n + 114.0),f.x),
                   mix(hash(n + 270.0), hash(n + 271.0),f.x),f.y),f.z);
}

const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );

// From https://www.shadertoy.com/view/4sfGzS
float noise2(vec3 pos)
{
    vec3 q = 8.0*pos;
    float f  = 0.5000*noise(q) ; q = m*q*2.01;
    f+= 0.2500*noise(q); q = m*q*2.02;
    f+= 0.1250*noise(q); q = m*q*2.03;
    f+= 0.0625*noise(q); q = m*q*2.01;
    return f;
}
#endif

// Union operation from iq
vec2 opU(vec2 d1, vec2 d2)
{
	return (d1.x<d2.x) ? d1 : d2;
}

// Difference operation from iq
float opS(float d1, float d2)
{
    return max(-d2,d1);
}

// From https://iquilezles.org/articles/smin
float smin(float a, float b, float k)
{
    float h = clamp(0.5 + 0.5*(b - a)/k, 0.0, 1.0 );
    return mix(b, a, h) - k*h*(1.0 - h);
}

// From https://www.shadertoy.com/view/Xds3zN
float sdBox(vec3 p, vec3 b)
{
    vec3 d = abs(p) - b;
    return min(max(d.x, max(d.y, d.z)),0.0) + length(max(d, 0.0));
}

// From https://www.shadertoy.com/view/Xds3zN
float udRoundBox(vec3 p, vec3 b, float r)
{
    return length(max(abs(p) - b,0.0)) - r;
}

// From https://www.shadertoy.com/view/Xds3zN
float sdCylinder(vec3 p, vec2 h)
{
    vec2 d = abs(vec2(length(p.xz),p.y)) - h;
    return min(max(d.x,d.y), 0.0) + length(max(d, 0.0));
}

// From https://www.shadertoy.com/view/Xds3zN
float sdTorus(vec3 p, vec2 t)
{
    return length(vec2(length(p.xz) - t.x,p.y)) - t.y;
}
float sdTorusQ(vec3 p, vec2 t)
{
    return max(max(length(vec2(length(p.xz) - t.x,p.y)) - t.y, p.x), p.z);
}

// From https://www.shadertoy.com/view/Xds3zN
float sdConeSection(in vec3 p, in float h, in float r1, in float r2)
{
    float d1 = -p.y - h;
    float q = p.y - h;
    float si = 0.5*(r1 - r2)/h;
    float d2 = max( sqrt( dot(p.xz,p.xz)*(1.0 - si*si)) + q*si - r2, q );
    return length(max(vec2(d1,d2), 0.0)) + min(max(d1,d2), 0.);
}

// Distance mapping of the bricks
vec2 map_bricks(vec3 pos)
{
    vec3 pos2 = pos;
    pos2.yz+= 0.07*texture2D(iChannel1, pos.yz*0.005).g;
    pos2.z+= 0.5*(brickStep.z + 0.02)*mod(floor(0.5*pos2.y/brickStep.y), 2.);

    vec2 nb = floor(pos2.yz/brickStep.yz*vec2(0.5, 1.));
    float nbBrick = nb.x*2. + nb.y*80.;
    float btd = 1. + 0.3*(hash(nbBrick) - 0.5);
    
    pos2.yz = mod(pos2.yz, brickStep.yz*vec2(2., 1.));
    float bricks = udRoundBox(pos2 - vec3(wallPos.x - wallSize.x + brickSize.x*0.5*btd, brickStep.y, 0.), brickSize, brickBR);
    
    #ifdef brick_bump
    bricks+= 0.01*smoothstep(0.1, 0.95, texture2D(iChannel3, pos.yz*0.18).r + 0.6*texture2D(iChannel0, 0.2*pos.yz).r)*smoothstep(-0.2, -0.23, pos.x - wallPos.x + wallSize.x - brickSize.x*0.5*btd);
    #endif
    
    #ifdef show_chimney
    bricks = max(bricks, -sdCylinder(pos.yxz-chimneyOrig, vec2(tubeDiam/2. + 0.1, 2.)));
    #endif
    
    return vec2(bricks, BRICKS_OBJ);
}

#ifdef show_chimney
// Distance mapping of the inside (hole of the chimney)
float getTubeInt(vec3 pos)
{ 
    return sdCylinder(pos.yxz + vec3(tubeLen1 + tubeclen, -tubeclen - tubeLen2 - 3., 0), vec2(tubeDiam/2.3, tubeLen2*2.));
}

// Distance mapping of the metallic chimney
vec2 map_chimney(vec3 pos)
{
    const float cw = 0.16;
    const float ch = 0.02;
    
    pos = pos.yxz;
    pos-= chimneyOrig;
    
    // Horizontal tube
    float chimney = sdCylinder(pos, vec2(tubeDiam/2., tubeLen1));
    // Bands around the tube
    float angle1 = atan(pos.x, pos.z) + 3.*pos.y - 0.07;
    float angle2 = atan(pos.y + tubeLen1 + tubeclen, pos.z) - 3.*pos.x + 1.07;
    chimney-= ch*smoothstep(cw, 0., abs(0.2 - mod(angle1, pi)));
    
    // Curved tube
    float rtube = sdTorusQ(pos.yzx + vec3(tubeLen1, 0., -tubeclen), vec2(tubeclen, tubeDiam/2.));
    vec3 pos2 = pos.yzx + vec3(tubeLen1, 0., -tubeclen);
    // Bands around the tube
    float angle3 = atan(pos2.x - 0.025, pos2.z);
    float angle4 = atan(pos.x*cos(angle3*0.65 - 1.15) + (pos.y + 2.7)*sin(angle3*0.65 - 1.), pos.z) + 4.3*angle3 - 1.72;
    rtube-= ch*smoothstep(cw, 0., abs(0.2 - mod(angle4, pi)));
    
    chimney = min(chimney, rtube);
    
    // Vertical tube
    float tube2 = sdCylinder(pos.yxz + vec3(tubeLen1 + tubeclen, -tubeclen - tubeLen2, 0), vec2(tubeDiam/2., tubeLen2));
    // Bands around the tube
    tube2-= ch*smoothstep(cw, 0., abs(0.2 - mod(angle2, pi)));
    chimney = min(chimney, tube2);
 
    // Broad conic top of the chimney
    float cone1 = sdConeSection(pos.yxz + vec3(tubeLen1 + tubeclen, -tubeclen - tubeLen2 - tubeLen3, 0), tubeLen3/2., tubeDiam/2.6, tubeDiam2/2.23);
    float cone2 = sdConeSection(pos.yxz + vec3(tubeLen1 + tubeclen, -tubeclen - tubeLen2 - tubeLen3*2. - 0.08, 0), tubeLen3/2., tubeDiam2/2.23, tubeDiam/2.56);
    float cone = smin(cone1, cone2, 0.2);
    chimney = smin(chimney, cone, 0.1);
    
    // Small "collar" at the base of the broad part 
    float collar1 = sdCylinder(pos.yxz + vec3(tubeLen1 + tubeclen, -tubeclen - tubeLen2 - 0.6, 0), vec2(tubeDiam/1.85, 0.065));
    chimney = smin(chimney, collar1, 0.15);
    
    // Small rounding at the middle of the broad part  
    float rborder = sdTorus(pos.yxz + vec3(tubeLen1 + tubeclen, -tubeclen - tubeLen2 - tubeLen3 - 0.5, 0.), vec2(tubeDiam2/1.8, 0.01));
    chimney = smin(chimney, rborder, 0.2);
    
    // Inside of the chimney (hole of the chimney)
    float tubeInt = getTubeInt(pos);
    chimney = max(chimney, -tubeInt);
    
    return vec2(chimney, CHIMNEY_OBJ);
}
#endif

// Main distance mapping function
vec2 map(vec3 pos)
{
    vec2 wall = vec2(sdBox(pos - wallPos, wallSize), WALL_OBJ);
    float wallBound = sdBox(pos - wallPos, wallSize*1.04);
    vec2 bricks = max(map_bricks(pos), wallBound);
    #ifdef show_chimney
    vec2 chimney = map_chimney(pos);
    return opU(opU(wall, bricks), chimney);
    #else
    return opU(wall, bricks);
    #endif
}

// Simple mapping function of the wall
#ifdef show_chimney
vec2 map_s(vec3 pos)
{
    vec2 wall = vec2(sdBox(pos-wallPos, wallSize), BRICKS_OBJ);
    
    return wall;
}

// Mapping of the inside of the hole (for smoke mapping)                            
float map_int(vec3 pos)
{
    pos = pos.yxz;
    pos-= chimneyOrig;
    
    return getTubeInt(pos);
}
#endif

// Main tracing function
vec2 trace(vec3 cam, vec3 ray, float maxdist) 
{
    float t = 1.;
    float objnr = 0.;
    vec3 pos;
    float dist;
    
  	for (int i = 0; i < 90; ++i)
    {
    	pos = ray*t + cam;
        vec2 res = map(pos);
        dist = map(pos).x;
        if (dist>maxdist || abs(dist)<0.0004)
            break;
        t+= dist*1.;
        objnr = abs(res.y);
  	}
  	return vec2(t, objnr);
}

// Gets the position of the wall
#ifdef show_chimney
vec2 trace_s(vec3 cam, vec3 ray, float maxdist) 
{
    float t = max((-cam.x - wallPos.x + wallSize.x)/ray.x, maxdist);
    float objnr = 0.;
    vec3 pos;
    float dist;
  
    pos = ray*t + cam;
    vec2 res = map_s(pos);
    dist = res.x;
    t+= dist;
    objnr = abs(res.y);

  	return vec2(t, objnr);
}
#endif

// From https://www.shadertoy.com/view/MstGDM
// Normal mapping function
vec3 getNormal(vec3 pos, float e)
{  
    vec2 q = vec2(0, e);
    return normalize(vec3(map(pos + q.yxx).x - map(pos - q.yxx).x,
                          map(pos + q.xyx).x - map(pos - q.xyx).x,
                          map(pos + q.xxy).x - map(pos - q.xxy).x));
}

// Normal mapping function, simplified version
#ifdef show_chimney
vec3 getNormal_s(vec3 pos, float e)
{  
    vec2 q = vec2(0, e);
    return normalize(vec3(map_s(pos + q.yxx).x - map_s(pos - q.yxx).x,
                          map_s(pos + q.xyx).x - map_s(pos - q.xyx).x,
                          map_s(pos + q.xxy).x - map_s(pos - q.xxy).x));
}
#endif

// Gets the flares of the lamps (kind of non-reflective specular)
vec3 getFlares(vec3 ray)
{
	vec3 rc = vec3(0.);
    rc+= 0.4*clamp(normalize(sunLamp.color)*sunLamp.intensity*pow(max(0.0, dot(ray, normalize(sunLamp.direction))), 300.), 0., 1.);

    return rc;
}

// Gets the color of the sky
vec3 sky_color(vec3 ray)
{
    float elev = atan(ray.y);
    float azimuth = atan(ray.x, ray.z);
 
    vec3 sky = ambientColor + vec3(0.4, 0.22, 0.05)*2.5*(0.65 - elev);
    
    // Clouds
    #ifdef procedural_clouds
    float cloudst = smoothstep(-0.2, 0.5, elev)*smoothstep(0.1, 0.97, noise2(11.*cloudSize*ray + vec3(cloudSpeed*currTime)));
    #else
    float cloudst = smoothstep(-0.2, 0.5, elev)*texture2D(iChannel1, cloudsize*ray.xy).r;
    #endif
    sky = mix(sky, 0.45 + 0.6*vec3(cloudst), smoothstep(0.12, 0.5, cloudst)) + 0.3*vec3(smoothstep(0.2, 0.8, cloudst));
    
    // Ground
    vec3 grass = vec3(0.05, 0.45, 0.3) + vec3(0.19, 0.13, -0.03)*2.7*(0.65 - elev);
    grass = grass*(0.6 + 2.*abs(elev)*texture2D(iChannel0, 12.*ray.xy).r);
    
    return mix(mix(grass, vec3(0.65)*(0.7 + 0.3*texture2D(iChannel0, 12.*ray.xy).r), smoothstep(-0.17 - 0.035*abs(azimuth), -0.172 - 0.035*abs(azimuth), elev)), sky, smoothstep(-0.0003, 0.0003, elev)) + getFlares(ray); 
}

// Gets the simplified version of the color of the sky
vec3 sky_color_s(vec3 ray)
{
    float elev = atan(ray.y);
 
    const float cloudsize = 0.25;
    vec3 sky = ambientColor + vec3(0.4, 0.22, 0.05)*2.5*(1. - elev);
    
    vec3 grass = vec3(0.0, 0.35, 0.25) + vec3(0.22, 0.16, -0.03)*2.8*(0.65 - elev);
    
    return mix(mix(grass, vec3(0.65), smoothstep(-0.31, -0.312, elev)), sky, smoothstep(-0.0003, 0.0003, elev)); 
}

// Get the color of the cement parts between the bricks
vec3 getWallColor(vec3 pos)
{
    return vec3(0.3 + 0.7*texture2D(iChannel0, 0.9*pos.yz).r)*(0.5 + 0.5*smoothstep(0., 0.12, texture2D(iChannel3, 0.3*pos.yz).r));
}

// Get the color of the metallic chimney
vec3 getChimneyColor(vec3 pos, vec3 norm)
{
    vec3 chcol = vec3(1.25) - 0.7*texture2D(iChannel2, 0.0007*(5.*pos.xy + cross(norm, pos).yz + cross(pos, norm).zx)).x;
    
    return chcol;
}

// Get the color of the bricks
vec3 getBrickColor(vec3 pos)
{
    vec3 brickCol1 = vec3(0.60, 0.27, 0.15);
    vec3 brickCol2 = vec3(0.45, 0.24, 0.13);
    vec3 brickCol3 = vec3(0.29, 0.10, 0.04);
    
    vec3 pos2 = pos;
    pos2.yz+= 0.07*texture2D(iChannel1, pos.yz*0.005).g;
    pos2.z+= 0.5*brickStep.z*floor(0.5*pos2.y/brickStep.y);
    vec2 nb = floor(pos2.yz/brickStep.yz*vec2(0.5, 1.));
    float nbBrick = nb.x + nb.y*90.;
    float nbBrickf = pow(hash(nbBrick), 6.);
    vec3 brickCol = mix(brickCol1, brickCol2, nbBrickf);
    brickCol = mix(brickCol, brickCol3, pow(smoothstep(0.1, 1.05, 1.2*texture2D(iChannel2, pos.yz*0.18).x*texture2D(iChannel2, pos.yz*0.23).x), 1.5));
    brickCol*= 0.4 + 0.6*smoothstep(0.80, 0.60, texture2D(iChannel1, pos.yz*0.07).b);
    return brickCol;
}

vec3 getBrickColor_s(vec3 pos)
{
    return vec3(0.9, 0.3, 0.05);
}

// Combines the colors
vec3 getColor(vec3 norm, vec3 pos, int objnr)
{
   vec3 col = objnr==WALL_OBJ?getWallColor(pos):(
              objnr==BRICKS_OBJ?getBrickColor(pos):(
              objnr==CHIMNEY_OBJ?getChimneyColor(pos, norm):sky_color(pos)));

   return col;
}

// Get the simplified color of the wall (basic brick color)
vec3 getColor_s(vec3 norm, vec3 pos, int objnr)
{
   vec3 col = getBrickColor_s(pos);
   return col;
}

////////////////////////////////////////////////////////
// Smoke functions from www.shadertoy.com/view/XsX3RB //
////////////////////////////////////////////////////////

#ifdef show_smoke
vec3 rayRef;
// Mapping of the smoke
vec4 mapSmoke(in vec3 pos)
{
    vec3 pos2 = pos;
    pos2-= chimneyOrig + vec3(5.65, -0.8, 0.);
    
    // Calculating the smoke domain (3D space giving the probability to have smoke inside
    float sw = max(tubeDiam*0.84 + 0.25*pos2.y*(1. + max(0.15*pos2.y, 0.)) + 0.2*windIntensity*(pos.y + chimneyOrig.x - tubeclen - tubeLen2 + 0.3), 0.);
    float smokeDomain = smoothstep(1.2 + sw/4.3, 0.7 - sw*0.5, length(pos2.xz)/sw);
    
    float d;
    vec4 res;
    if (smokeDomain>0.1)
    {           
    	// Space modification in function of the time and wind
        vec3 q = pos2*vec3(1., 1. + 0.5*windIntensity, 1.) + vec3(0.0,-currTime*smokeSpeed + 10.,0.0);
    	q/= smokeScale;
        q.y+= 8.*dWindIntensity + 1.5/(0.7 + dWindIntensity);
        
        // Turbulence of the smoke
        #ifdef smoke_turbulence
        if (smokeTurbulence>0.)
        {
        	float n = smoothstep(4., 0., pos2.y + 3.2)*smokeTurbulence*noise(q*smokeTurbulenceScale)/(currTime + 3.);
        	q.xy = rotateVec(-q.xy, pos.z*n);
        	q.yz = rotateVec(-q.yz, pos.x*n);
        	q.zx = rotateVec(-q.zx, pos.y*n);
        }
        #endif
        
        // Calculation of the noise
        d = clamp(0.6000*noise(q), 0.4, 1.); q = q*2.02;  
        d+= 0.2500*noise(q); q = q*2.03;
        d+= 0.1200*noise(q); q = q*2.08;
        d+= 0.0500*noise(q);
        
        #ifdef heat_refraction
        // Calculation of the refraction due to the temperature difference in the air
        float rrf = smokeDomain*(1. - clamp((pos2.y + 2.8)*0.55, 0., 1.))*smoothstep(0., .3, pos2.y + 3.2);
        rayRef.x+= (smokeRefInt*noise(q*3.27 + q*4.12) - 0.5*smokeRefInt)*rrf;
        rayRef.y+= (smokeRefInt*noise(q*3.37 - q*3.96) - 0.5*smokeRefInt)*rrf;
        rayRef.z+= (smokeRefInt*noise(q*3.11 + q*3.82) - 0.5*smokeRefInt)*rrf;
        #endif

        d = d - 0.3 - smokeBias - 0.04*pos.y + 0.05*(1. + windIntensity);
        d = clamp(d, 0.0, 1.0);
        
 		res = vec4(pow(d*smokeDomain, smokePow));

    	// Some modifications of color and alpha
		res.xyz = mix(smokeCol, 0.2*vec3(0.4, 0.4, 0.4), res.x);
		res.xyz*= 0.2 + 0.2*smoothstep(-2.0, 1.0, pos.y);
    	res.w*= max(smokeDens - 1.8*sqrt(pos.y - 4.), 0.);
    }
    else
    {
        d = 0.;
        res = vec4(0.);
    }
	
	return res;
}

// Raymarching of the smoke
vec4 raymarchSmoke(in vec3 ro, in vec3 rd, in vec3 bcol, float tmax, bool isShadow)
{
	vec4 sum = vec4(0.0);
    vec2 windDir = rotateVec(vec2(1., 0.), windAngle);

	float t = isShadow?5.4:abs(0.95*(campos.z - chimneyOrig.z)/rd.z);
	for(int i=0; i<32; i++)
	{
		if(t>tmax || sum.w>1.) break;
		vec3 pos = ro + t*rd;
        
        // Influence of the wind
        pos.xz+= windDir*windIntensity*(pos.y + chimneyOrig.x - tubeclen - tubeLen2 + 0.3);

		vec4 col = mapSmoke(pos);
		
        if (col != vec4(0.))
        {
        	// Color modifications of the smoke
        	col.rgb+= (1. - smokeColPresence)*(1.0 - col.w);
			col.rgb = mix(col.rgb, bcol, smoothstep(smokeColBias, 0.0, col.w));
			col.rgb*= col.a;

			sum = sum + col*(1.0 - sum.a);
        }
		t+= 0.07*(1. + windIntensity)*max(0.1,0.05*t);
	}
	sum.rgb/= (0.001 + sum.a);
	return clamp(sum, 0.0, 1.0);
}
#endif

////////////////////////////////////////////////////////////////

vec4 smokeRes;
// From https://www.shadertoy.com/view/Xds3zN, but I changed the code
float softshadow(vec3 ro, vec3 rd, float mint, float tmax)
{
	float res = 1.0;
    float t = mint;
    for(int i=0; i<12; i++)
    {
    	float h = map(ro + rd*t).x;
        res = min(res, 10.0*h/t + 0.02*float(i));
        t+= 1.5*clamp(h, 0.01, 0.5);
        if(h<0.001 || t>tmax) break;
    }
    #ifdef show_smoke
    vec4 rsr = raymarchSmoke(ro, rd, vec3(1.), 6., true);
    return clamp(min(res, 1. - rsr.a*0.9), 0.0, 1.0);
    #else
    return clamp(res, 0.0, 1.0);
    #endif
}

// From https://www.shadertoy.com/view/Xds3zN
float calcAO(in vec3 pos, in vec3 nor)
{
	float occ = 0.0;
    float sca = 1.0;
    for(int i=0; i<5; i++)
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor*hr + pos;
        float dd = map(aopos).x;
        occ+= -(dd - hr)*sca;
        sca*= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 );    
}

// Fresnel reflectance factor through Schlick's approximation: https://en.wikipedia.org/wiki/Schlick's_approximation
float fresnel(vec3 ray, vec3 norm, float n2)
{
   float n1 = 1.; // air
   float angle = acos(-dot(ray, norm));
   float r0 = dot((n1 - n2)/(n1 + n2), (n1 - n2)/(n1 + n2));
   float r = r0 + (1. - r0)*pow(1. - cos(angle), 5.);
   return r;
}

// Shading of the objects
vec3 lampShading(DirLamp lamp, vec3 norm, vec3 pos, vec3 ocol, int objnr, bool simple)
{
	vec3 pl = normalize(lamp.direction);
      
    // Diffuse shading
    vec3 col;
    if (objnr==CHIMNEY_OBJ)
        col = ocol*lamp.color*lamp.intensity*(0.1 + 0.75*pow(clamp(dot(norm, pl), 0., 0.93), 1.8));
    else
        col = ocol*lamp.color*lamp.intensity*clamp(dot(norm, pl), 0., 1.);
    
    // Specular shading
    #ifdef specular
    float specint2 = objnr==CHIMNEY_OBJ?specint[CHIMNEY_OBJ]:specint[BRICKS_OBJ];
    float specshin2 = objnr==CHIMNEY_OBJ?specshin[CHIMNEY_OBJ]:specshin[BRICKS_OBJ];
    if (dot(norm, lamp.direction) > 0.)
        col+= lamp.color*lamp.intensity*specint2*pow(max(0.0, dot(reflect(pl, norm), normalize(pos - campos))), specshin2);
    #endif
    
    // Softshadow
    #ifdef shadow
    if (!simple)
        col*= shi*softshadow(pos, pl, shf, 7.) + 1. - shi;
    #endif
    
    return col;
}

// Sets the position of the camera with the mouse and calculates its direction
void setCamera()
{
   vec2 iMouse2;
   if (iMouse.x==0. && iMouse.y==0.)
      iMouse2 = iResolution.xy*vec2(0.52, 0.65);
   else
      iMouse2 = iMouse.xy;
   
   campos = vec3(19.*(1. - iMouse2.x/iResolution.x - 0.5),
                 18.*(iMouse2.y/iResolution.y - 0.35),
                 10.);
   /*campos = camtarget + vec3(-20.*cos(3. + 6.*iMouse2.x/iResolution.x),
                 20.*cos(3.*iMouse2.y/iResolution.y),
                 20.*sin(3. + 6.*iMouse2.x/iResolution.x)*sin(3.*iMouse2.y/iResolution.y)); */
    
   camdir = camtarget-campos;   
}

// From https://www.shadertoy.com/view/lsSXzD, modified
vec3 GetCameraRayDir(vec2 vWindow, vec3 vCameraDir, float fov)
{
	vec3 vForward = normalize(vCameraDir);
	vec3 vRight = normalize(cross(vec3(0., 1., 0.), vForward));
	vec3 vUp = normalize(cross(vForward, vRight));
    
	vec3 vDir = normalize(vWindow.x*vRight + vWindow.y*vUp + vForward*fov);
	return vDir;
}

// Tracing and rendering a ray
RenderData trace0(vec3 tpos, vec3 ray, float maxdist, bool simple)
{
    #ifdef show_chimney
    vec2 tr = simple?trace_s(tpos, ray, maxdist):trace(tpos, ray, maxdist);
    #else
    vec2 tr = trace(tpos, ray, maxdist);
    #endif
    float tx = tr.x;
    int objnr = int(tr.y);
    vec3 col;
    vec3 pos = tpos + tx*ray;
    vec3 norm;
    
    if (tx<maxdist)
    {
        #ifdef show_chimney
        norm = simple?getNormal_s(pos, normdelta):getNormal(pos, normdelta);
        col = simple?getColor_s(norm, pos, objnr):getColor(norm, pos, objnr);
        #else
        norm = getNormal(pos, normdelta);
        col = getColor(norm, pos, objnr);        
        #endif
        
        // Ambient occlusion
        #ifdef ambocc
        float aofact;
        if (!simple)
           aofact = 1. - aoint + aoint*calcAO(pos, norm);
        else
           aofact = 1.;
        #else
        float aofact = 1.;
        #endif
        
        // Shading
        col = ambientColor*ambientint*aofact + lampShading(sunLamp, norm, pos, col, objnr, simple)*(0.6 + 0.4*aofact);
    }
    else
    {
        // Shows the color of the sky
        objnr = SKY_OBJ;
        col = simple?sky_color_s(ray):sky_color(ray);
    }
    return RenderData(col, pos, norm, objnr);
}

// Function to get the intensity of the wind as a function 
float getWindIntensity(float t)
{
    return maxWindIntensity*smoothstep(-0.3, 2.2, sin(t*0.153 + 18.) + 0.63*sin(t*0.716 - 7.3) + 0.26*sin(t*1.184 + 87.));
}
// Function to get the angle of the wind as a function 
float getWindAngle(float t)
{
    return maxWindAngle*pi*(sin(t*0.0117) + 0.67*sin(t*0.0672 + 5.6) + 0.26*sin(t*0.1943 - 18.7));
}

// Main render function with diffuse reflections on the chimney
vec3 ray;
int robjnr;
bool isIns;
vec3 render(vec2 xlv_TEXCOORD0, bool calcSmoke)
{   
    // The specular is different between the bricks and chimney
    specint[BRICKS_OBJ] = 0.15;
    specshin[BRICKS_OBJ] = 0.8;
    specint[CHIMNEY_OBJ] = 0.95;
    specshin[CHIMNEY_OBJ] = 15.;
    
    vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy; 
    uv = uv*2.0 - 1.0;
    uv.x*= iResolution.x / iResolution.y;

    ray = GetCameraRayDir(uv, camdir, fov);

    // Modification of the ray because of the air refraction due to the heat
    #ifdef show_smoke
    rayRef = vec3(0.);
    smokeRes = raymarchSmoke(campos, ray, vec3(1.), 40., false);
    #ifdef heat_refraction
    ray+= rayRef;
    #endif
    #endif
    
    RenderData traceinf = trace0(campos, ray, maxdist, false);
    vec3 pos = traceinf.pos;
    vec3 col = traceinf.col;
    robjnr = traceinf.objnr;
    
    #ifdef reflections
    vec3 refray = reflect(ray, traceinf.norm);
    if (robjnr==CHIMNEY_OBJ)
    {   
        vec3 cols = vec3(0.);
        for (float r=0.; r<nbrref ;r++)
        {
           vec3 randpart = hash3(vec3(1.)*(1.74 + r*21.12 + aaindex) + 80.*traceinf.pos);
           vec3 refray2 = refray + rrefblur*(randpart - vec3(0.5));
           RenderData traceinf_ref = trace0(traceinf.pos, refray2, 20., true);
           cols+= traceinf_ref.col;   
        }
        float r = clamp(fresnel(ray, traceinf.norm, 2.9), 0., 0.45);
        col = mix(col, cols/nbrref, r);
    }
    #endif
    
    // Sets a variable if the smoke is inside the chimney
    #ifdef show_smoke
    isIns = map_int(pos)<0.04;
    #endif
    
    return col;
}

// Main tracing function
void main( )
{
    currTime = iTime*timeFactor;
    setCamera();
    windIntensity = getWindIntensity(currTime*0.8);
    dWindIntensity = windIntensity - getWindIntensity(currTime*0.8 - 0.2);
    windAngle = getWindAngle(currTime);
    
    // Antialiasing loop
    vec3 vs = vec3(0.);
    for (int j=0; j<aasamples ;j++)
    {
       float oy = float(j)*aawidth/max(float(aasamples-1), 1.);
       for (int i=0; i<aasamples ;i++)
       {
          float ox = float(i)*aawidth/max(float(aasamples-1), 1.);
          vs+= render(xlv_TEXCOORD0 + vec2(ox, oy), i + j==0);
          aaindex = 1.23*float(i) + 18.79*float(j);
       }
    }
    vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;

    vec3 col = vs/vec3(aasamples*aasamples);
    
    // Adds the smoke to the image
    #ifdef show_smoke
    if (robjnr!=CHIMNEY_OBJ || isIns)
        col = mix(col, smokeRes.xyz, smokeRes.w)*(1. + 0.06*smokeRes.w);
    #endif
    
    gl_FragColor = vec4(col, 1.);
}
