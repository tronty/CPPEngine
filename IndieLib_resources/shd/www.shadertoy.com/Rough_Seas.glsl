// Rough Seas, by Dave Hoskins.
// https://www.shadertoy.com/view/dtXGW4

// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// By David Hoskins, 2022.


#define FAR 1000.
#define FOG_COLOUR vec3(.5,.7,.8)
#define SKY_TOP vec3(.12, 0.2, 0.3)

// Below is the number of samples per frame,
// It may be better to use a DOF in a separate pass instead.

#define SAMPLES 1 // * 1 for speed.

float time_=0.0;
vec3 camPos;
vec3 skyColour;
vec3 sunDir = normalize(vec3(4,8,18));


//------------------------------------------------------------------------------
// Hashes from here: https://www.shadertoy.com/view/4djSRW
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

// Pretty basic smoothed noise...
//------------------------------------------------------------------------------
float noise2D(in vec2 p)
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

//------------------------------------------------------------------------------
// A very basic sky...
vec3 sky(vec3 dir)
{
 
    return mix(FOG_COLOUR, SKY_TOP ,abs(dir.y)*1.7);
}

//-----------------------------------------------------------------
// This creates the sea, it's complexity is governed by the incoming iteration count...
float COSR = cos(.43);
float SINR = sin(.52);
mat2 rot2D = mat2(COSR, SINR, -SINR, COSR) * 1.4;
float oceanFundamental(vec2 p, float d, float tim, float iter)
{
    float a =noise2D(p*.01)*8.+3.0;
    float h = 0.0;
    float it = 1./iter;
    p.x -= tim*5.0;
    float spiky = (noise2D(p) + noise2D(p*2.5)*.5)*.5;
    p *= .03;// ...Scale it
    spiky = pow(spiky, 5.)*80.;
    for (float i = 0.0; i <= 1.0; i += it)
    {
        float t = (1.-i) * tim;
        float r =noise2D(p*2.1) * i;
        vec2 y1 = (cos(p-t)+1.0);
        vec2 y2 = (1.0-abs(sin(p-t)));
        
        y1 = mix(y1, y2, r);
          
        float s = y1.x + y1.y;
        
        h += s*a;
        
        a *= .63;
        p = p * rot2D;
        p += 19.9;
    }
    h+= spiky*smoothstep(55.0, 150.0, h);
    return h;
}

// Map the ocean relative to the point...
//-----------------------------------------------------------------
float map(in vec3 p, in float d, float iter)
{
    float h = oceanFundamental(p.xz, d, time_, iter);
    return p.y-h;
}

// Bog standard ray marching, there's so much noise that any misses get lost...🤞
//-----------------------------------------------------------------
float rayMarch(vec3 p, vec3 dir)
{
    float d = 0.0;
    for ( int i = 0; i < 120; i++)
    {

        float h = map(p + dir*d, d, 7.0);
        if (abs(h) < 0.15 || d > FAR)
        {
            break;
        }
        //if (h < 0.0) h *= .5;
        d+= h*.8;
    }
    return d;
}

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
    ds *= 3./iResolution.y;
    ds = max(ds*ds, .1);

    float c = map(pos, 0., 13.);
    vec2 eps_zero = vec2(ds, 0.0);
    return normalize(vec3(map(pos + eps_zero.xyy, 0.0, 13.),
                          map(pos + eps_zero.yxy, 0.0, 13.),
                          map(pos + eps_zero.yyx, 0.0, 13.)) - c);
}

//------------------------------------------------------------------------------

