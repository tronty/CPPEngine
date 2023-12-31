// Rough Seas Porthole, by Dave Hoskins.
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// By David Hoskins, 2023.

// This shader is  https://www.shadertoy.com/view/DtBGWW
// Forked from     https://www.shadertoy.com/view/dtXGW4
// Used my Rough Seas shader and added a cabin porthole, underwater effects, foam and distortion.

// Inspiration was from: https://www.istockphoto.com/search/more-like-this/882229368?assettype=film&phrase=rough%20sea


#define FAR 1400.
#define FOG_COLOUR vec3(.34,.3,.3)
#define SKY_TOP vec3(.1, .13, 0.13)
#define FOAM_COLOUR vec3(.51,.55,.6)
#define PI 3.141592653

vec3 camPos;
//float time;
vec3 surf;
vec3 skyColour;
const vec3 sunDir = normalize(vec3(4,8,18));
//#define EXPORT_VERSON

//------------------------------------------------------------------------------
// Hashes from here: https://www.shadertoy.com/view/4djSRW
// It'll work in the GLSLsandbox and others that don't accept uvec2's.
float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}
vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}

float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}
vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy + p3.yxx)*p3.zyx);
}

// Smoothed noise overridden as 2D & 3D...
//------------------------------------------------------------------------------
float noise(in vec2 p)
{
	vec2 f = fract(p);
    p = floor(p);
    f = f * f * (3.0 - 2.0 * f);
    float res = mix(mix(hash12(p),
						hash12(p + vec2(1.0, 0.0)), f.x),
					mix(hash12(p + vec2(0.0, 1.0)),
						hash12(p + vec2(1.0, 1.0)), f.x), f.y);
    return res;
}

float noise(in vec3 p)
{
    const vec2 add = vec2(1.0, 0.0);

    vec3 f = fract(p); 
    f *= f * (3.0-2.0*f);
    p = floor(p);

    float h = mix(
                    mix(mix(hash13(p), hash13(p + add.xyy),f.x),
                        mix(hash13(p + add.yxy), hash13(p + add.xxy),f.x), f.y),
                    mix(mix(hash13(p + add.yyx), hash13(p + add.xyx),f.x),
                        mix(hash13(p + add.yxx), hash13(p + add.xxx),f.x), f.y),
                 f.z);
    return h*h*h*2.;
}

//------------------------------------------------------------------------------
// A very basic sky...
vec3 sky(vec3 dir)
{
    return mix(FOG_COLOUR, SKY_TOP ,abs(dir.y)*2.);
}

//-----------------------------------------------------------------
// This creates the sea, it's complexity is governed by the incoming iteration count...
const float COSR = cos(.43);
const float SINR = sin(.52);
const mat2 rot2D = mat2(COSR, SINR, -SINR, COSR) * 1.4;
float oceanFundamental(vec2 p, float tim, float iter)
{
    float a =noise(p*.01)*8.+3.0;
    float h = 0.0;
    float it = 1./iter;
    float spr = 0.0;
   //p.x -= tim*1.0;

    p *= .025;// ...Scale it

    for (float i = min(iTime,0.0); i <= 1.0; i += it)
    {
        float t = (1.08-i) * tim;
        float r =noise(p*2.1+t) * i;
        vec2 y1 = (cos(p-t)+1.0);
        vec2 y2 = (1.0-abs(sin(p-t)));
        
        y1 = mix(y1, y2, r);
          
        float s = y1.x + y1.y;
        
        h += s*a;
        
        a *= .59;
        p = p * rot2D;
        p += 19.9;
    }

    return h;
}

//-----------------------------------------------------------------
float fbm(in vec3 p)
{
    float a = 1.;
    float y = 0.0;
    for (int i = min(iFrame,0); i< 6; i++)
    {
        y += noise(p) * a;

        a *= .5;
        p.xz *= rot2D;
        p.y += 9.1;
    }
    return y;
}

// Do a warped multi FBM...
float multiFbm(vec3 p)
{
    float f = 0.0;
    f = fbm(p*.87);
    f = fbm(p + f);
    f = fbm(p*.5 + f*.4);

    return f;
}


// Map the ocean relative to the point...
//-----------------------------------------------------------------
float map(in vec3 p, float iter)
{
    float h = oceanFundamental(p.xz, time, iter);
    return p.y-h;
}

