/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
// https://www.shadertoy.com/view/XsVSDW
// Flame thrower
float mod(float x, float y)
{
  return x - y * floor(x / y);
}


float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}
float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
float iTime=0.0;
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;
/*
"Flame thrower" by Emmanuel Keller aka Tambako - June 2016
License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
Contact: tamby@tambako.ch
*/

// ToDo
// * Variation power flame thrower with time
// * Better tex2D/color tube (bump)
// * Amb Occ
// * Problem bump slow
// * Lamp(s) for flame thrower
// * Heat refraction
// - Speed/compile optimisations

#define pi 3.14159265359

// Switches, you can play with them, but be careful, some options can crash your browser!
#define specular
#define reflections
#define ambocc
//#define test_ambocc
#define bump_maps
#define show_floor
#define show_wall
#define show_tube
#define show_flame
#define flame_a_turb
#define heat_refraction
//#define antialias

float3 Lamp_position[3];
float3 Lamp_color[3];
float Lamp_intensity[3];
float Lamp_attenuation[3];

float3 RenderData_col;
float3 RenderData_pos;
float3 RenderData_norm;
int RenderData_objnr;

float3 RenderData__col;
float3 RenderData__pos;
float3 RenderData__norm;
int RenderData__objnr;
   
// Every object of the scene has its ID
#define SKY_OBJ        0
#define FLAME_OBJ      1
#define TUBE_OBJ       2
#define FLOOR_OBJ      3
#define WALL_1_OBJ     4
#define WALL_2_OBJ     5

//Lamp lamps[3];

// Campera options
float3 campos = float3(0.5, -0.4, 10.);
float3 camtarget = float3(0., 0.7, 0.);
float3 camdir;
float fov = 4.5;
float angle;
float angle2;

// Ambient light
float3 ambientColor = float3(0.3, 0.3, 0.3);
float ambientint = 0.;

// Color options
float3 flameColor = float3(1., 0.68, 0.32);
float3 tubeColor = float3(0.3, 0.32, 0.42);
float3 floorColor = float3(0.5, 0.5, 0.54);
float3 wallColor = float3(0.3, 0.3, 0.34);

// Shading options
float specint = 0.14;
float specshin  = 5.;
float aoint = 0.42;
    
// Tracing options
float normdelta = 0.001;
float maxdist = 30.;

// Antialias. Change from 1 to 2 or more AT YOUR OWN RISK! It may CRASH your browser while compiling!
float aawidth = 0.8;
int aasamples = 2;

float tubeDiameter = 0.14;
float tubeLenght = 0.8;
float3 flamePos = float3(0.05, 1.1, 1.5);

bool traceFlame = true;

float flameVar;

float textureScale = 0.3;
float rms;
float rmb = 0.8;

void init()
{
    // The lamps
    lamps[0] = Lamp(float3(-1.5, 3., 1.), float3(1., 1., 1.), 1.2, 0.01); // 1.2 // ???
    lamps[1] = Lamp(float3(-5., 12., 15.), float3(0.65, 0.75, 1.), 1.05, 0.01); // 0.9 // ???
    lamps[2] = Lamp(flamePos*2., 1.2*flameColor, 0., 0.01); // ???
    
    rms = 0.28 + 0.003*iTime;
    
    // Power of the flame in function of the time
    flameVar = sin(iTime*0.55) + 0.56*sin(iTime*0.134) + 0.22*sin(iTime*0.095);
}

// Union operation from iq
float2 opU(float2 d1, float2 d2)
{
	return (d1.x<d2.x) ? d1 : d2;
}

float2 rotateVec(float2 vect, float angle)
{
    float2 rv;
    rv.x = vect.x*cos(angle) - vect.y*sin(angle);
    rv.y = vect.x*sin(angle) + vect.y*cos(angle);
    return rv;
}

float3 rotateVec2(float3 posr)
{
    posr = float3(posr.x, posr.y*cos(angle2) + posr.z*sin(angle2), posr.y*sin(angle2) - posr.z*cos(angle2));
    posr = float3(posr.x*cos(angle) + posr.z*sin(angle), posr.y, posr.x*sin(angle) - posr.z*cos(angle)); 
    
    return posr;
}