// I was using my 2 tweet water caustic here,
// but some compilers opimized it broken with the rest of the code
// So I opted for a basic voronoi cell thing...
float waterPattern(vec2 p)
{
    p *=.02;
    vec2 n = floor(p);
    vec2 f = fract(p);
    float wp = 1e10;
    for (int i = -1;i<=1;i++)
    {
        for (int j = -1;j<=1;j++)
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
    for( float i = 3.0; i < 20.0; i+=5.)
    {
        float h = map(p + dir*i, i, 7.);
        if (h > 0.) break;
        d += -h;
    }
    return clamp(1.0-d*.027, 0.0, 1.0);
}

//------------------------------------------------------------------------------
vec3 lighting(vec3 pos, vec3 nor, in vec3 dir,in vec3 mat)
{
    vec3 col;
    col = mat * max(dot(sunDir, nor), 0.0);
    vec3 ref = reflect(dir, nor);
    col += pow(max(dot(sunDir, ref), 0.0), 150.0);

    float fres = clamp(pow( 1.+dot(nor, dir), 5. ), 0.0, 1.0);
    col = mix(col, sky(nor), .4);

  
    return col;
}

//------------------------------------------------------------------------------
vec3 diffuse(in vec3 pos, in vec3 nor, in float dep)
{
    pos.x -= time_*.5;
    vec3 mat = vec3(.2,.3,.5);
    float h = smoothstep(0.1, 1.0,nor.y);

    mat = mix(mat, vec3(.3,.8,.7), dep);

    float foam = waterPattern(pos.xz*vec2(.5,1.)+99.)*15.;
    foam += waterPattern(pos.xz*3.63)*10.;
    
    

    mat = mat+foam * dep*dep*2.;

    return mat;
}

//------------------------------------------------------------------------------
void main( )
{
    // Take into account non-square viewport to keep aspect ratio of shapes..
    // Zero in centre and .5 at the max Ys
    vec2 uv = (xlv_TEXCOORD0-iResolution.xy*.5)/iResolution.y;

    vec2 mouse = vec2(0);
    if (iMouse.z > 0.) mouse = (iMouse.xy-iResolution.xy*.5) /iResolution.y;
    

    vec3 colour = vec3(0);
    for (int loop = min(0, iFrame); loop < SAMPLES; loop++)
    {
        float fLoop = float(loop);
    
        // Find a good point in time_ with time_ blurring for the top of the display...
        float f = xlv_TEXCOORD0.y/iResolution.y;
        time_ = iTime+hash12(xlv_TEXCOORD0+fLoop)*f*.1 + 20.;

        // Set colour to zero then call the rayMarcher to get distant object...
      
        camPos = vec3(time_*.01,100,0);
        float h = (sin(time_*.75)+1.0)*50.+ 60.;

        float oce = 0.0;

        // Bounce along the average wave height for a set time_...
        for (float i = 0.0; i < 4.0; i++)
        {
            oce += oceanFundamental(camPos.xz, 0.0, time_ + i, 7.0);
        }
        oce = (oce / 4.0)+10.0;
        if (h< oce)
        {
            // Don't lock any movement -it's the sea!
            float f = oce-h;
            h = h+pow(f, .3)*.4;
         }


        // Setup camera...
        vec3 col;
        vec3 dir  = vec3(0,0, 1.);
        dir = viewMat (uv.y -.3 - h*.002+mouse.y*.5, uv.x-2.+time_*.25 + mouse.x*6.28) * dir;

        camPos.y = h;
        float dis = rayMarch(camPos, dir);


        if (dis < FAR)
        {
            // The position is the start position plus the normalised direction X distance...
            vec3  pos = camPos + dir * dis; // ...wave hit position
            vec3  nor = normal(pos, dis);   // ... Normal
             // The depth of the wave in forward direction, it's simple but effective in helping the water transparent effect...
            float dep = waveDepth(pos+dir*.3, dir);
            vec3  mat = diffuse(pos, nor, dep);

            col = lighting(pos, nor, dir, mat);
            col = mix(col, FOG_COLOUR, smoothstep(40.0, FAR, dis));
        }else
        {
            col = sky(dir);
        }
        colour += col;
    }
    colour /= float(SAMPLES);

    // Some adjustment..
    colour = colour*.5 + smoothstep(0.0, 1.0, colour)*.5;
    
    vec2 xy = xlv_TEXCOORD0/iResolution.xy;
    colour *= 0.5 + .5*pow( 80.0*xy.x*xy.y*(1.0-xy.x)*(1.0-xy.y), .5);  // ...Vignette.
    gl_FragColor = vec4(sqrt(colour), 1);
}

//-----------------------------------------------------------------