// March across the sea...
//-----------------------------------------------------------------
vec2 rayMarchSea(vec3 p, vec3 dir)
{
    float d = 0.0;
    float spr = 0.0;
    for ( int i = 0; i < 120; i++)
    {
        vec3 pos = p + dir*d;

        float hh = oceanFundamental(pos.xz, time, 7.);
        float h = pos.y-hh;
        
        if (h < .11 || d > FAR)
        {
            break;
        }
        float wind = (noise(pos*.05)) * noise(pos*2.73+vec3(-time*10., time*.11,0));
        spr += max(25.-h, 0.0) * smoothstep(40.0, .0,max(h, 0.0))*smoothstep(FAR, 0.0,d)*smoothstep(30., 80.0, hh)
        * wind;


        //if (h < 0.0) h *= .5;
        d+= h*.7;
    }
    return vec2(d, min(spr*.03, 1.0));
}



//==============================================================================
#define FARCAB 100.0
//------------------------------------------------------------------------------
mat2 rotMat2(float a)
{
    return mat2(cos(a), sin(a), -sin(a), cos(a));
}

float roundedBox( vec3 p, vec3 b, float r )
{
	return length(max(abs(p)-b,0.0))-r;
}

// Bend space into s rotations
vec2 nutCut(in vec2 p, float s)
{
    float k = s / PI / 2.;
    return p* rotMat2(floor((atan(p.y, p.x)) * k + .5) / k);
}


float nut(vec3 p, vec3 size, float r)
{
    p.xy*=rotMat2(r);
    // Six sides...
    p.xy = nutCut(p.xy, 6.0);

    // Now rounded box has 6 sides..
    float t = roundedBox(p, size,.08);
    t = min( t, length(p+vec3(0,0,.2))- size.x );

    return t;
}

//------------------------------------------------------------------------------
// https://iquilezles.org/articles/distfunctions/
float length6( vec2 p ) { p=p*p*p; p=p*p; return pow(p.x+p.y,1.0/6.); }
// Distance to a squarish torus..
float sdTorusFlat(vec3 p, vec2 t)
{
  float x = length(p.xy)-t.x;
  return length6(vec2(x,p.z))-t.y;
}

//------------------------------------------------------------------------------
// Min/max functions with material swap...
vec2 maxV2(vec2 v1, vec2 v2)
{
    return (v2.x > v1.x) ? v2:v1;
}
    
vec2 minV2(vec2 v1, vec2 v2)
{
   return (v2.x < v1.x) ? v2:v1;
}

//------------------------------------------------------------------------------
vec2 mapCab(vec3 p)
{
    // Do porthole rim as a flattened tourus...
    vec2 d = vec2(sdTorusFlat(p-vec3(0,0,.4), vec2(9.1,1.1)), 0);
    // Back wall...
    d = minV2(d, vec2(.0-p.z, 2.0));
    // Do the nuts as a mirrored configuration...
    p.xy = abs(p.xy);
    d = minV2(d, vec2(nut(p - vec3(6.2, 6.5, -.8),vec3(.15, .15, .12), .5), 1.0));
    d = minV2(d, vec2(nut(p - vec3(8.9,.0,-.8),   vec3(.15, .15, .12), 0.0), 1.));

    return d;
}

//------------------------------------------------------------------------------
vec3 normalCab(vec3 pos, float ds)
{
    ds *= 1./iResolution.y;
    //ds = max(ds*ds, .1);

    float c = mapCab(pos).x;
    vec2 eps_zero = vec2(ds, 0.0);
    return normalize(vec3(mapCab(pos + eps_zero.xyy).x,
                          mapCab(pos + eps_zero.yxy).x,
                          mapCab(pos + eps_zero.yyx).x) - c);
}

//------------------------------------------------------------------------------
float shadowCab(in vec3 ro, in vec3 rd)
{
	float res = 1.0;
    float t = .01;
    for( int i = min(iFrame,0); i < 30; i++ )
    {
		float h = mapCab(ro + rd*t).x;

        res = min( res, 4.*h/t );
        t += h;
        //if (res < .1) break;
    }
    return clamp( res, 0.1, 1.0 );
}

//------------------------------------------------------------------------------
vec3 lightCab(vec3 pos, vec3 nor, vec3 dir, vec4 mat, float wat)
{
    vec3 lamp = vec3((noise(vec2(time*.5))-.5)*30.,wat*.2,-15);

    vec3 ldir = normalize(lamp-pos);
    float a = smoothstep(-10.0, 4.0, wat)*.3+.05;
    float f = max(multiFbm(pos*5.), 0.0);
    f = pow(f, 3.0)*2.+.4;
    float r = multiFbm(pos*4.);
    
    float sha = shadowCab(pos, ldir);

    
    vec3 col = mat.xyz * max(dot(nor, ldir), 0.0)*4.*a*sha;
    vec3 ref = reflect(dir, nor);
    col += vec3(1.,.8, .7) * pow(max(dot(ref, ldir), 0.0), 2.0) * mat.w * sha;
    return min(col, 1.0);
} 