// 1D hash function
float hash(float n)
{
    return frac(sin(n)*753.5453123);
}

// From https://www.shadertoy.com/view/4sfGzS
float noise_(float3 x)
{
    //x.x = mod(x.x, 0.4);
    float3 p = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return lerp(lerp(lerp(hash(n+  0.0), hash(n+  1.0),f.x),
                   lerp(hash(n+157.0), hash(n+158.0),f.x),f.y),
               lerp(lerp(hash(n+113.0), hash(n+114.0),f.x),
                   lerp(hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

// From https://www.shadertoy.com/view/Xds3zN
float sdCylinder(float3 p, float2 h)
{
    float2 d = abs(float2(length(p.xz),p.y)) - h;
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

// Distance mapping function of the tube where the flame comes
float map_tube(float3 pos)
{
    pos-= flamePos;
    pos.x+= tubeLenght;
    pos = pos.yxz;
    
    float df = sdCylinder(pos, float2(tubeDiameter, tubeLenght));
    df+= -0.006 + 0.003*noise_(pos*float3(70., 20., 70.));
    df = max(df, -sdCylinder(pos, float2(tubeDiameter - 0.02, tubeLenght + 0.02)));
    return df;
}

// Gets the height of the water on the floor
float waterPos;
float getWaterPos(float3 pos)
{
    float3 pos2 = pos + float3(0.3*noise_(pos*4.2),
                          0.27*noise_(pos*6.5),
                          0.32*noise_(pos*3.8))
                      + float3 (7., 0.5, 12.);
    return noise_(pos2*1.7) - 0.1 
        + 1.2*smoothstep(-0.3, 0.2, pos.x) 
        + 1.4*smoothstep(0.3, -0.2, pos.z);
    return 0.;
}

// Get the bumpmap of the floor/walls
float getBumps(float3 pos, int objnr)
{
    #ifdef bump_maps
    if (objnr==FLOOR_OBJ)
    {
    	float a = 0.8*smoothstep(rms + 0.2, rms, waterPos);
    	return 0.025*(tex2D(iChannel0, pos.xz*textureScale + float2(0.55, 0.)).r*a - 0.55*a);
    }
    else if (objnr==WALL_1_OBJ)
        return 0.01*tex2D(iChannel0, pos.yz*textureScale).r;
    else if (objnr==WALL_1_OBJ)
        return 0.01*tex2D(iChannel0, pos.xy*textureScale).r;
    else
        return 0.;
    #else
    return 0.;
    #endif    
}

// Distance mapping function of the floor
float map_floor(float3 pos)
{
    return pos.y;
}

// Distance mapping function of the wall on the left
float map_wall_1(float3 pos)
{
    return -pos.x;
}

// Distance mapping function of the wall behind
float map_wall_2(float3 pos)
{
    return pos.z;
}

// Flame parameters
float flameIntensity = 0.4;
float flameStep = 0.07;
float flamePow = 1.5;
float flameBias = 0.;
float faf = 0.18;
float fd0 = -0.04;
float fg = 0.02;
float dnf = -0.25;
float dnx = 0.26;
float dns = 40.;
float fdx = 0.2;
float fts = 6.;
float ftf = 1.1;
float ft;

float fcp = 1.5;

// Simplified mapping function of the flame
float map_flame_s(float3 pos)
{
   
    float3 q = pos*0.6;
    q*= float3(1., 1.5, 1.);
    q+= float3(ft, 0., 0.);
    float dn = 0.5*(dnf - dnx*pos.x);
    pos.x+= dn*noise_(q + float3(12., 3. + ft, 16.)) - dn/2.;
    pos.y+= dn*noise_(q + float3(14., 7., 20.)) - dn/2.;
    pos.z+= dn*noise_(q + float3(8., 22., 9.)) - dn/2.;

    float df = length(pos.yz) + 0.8*pos.x + 2.;
    
    return df;
}

// Main mapping function of the flame
float map_flame(float3 pos, bool turb, bool bounding)
{  
    #ifdef show_flame
    if (!traceFlame && bounding)
        return 10.;
    
    ft = iTime*dns;
    
    pos-= flamePos;
    pos.x+= tubeLenght - 0.33;
    pos.y+= pos.x*pos.x*fg - fg;
 
    float3 q = pos*fts;
    
    if (turb)
    {
        #ifdef flame_a_turb
        float n = 0.07*noise_(q*0.6);
        q.xy = rotateVec(-q.xy, pos.z*n);
    	q.yz = rotateVec(-q.yz, pos.x*n);
    	q.zx = rotateVec(-q.zx, pos.y*n);
    	#endif
        
    	q*= float3(1., 1.5, 1.);
        q+= float3(ft, 0., 0.);
    	q.x+= 0.5*pos.x*noise_(q + float3(30., 40., 50. + ft));
    	q.y+= 0.5*pos.x*noise_(q + float3(10., 30. + ft, 20.));
    	q.z+= 0.5*pos.x*noise_(q + float3(20., 60. - ft, 40. - ft));
 
    	float dn = (dnf - dnx*pos.x);
    	pos.x+= dn*noise_(q + float3(12., 3.+ ft, 16. - ft)) - dn/2.;
    	pos.y+= dn*noise_(q + float3(14., 7., 20.)) - dn/2.;
    	pos.z+= dn*noise_(q + float3(8. + ft*0.3, 22., 9.)) - dn/2.;
    }        
    
    float df = length(pos.yz) + faf*pos.x + fd0;
    
    if (bounding)
    {
        df-= 0.5*smoothstep(-1.1, -4., pos.x);
        df = lerp(df, sdCylinder(pos.yxz + float3(0., 1., 0.), float2(tubeDiameter - 0.01, tubeLenght*2.)), smoothstep(-1.3, -1.12, pos.x));   
    }
    else
        df = lerp(df, sdCylinder(pos.yxz + float3(0., 1., 0.), float2(tubeDiameter + 0.01, tubeLenght*2.)), smoothstep(-1.5, -1.12, pos.x));   

    return df;
    #else
    return 10.;
    #endif
}

// Main mapping function
float2 map(float3 pos)
{
    float2 res;

    float tube = map_tube(pos);
    float ffloor = map_floor(pos);
    float wall1 = map_wall_1(pos);
    float wall2 = map_wall_2(pos);
    float flame = map_flame(pos, false, true);
    
    res = float2(tube, TUBE_OBJ);
    res = opU(float2(ffloor, FLOOR_OBJ), res);
    res = opU(float2(wall1, WALL_1_OBJ), res);
    res = opU(float2(wall2, WALL_2_OBJ), res);
    //res = opU(float2(flame, TUBE_OBJ), res);
    res = opU(float2(flame, FLAME_OBJ), res);

    return res;
}

// Main tracing function
float2 trace(float3 cam, float3 ray, float maxdist) 
{
    float t = 0.02;
    float objnr = 0.;
    float3 pos;
    float dist;
    float dist2;
    
  	for (int i = 0; i < 76; ++i)
    {
    	pos = ray*t + cam;
        float2 res = map(pos);
        dist = res.x;
        if (dist>maxdist || abs(dist)<0.002)
            break;
        t+= dist*0.75;
        objnr = abs(res.y);
  	}
  	return float2(t, objnr);
}

// Gets the normal at specified position
float3 getNormal(float3 pos, float e, int objnr)
{  
    float2 q = float2(0, e);
    float3 b = float3(map(pos + q.yxx).x - map(pos - q.yxx).x,
                  map(pos + q.xyx).x - map(pos - q.xyx).x,
                  map(pos + q.xxy).x - map(pos - q.xxy).x);
    
    waterPos = getWaterPos(pos);
    b+= float3(getBumps(pos + q.yxx, objnr) - getBumps(pos - q.yxx, objnr),
             getBumps(pos + q.xyx, objnr) - getBumps(pos - q.xyx, objnr),
             getBumps(pos + q.xxy, objnr) - getBumps(pos - q.xxy, objnr));
        
        
    
    return normalize(b);
}

// Gets the color of the sky
float3 sky_color(float3 ray)
{ 
    return float3(0.);
}

// Gets the color of the tube
float3 getTubeColor(float3 pos)
{
    return tubeColor*tex2D(iChannel2, float2(pos.x, 2.*atan(pos.y, pos.z))).rrr;
}

// Gets the color of the floor
float3 getFloorColor(float3 pos)
{
    return floorColor*lerp(tex2D(iChannel0, pos.xz*textureScale + float2(0.55, 0)).rgb, 
                          tex2D(iChannel0, pos.xz*textureScale + float2(0.55, 0)).rrr, 0.8);
}

// Gets the color of the wall on the left
float3 getWallColor1(float3 pos)
{
    return wallColor*lerp(tex2D(iChannel0, pos.yz*textureScale).rgb, 
                         tex2D(iChannel0, pos.yz*textureScale).rrr, 0.6);
}

// Gets the color of the wall behind
float3 getWallColor2(float3 pos)
{
    return wallColor*lerp(tex2D(iChannel0, pos.xy*textureScale).rgb, 
                         tex2D(iChannel0, pos.xy*textureScale).rrr, 0.6);
}

// Gets the reflection factor of the floor in function of the position
float getFloorReflection(float3 pos)
{
    return 0.85*smoothstep(rms - 0.1, rms + 0.25, getWaterPos(pos));
}

// Gets the color of the flame
float3 getFlameColor(float3 pos)
{
    return flameColor;
}

// Gets the color of the flame depending on the position and ray direction by volume sampling
float3 getFlameDensColor(float3 pos, float3 ray, float s, float fi, int nbSteps)
{
    float d = 1.;
    float f;
    float3 scol = float3(0.);
    for (int i=0; i<70; i++)
    {
    	if (i==nbSteps)
            break;
        pos+= ray*s;
        f = -map_flame(pos, true, false);
        f = sign(f)*pow(abs(f), flamePow);
    	d = clamp(f + flameBias, 0., 10.);
        d*= smoothstep(-7. - 0.8*flameVar, -4. - 0.3*flameVar, pos.x)*smoothstep(-1.3, -1.5, pos.x)*(3. + 0.4*pos.x);
        d*= (0.7 + 20./(pow(abs(pos.x), 3.) + 1.3));
        d*= 1. + 14.*smoothstep(-1.88, -1.2, pos.x);
        scol+= d*getFlameColor(pos);
    }
    
    return clamp(scol*fi, 0., 1.5);
}
    
// Combines the colors
float3 getColor(float3 norm, float3 pos, int objnr, float3 ray)
{
   pos = rotateVec2(pos);
   return (objnr==TUBE_OBJ?getTubeColor(pos):
          (objnr==FLOOR_OBJ?getFloorColor(pos):
          (objnr==WALL_1_OBJ?getWallColor1(pos):
          (objnr==WALL_2_OBJ?getWallColor2(pos):
           float3(0.)))));
}

// From https://www.shadertoy.com/view/Xds3zN
float calcAO(in float3 pos, in float3 nor)
{
	float occ = 0.0;
    float sca = 1.1;
    for(int i=0; i<10; i++)
    {
        float hr = 0.06 + 0.3*float(i)/4.0;
        float3 aopos =  nor*hr + pos;
        float dd = map(aopos).x;
        occ+= -(dd - hr)*sca;
        sca*= 1.3;
    }
    //occ = 2.*smoothstep(0.06, 0.5, occ);
    return clamp( 1.0 - 0.03*occ, 0.0, 1.0 );    
}

// Fresnel reflectance factor through Schlick's approximation: https://en.wikipedia.org/wiki/Schlick's_approximation
float fresnel(float3 ray, float3 norm, float n2)
{
   float n1 = 1.; // air
   float angle = acos(-dot(ray, norm));
   float r0 = dot((n1-n2)/(n1 + n2), (n1 - n2)/(n1 + n2));
   float r = r0 + (1. - r0)*pow(1. - cos(angle), 5.);
   return r;
}

// Shading of the objects pro lamp
float3 lampShading(Lamp lamp, float3 norm, float3 pos, float3 ocol, int objnr, int lampnr)
{     
    float3 lp = lamp.position;
    float li = lamp.intensity;
    float3 lc = lamp.color;
    if (lampnr==2)
    {
        lp = pos + norm;
        #ifdef show_flame
        li = 1.2*(1. + 0.3*flameVar)*clamp(3.*(1. - 0.25*map_flame_s(pos)), 0., 5.);
        #endif
    }
    
    float3 pl = normalize(lp - pos);
    float dlp = distance(lp, pos);
    float3 pli = pl/pow(1. + lamp.attenuation*dlp, 2.);
    float dnp = dot(norm, pli);
    float specintc;
    float specshinc;
      
    // Diffuse shading
    float3 col;
    col = ocol*lc*li*clamp(dnp, 0., 1.);
    
    // Specular shading
    #ifdef specular
    // Special varaiation of the specular shading for the floor
    if (objnr==FLOOR_OBJ)
    {
       	specintc = clamp(1.2*getFloorReflection(pos), 0., 1.);
       	specshinc = 50.*pow(specint, 0.5);
    }
    else
    {
        specintc = specint;
        specshinc = specshin;
    }
    
    //if (dot(norm, lp - pos) > 0.0)
    if (lampnr<2)
        col+= lamp.color*li*specintc*pow(max(0.0, dot(reflect(pl, norm), normalize(pos - campos))), specshinc);
    #endif
    
    return col;
}

// Shading of the objects over all lamps
float3 lampsShading(float3 norm, float3 pos, float3 ocol, int objnr)
{
    float3 col = float3(0.);
    for (int l=0; l<3; l++) // lamps.length()
        col+= lampShading(lamps[l], norm, pos, ocol, objnr, l);
    
    return col;
}

// From https://www.shadertoy.com/view/lsSXzD, modified
float3 GetCameraRayDir(float2 vWindow, float3 vCameraDir, float fov)
{
	float3 vForward = normalize(vCameraDir);
	float3 vRight = normalize(cross(float3(0.0, 1.0, 0.0), vForward));
	float3 vUp = normalize(cross(vForward, vRight));
    
	float3 vDir = normalize(vWindow.x * vRight + vWindow.y * vUp + vForward * fov);

	return vDir;
}

// Sets the position of the camera with the mouse and calculates its direction
void setCamera()
{
   float2 iMouse2;
   if (iMouse.x==0. && iMouse.y==0.)
      iMouse2 = iResolution.xy*float2(0.52, 0.65);
   else
      iMouse2 = iMouse.xy;
   
   campos = float3(-3. + 10.*cos(1.3 + 1.3*iMouse2.x/iResolution.x)*(1. - 0.3*iMouse2.y/iResolution.y),
                 12.*(iMouse2.y/iResolution.y),
                 10.*sin(1.3 + 1.3*iMouse2.x/iResolution.x)*(1. - 0.3*iMouse2.y/iResolution.y));
   camtarget = float3(-2., -1.2*iMouse2.y/iResolution.y + 1.1, 0.);
   camdir = camtarget - campos;   
}

// Combine the flame color with its background in a non-linear way
float3 combFlameCol(float3 col1, float3 col2)
{
    return pow(pow(col1, float3(fcp)) + pow(clamp(col2, 0., 1.), float3(fcp)), float3(1./fcp));   
}

// Tracing and rendering a ray
float3 flamecol = float3(0.);
void trace0(float3 tpos, float3 ray, float maxdist, bool ref)
{
    traceFlame = true;
    float2 tr = trace(tpos, ray, maxdist);
    traceFlame = false;
    float tx = tr.x;
    int objnr = int(tr.y);
    float3 col;
    float3 pos = tpos + tx*ray;
    float3 norm;
    float flameIntensityV = flameIntensity*(1. + 0.37*flameVar);
    
    if (ref)
    {
        if (map_flame(tpos, false, true)>9. && map_tube(pos)>0.3)
        	flamecol = getFlameDensColor(tpos, ray, flameStep, flameIntensityV, 70);
        else
            flamecol = float3(0.);
    }
    if (tx<maxdist*0.95)
    {
        float3 rayRef = float3(0.);
        
        // Includes the flame in the scene
        if (objnr==FLAME_OBJ)
        {
            flamecol = getFlameDensColor(pos, ray, flameStep, flameIntensityV, 70);
            
            // Calculating the effect of the heat refraction (quick and dirty)
            #ifdef heat_refraction
            rayRef = 0.032*clamp(smoothstep(-2.2, -2.5, pos.x)*pow(flamecol, float3(1.2)), -1., 1.);
            #endif
        }
            
        tr = trace(tpos, ray*(1. + rayRef), maxdist);
        tx = tr.x;
        objnr = int(tr.y);
        pos = tpos + tx*ray;
         
        norm = getNormal(pos, normdelta, objnr);
        col = getColor(norm, pos, objnr, ray);
      
        // Shading
        col = ambientColor*ambientint + lampsShading(norm, pos, col, objnr);
        
        // Ambient occlusion
        #ifdef ambocc
        #ifdef test_ambocc
        col = float3(calcAO(pos, norm));
        #else
        col*= 1. - aoint + aoint*float3(calcAO(pos, norm));
        #endif
        #endif
        
        // The end of the tube is glowing from the heat!
        #ifdef show_flame
        if (objnr==TUBE_OBJ)
            col+= smoothstep(-0.6, -2., pos.x)*float3(0.35, 0.13, 0.)*(0.8 + 0.2*flameVar);
        #endif
        
        col = combFlameCol(col, flamecol);
    }
    else
    {
        objnr = SKY_OBJ;
        col = float3(0.);
    }
    RenderData_col=col;
    RenderData_pos=pos;
    RenderData_norm=norm;
    RenderData_objnr=objnr;
    return;
}

// Main render function with reflections and refractions
float4 render(float2 fragCoord)
{   
  	float2 uv = fragCoord.xy / iResolution.xy; 
  	uv = uv*2.0 - 1.0;
  	uv.x*= iResolution.x / iResolution.y;

  	float3 ray0 = GetCameraRayDir(uv, camdir, fov);
    float3 ray = ray0;
  	//RenderData traceinf0 = 
  	trace0(campos, ray, maxdist, false);  	
    //RenderData traceinf = traceinf0;
    float3 RenderData__col=RenderData_col;
    float3 RenderData__pos=RenderData_pos;
    float3 RenderData__norm=RenderData_norm;
    int RenderData__objnr=RenderData_objnr;
   
  	float3 col = RenderData__col;
    float3 refray;
    
    #ifdef reflections
    float3 flamecol0 = flamecol;
    if (RenderData__objnr==FLOOR_OBJ)
    {	               
        refray = reflect(ray, RenderData__norm);

        //RenderData traceinf_ref = 
        trace0(RenderData__pos, refray, maxdist, true);
        float rf = getFloorReflection(RenderData__pos);

        col = combFlameCol(lerp(col, RenderData_ref.col, rf), rf*flamecol0);
        //col = combFlameCol(RenderData_ref.col, flamecol0);
        //col = RenderData_ref.col;
    }
    #endif

  	return float4(col, 1.0);
}



float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;  
    init();    
    setCamera();
    
    // Antialiasing.
    #ifdef antialias
    float4 vs = float4(0.);
    for (int j=0;j<aasamples ;j++)
    {
       float oy = float(j)*aawidth/max(float(aasamples-1), 1.);
       for (int i=0;i<aasamples ;i++)
       {
          float ox = float(i)*aawidth/max(float(aasamples-1), 1.);
          vs+= render(fragCoord + float2(ox, oy));
       }
    }
    float2 uv = fragCoord.xy / iResolution.xy;
    return vs/float4(aasamples*aasamples);
    #else
    return render(fragCoord);
    #endif
}  