//------------------------------------------------------------------------------
vec2 marchCabin(const in vec3 ro, const in vec3 rd)
{
    vec2 d = vec2(0.0);//d -= hash12(ro.yz)*.5;
    for (int i = 0; i < 50 && d.x < FARCAB; i++)
    {
        vec3 p = ro+rd*d.x;
        vec2 m = mapCab(p);
        d.y = m.y;
        if ((m.x) < .05 ||  d.x > FARCAB) break;
        d += m.x*.8;
        

    }
    return d;
}

//==============================================================================

//------------------------------------------------------------------------------
// Get a view of pixel using Euler...
mat3 viewMat (float ay, float az)
{
  vec2 o, ca, sa;
  o = vec2 (ay, az);
  ca = cos (o);
  sa = sin (o);
  return mat3 (ca.y, 0., - sa.y, 0., 1., 0., sa.y, 0., ca.y) *
         mat3 (1., 0., 0., 0., ca.x, - sa.x, 0., sa.x, ca.x);
}

//------------------------------------------------------------------------------

// I forgot where this came from, it using a higher iteration than the ray march...
vec3 normal(vec3 pos, float ds)
{
    ds *= 2./iResolution.y;
    ds = max(ds*ds, .1);

    float c = map(pos, 14.);
    vec2 eps_zero = vec2(ds, 0.0);
    return normalize(vec3(map(pos + eps_zero.xyy, 14.),
                          map(pos + eps_zero.yxy, 14.),
                          map(pos + eps_zero.yyx, 14.)) - c);
}

//------------------------------------------------------------------------------

// I was using my 2 tweet water caustic here,
// but some compilers optimised it broken with the rest of the code
// So I opted for a basic voronoi cell thing...
float waterPattern(vec2 p)
{
    p *=.02;
    vec2 n = floor(p);
    vec2 f = fract(p);
    float wp = 1e10;
    for (int i = min(iFrame, -1);i<=1;i++)
    {
        for (int j = min(iFrame, -1);j<=1;j++)
        {
            vec2 g = vec2(i, j);
            vec2 o = hash22(n+g);
            
            vec2 r = g + o - f;
            float d = dot(r, r);
            if (d < wp)
            {
                wp = d;
            }
        }
    }
    return pow(wp, 3.5);
}


//------------------------------------------------------------------------------
float waveDepth(vec3 p, vec3 dir)
{
    float d = 0.0;
    for( float i = 3.0; i <= 23.0; i+=5.)
    {
        float h = map(p + dir*i, 7.);
        if (h > 0.) break;
        d += -h;
    }
    return clamp(1.0-d*.02, 0.0, 1.0);
}

//------------------------------------------------------------------------------
vec3 lighting(vec3 pos, vec3 nor, in vec3 dir,in vec3 mat)
{
    vec3 col;
    col = mat * max(dot(sunDir, nor), 0.0);
    vec3 ref = reflect(dir, nor);
    float fres = clamp(pow( 1.+dot(nor, dir), 7. ), 0.0, 1.0);

    col = mix(col, sky(nor), .2);
    col = mix(col, sky(ref), fres);
    return col;
}

//------------------------------------------------------------------------------
vec3 diffuse(in vec3 pos, in vec3 nor, in float dep)
{
    pos.x -= time*1.3;
    vec3 mat = vec3(.1,.1,.12);
    float h = smoothstep(0., 1.0,nor.y);
    
    mat += h*.1;

    mat = mix(mat, vec3(.3,.7,.7), dep);

    // Add different frequencies of voronoi cells...
    float foam = waterPattern(pos.xz+99.)*25.;
    foam += waterPattern(pos.xz*(1.5-nor.y)*30.0)*20.;
    foam += waterPattern(pos.xz*8.)*16.;
    foam = clamp(foam, 0.0, 1.0);
    mat = mat+foam * dep*dep*2.;

    return mat;
}
//----------------------------------------------------------------------------------------------------------
float distanceRayPoint(vec3 ro, vec3 rd, vec3 p)
{
    p -= ro;
    float h = dot(p,rd);
    return length(p-rd*h);
}

#define tri(x) abs(fract(x)-.5)*2.

//----------------------------------------------------------------------------------------------------------
vec3 getOffset(in vec3 id, float si)
{
    return tri(id+time*.5);
}

vec3 getColour(vec2 id)
{
    float t1 = id.x*.4;
    float t2 = id.y*.4;
    return vec3(.75,.6+t1, .6+t2);
}

//----------------------------------------------------------------------------------------------------------
#define BUBBLES 65
#define BUBBLE_SIZE 50.0
vec3 bubbles(in vec3 ro, in vec3 rd)
{ 
    float d;
    ro *= .00003;
    float fade = 1.0;
	vec3 pos = floor(ro),
         ri = 1./rd,
		 rs = sign(rd),
		 dis = (pos-ro + 0.5 +rs*.5) * ri,
         sum = vec3(0), col = sum;
    
	for( int i = min(iFrame,0); i < BUBBLES; i++ )
    {
        vec3 id = hash33(pos*19.31);
        float si = (id.x+id.z)*.1+.2;

        vec3 offset = getOffset(id, si);

        d = distanceRayPoint(ro, rd, pos+offset);
        vec3 mat =  getColour(id.xy);
        
        col = mat * pow(smoothstep(si, 0.0,d),8.)*10. *fade;
        sum += col;

        fade*=.95;

  
		vec3 mm = step(dis.xyz, dis.yzx) * step(dis.xyz, dis.zxy);
		dis += mm * rs * ri;
        pos += mm * rs;

	}
  
	return sum;
}

//------------------------------------------------------------------------------
// Exponential fader...

//------------------------------------------------------------------------------
float fader(float edge0, float edge1, float x)
{
    float t = (x - edge0) / (edge1 - edge0);
    return  clamp(exp((t-.9825)*3.)-.0525, 0.0, 1.0);
}

//------------------------------------------------------------------------------
void surfFoam(vec3 p, float water)
{
     float w = -(water-4.0)*pow(multiFbm(p*2.+vec3(0, time*8., time*4.)), 2.0)*.5;
     surf += FOAM_COLOUR*fbm(camPos*8.3+vec3(0,0,time*6.))* .35 * w;
     surf *= FOAM_COLOUR*smoothstep(-10.,1.0, water);
}

//------------------------------------------------------------------------------
float underWaterAtTime(vec3 p, float ti)
{
    float h = oceanFundamental(p.xz, ti, 7.0)+multiFbm(camPos*5.3+ti*7.);
     
    return p.y-h-4.;
}

vec3 cameraSetup(float ti)
{
     vec3 p = vec3(ti*10.,100,0);
     float h = (sin(ti*.65)+1.0)*40. + (sin(ti*1.37))*10. + 20.;
     p.y = h;
        
     // Don't spend too long in the water?..
     //float w = (map(p, 7.0));
     //if (w < 0.0) p.y  += w*w*.04;
     return p;
}
 
vec3 bumpmap(vec3 p, in vec3 dir, in float a)
{
    p *= 5.;
    float v0 = multiFbm(p);
    float v1 = multiFbm(p+vec3(.1,0.,0));
    float v2 = multiFbm(p+vec3(0,0.,.1));
    vec3 grad = normalize(vec3(v1-v0, 0.0, v2-v0));
    grad -= dir*dot(dir, grad);          

    
    return normalize( dir + grad*a );
}
 
//------------------------------------------------------------------------------
// Grab the vec4 diffuse texture with specular in .w...
// You can also adjust the normal here..
vec4 getMaterial(in int id, in vec3 p, inout vec3 nor)
{
    vec4 col = vec4(0);
    vec2 bn = vec2(0.0);
    float d = 0.0;
    switch(id)
    {
        case 0: // Porthole rim...
            col = vec4(.07, .045, .04, .07);
            col = col*(.6+.4*multiFbm(p*5.+30.));
            col.w = col.w*col.w*70.;
            bn = vec2(3.0,0.08);
            break;
        case 1: // Nuts...
            col = vec4(.05, .05, .06, .3);
            col.w = col.w*(.4+.6*multiFbm(p*21.));
            break;
        case 2: // Back wall...
            col = vec4(.00,.02, multiFbm(p*.5-300.)*0.02, .1 );
            float f = multiFbm(p*1.2+999.);
            col = mix(col, col*.8, smoothstep(.5, .52, f));
            bn = vec2(4.,0.1);
            col =mix(col, vec4(0.05, 0.01,0,.0), (max(multiFbm(p*.4)-0.4,0.))*.4);
            break;
    }    
    nor = bumpmap(p * bn.x, nor, bn.y);
    return col;
}


//------------------------------------------------------------------------------
void main( )
{
	vec2 coord=xlv_TEXCOORD0;
    // Take into account non-square viewport to keep aspect ratio of shapes..
    // Zero in centre and .5 at the max Ys
    vec2 uv = (coord-iResolution.xy*.5)/iResolution.y;

    time = iTime+212.;
    // Reset colours...
    vec3 colour = vec3(0);
    surf = vec3(0.0);
    // Make simple camera shade with uv movements...
    vec2 shake = vec2(noise(vec2(time*.6))-.5, noise(vec2(time*.7+233.))-.5) * vec2(.1, .1);
    uv += shake;
    
    camPos = cameraSetup(time);
    float porthole = length(uv)*2.;
    
    // Don't do things unnecessarily layered, only render sea in porthole...
    if (porthole < 1.0)
    {
        // Camera...
        // We need to start the ray from the porthole, because a single camera point will all go underwater at the same time,
        // and it needs to have the window wash effect.
        camPos.xy += uv*10.;

        vec3 dir  = vec3(0,0, 1.);
        dir = viewMat (uv.y +.0, uv.x+.5) * dir;
        
        float water = (underWaterAtTime(camPos, time));
        
        if(water < 4.0) //...Minus values under water
        {
            
            // Tint water colour to green sea...
            colour  = mix(FOAM_COLOUR, vec3(.0, .04,.03),clamp(pow(-(water-4.0), .5)*.3, 0.0, 1.0));
            // With a deeper blue...
            colour  = mix(colour, vec3(.0, .0,.01),clamp((-3.0-water)*.015, 0.0, 1.0));
            // Top white foam band..
            colour  = mix(colour, FOAM_COLOUR,clamp(2.-(water*water*.4)*multiFbm(camPos*3.3), 0.0, 1.0));
            
            // Do 2D surf Foam...
            surfFoam(camPos, water);
            
            // If under water, do bubbles...
            if (water < .0) surf += bubbles(camPos*25000., dir)*.3*clamp(4.0+water*.1,0.1, 3.0)*.03;
        }
        if (water >= 0.)
        {
            // Above water...
            vec3 p = cameraSetup(time-1.);

            float past = underWaterAtTime(p, time-1.); // ...Did we go underwater a second ago?
            // Determine an amount of water distortion...
            float f = smoothstep(25.0, -8.0, past)*.15;
            // Make the distortion less as we go down the screen...
            float w = smoothstep(.6, -.3, uv.y);
            f = f*f*3. * w;
            float d =0.0;
            if (f > 0.0)
            {
                dir = bumpmap(vec3(uv.x*1.5,time,uv.y), dir, f);
            }

            // March the sea
            vec2 dis = rayMarchSea(camPos, dir);
            if (dis.x < FAR)
            {
                // The position is the start position plus the normalised direction X distance...
                vec3  pos = camPos + dir * dis.x; // ...wave hit position
                vec3  nor = normal(pos, dis.x);   // ... Normal
                 // The depth of the wave in forward direction, it's simple but effective in helping the water transparent effect...
                float dep = waveDepth(pos, dir);
                vec3  mat = diffuse(pos, nor, dep);
                
                colour = lighting(pos, nor, dir, mat);
                colour = mix(FOG_COLOUR, colour, exp(-dis.x*.001));
            }else
            {
                colour = sky(dir);
            }

           colour = mix(colour, FOAM_COLOUR, dis.y);

            
         }
    }else 
    {   // Everything outside the portal...
        float water = (underWaterAtTime(camPos, time));
     
        //colour = vec3((1.1-porthole)*.3)* abs((uv.y-.5)*4.);
        camPos = cameraSetup(time);
        vec3 dir  = vec3(0,0, 1.);
        dir = viewMat (uv.y, uv.x) * dir;
        camPos = vec3(0,0,-15);
        vec2 dis = marchCabin(camPos, dir);
        if (dis.x < FARCAB)
        {
            vec3 pos = camPos + dir * dis.x;
            vec3 nor = normalCab(pos, dis.x);
            int matID = int(dis.y);
            vec4 mat = getMaterial(matID, pos, nor);
            colour = lightCab(pos, nor, dir, mat, water);
        }
    }
    colour += surf;

    if (porthole < 1.0) colour = mix(colour, colour*.1, smoothstep(0.9, 1.,porthole));
    
    // Some adjustment..
    colour = clamp(colour*.2 + smoothstep(0.0, 1.0, colour)*.8,0.0, 1.0);

    colour = sqrt(colour);
    colour += hash12(coord+time)/256.0; //...Dither those 8bit limitations away
    
    
#ifdef EXPORT_VERSON
    gl_FragColor = vec4(fader(0.0, 4.0, iTime) * fader(299.0, 294.0, iTime)*colour, 1);
#else
    gl_FragColor = vec4(fader(0.0, 4.0, iTime) * colour, 1);
#endif
}

//-----------------------------------------------------------------

