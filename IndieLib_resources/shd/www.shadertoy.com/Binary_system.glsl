uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

// "Binary system" by Duke
// Based on "Binary star" https://www.shadertoy.com/view/ltjSWV shader 
// Blackhole based on this http://glslsandbox.com/e#21204.0 shader
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0

#define DITHERING
#define KEYBOARDZOOM

#define pi 3.14159265
#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)

const float spin_speed=0.7;

mat2 Spin(float angle){
	return mat2(cos(angle),-sin(angle),sin(angle),cos(angle));
}

// mass (and radius of the event horizon as well)
const float mass = .15;

float smin( float a, float b, float k )
{
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}

//-------------------------Noise--------------------------
// IQ's noise
float pn( in vec3 p )
{
    vec3 ip = floor(p);
    p = fract(p);
    p *= p*(3.0-2.0*p);
    vec2 uv = (ip.xy+vec2(37.0,17.0)*ip.z) + p.xy;
    uv = textureLod( iChannel0, (uv+ 0.5)/256.0, -100.0 ).yx;
    return mix( uv.x, uv.y, p.z );
}

// FBM
float fpn(vec3 p) {
    return pn(p*.06125)*.57 + pn(p*.125)*.28 + pn(p*.25)*.15;
}

float rand(vec2 co){// implementation found at: lumina.sourceforge.net/Tutorials/Noise.html
	return fract(sin(dot(co*0.123,vec2(12.9898,78.233))) * 43758.5453);
}

//-------------------------Disk---------------------------

float length2( vec2 p )
{
	return sqrt( p.x*p.x + p.y*p.y );
}

float length8( vec2 p )
{
	p = p*p; p = p*p; p = p*p;
	return pow( p.x + p.y, 1.0/8.0 );
}

float Disk( vec3 p, vec3 t )
{
    vec2 q = vec2(length2(p.xy)-t.x,p.z*0.5);
    return max(length8(q)-t.y, abs(p.z) - t.z);
}

//-------------------------Star---------------------------
// Noise Settings
#define NoiseSteps 4
#define NoiseAmplitude 0.08
#define NoiseFrequency 48.0
  
float Turbulence(vec3 position, float minFreq, float maxFreq, float qWidth)
{
    float value = 0.0;
    float cutoff = clamp(0.5/qWidth, 0.0, maxFreq);
    float fade;
    float fOut = minFreq;
    for(int i=NoiseSteps ; i>=0 ; i--)
    {
      if(fOut >= 0.5 * cutoff) break;
      fOut *= 2.0;
      value += abs(pn(position * fOut))/fOut;
    }
    fade = clamp(2.0 * (cutoff-fOut)/cutoff, 0.0, 1.0);
    value += fade * abs(pn(position * fOut))/fOut;
    return 1.0-value;
}

float SphereDist(vec3 position, vec3 rposition, float radius)
{
    return length(position - rposition) - radius;
}

float Star(vec3 position, vec3 rotdir, vec3 rposition, float radius)
{
    float distance;
    float t=spin_speed*iTime;
    float noise = Turbulence(vec3(Spin(t*0.25+rotdir.y*.81)*rotdir.zx*50.,rotdir.y*50.+t), 0.1, 1.5, 0.03) * NoiseAmplitude;
    noise = clamp((abs(noise)),0.0,1.0);
    distance = SphereDist(position, rposition, radius) - noise;
    return distance;
}

//-------------------------Vortex-------------------------
float ridged(float f){
	return 1.0-2.0*abs(f);
}

float Vortex(vec3 q, vec3 rposition, float h, float invjet) // the isosurface shape function, the surface is at o(q)=0
{
    q += rposition;
	float t=spin_speed*iTime;
	if(q.y<0.0) return length(q.xyz*vec3(1.0,invjet,1.0))-5.75;
    vec3 spin_pos=vec3(Spin(t-sqrt(q.y))*q.xz,q.y-t*5.0);
	float zcurve=pow(q.y,1.5)*0.03;
	float v=abs(length(q.xz)-zcurve)-5.5-clamp(zcurve*0.2,0.1,1.0)*pn(spin_pos*vec3(0.1,0.1,0.1))*5.0;
	return max(v, q.y - h);
}

//-------------------------Map----------------------------
vec2 map(vec3 p) {
    
    float t=spin_speed*iTime;

    // system rotation
    R(p.xy, pi*iTime*0.02);
   
    float StarD = Star(p,p,vec3(-1.0,6.5,0.0),2.5);
    float VortexD = Vortex(p/0.02, vec3(45.0,-100.0,0.0), 80.0, 1.0)*0.02;
    //---------------------------    
    float Radius = 0.98;
    //float DiskD = Disk(p,vec3(1.0,Radius,0.35)) + fpn(vec3(Spin(t*0.125+p.z*.80)*p.xy*80.,p.z*40.-t)*6.0) * length(p) * 0.34; //old variant
    float DiskD = Disk(p,vec3(1.0,Radius,0.35)) + fpn(vec3(Spin(t*0.25+p.z*.10)*p.xy*20.,p.z*20.-t)*5.0) * length(p) * 0.34;
    float r = (sqrt(dot(p,p))/sqrt(2.0))*0.6;
	float a = atan(p.y, p.x);
	float dOffset = cos( (a+r*r*20.0)*2.-iTime*2.5) * .5 + .5;
	dOffset *= 1.0-r/Radius;
	DiskD += dOffset * .2;
    //---------------------------
    //float d4 = Star(p,p.yzx,vec3(0.0,0.0,0.0),0.05)*.8;
    //---------------------------
    vec3 q = p;
    R(q.yz, -0.25*pi*2.);
    float Jet1D = Vortex(q/0.012, vec3(0.0,130.0,0.0), 90.0, 0.0)*0.012 + pn(vec3(Spin(t*4.5+p.z*.80)*p.xy*80.,p.z*40.-t)) * 0.18;
    //---------------------------
    q = p;
    R(q.yz, 0.25*pi*2.);
    float Jet2D = Vortex(q/0.012, vec3(0.0,130.0,0.0), 90.0, 0.0)*0.012 + pn(vec3(Spin(t*4.5+p.z*.80)*p.xy*80.,p.z*40.-t)) * 0.18;
    //---------------------------
    vec2 result = vec2(smin(smin(VortexD,DiskD/*smin(DiskD,d4,0.1)*/,1.0),StarD,1.5), 1.0);
    
    result = vec2(min(min(Jet1D,Jet2D),result.x), step(min(Jet1D,Jet2D),result.x));
    
    return result;
}

//-------------------------Color palette------------------
// See "Combustible Voronoi"
// https://www.shadertoy.com/view/4tlSzl
vec3 firePalette(float i){

    float T = 1550. + 1400.*i; // Temperature range (in Kelvin).
    vec3 L = vec3(7.4, 5.6, 4.4); // Red, green, blue wavelengths (in hundreds of nanometers).
    L = pow(L,vec3(5.0)) * (exp(1.43876719683e5/(T*L))-1.0);
    return 1.0-exp(-5e8/L); // Exposure level. Set to "50." For "70," change the "5" to a "7," etc.
}

//-------------------------Main---------------------------
void main( )
{  

    // rd: direction of the ray
    vec3 rd = normalize(vec3((xlv_TEXCOORD0.xy-0.5*iResolution.xy)/iResolution.y, 1.));

    #ifdef KEYBOARDZOOM
    // keyboard control zoom
    // Press 1-2-3 to zoom in and zoom out.
    const float KEY_1 = 49.5/256.0;
	const float KEY_2 = 50.5/256.0;
	const float KEY_3 = 51.5/256.0;
    float key = 0.0;
    key += 0.7*texture(iChannel1, vec2(KEY_1,0.25)).x;
    key += 0.7*texture(iChannel1, vec2(KEY_2,0.25)).x;
    key += 0.7*texture(iChannel1, vec2(KEY_3,0.25)).x;
	vec3 ro = vec3(0., 0., -11.+key*3.8);

    R(rd.zy, -pi*3.35);
    R(ro.zy, -pi*3.35);
    
    R(rd.yx, iMouse.x*0.008*pi);
   	R(ro.yx, iMouse.x*0.008*pi);  
    #else
    // mouse control zoom 
    vec3 ro = vec3(0., 0., -11.0 + iMouse.y * 0.022);
    R(rd.yz, -0.0*0.01*pi*2.);
	R(rd.xz, iMouse.x*0.01*pi*2.);
    R(ro.yz, -0.0*0.01*pi*2.);
	R(ro.xz, iMouse.x*0.01*pi*2.);
    #endif
    
    // ld, td: local, total density 
    // w: weighting factor
    float ld=0., td=0., w;

    // t: length of the ray
    // d: distance function
    float d=1., t=0.;
   
    // Distance threshold.
    const float h = .1;
   
    // total color
    vec3 tc = vec3(0.);
   
    #ifdef DITHERING
    vec2 pos = ( xlv_TEXCOORD0.xy / iResolution.xy );
    vec2 seed = pos + fract(iTime);
    t=(.4+0.2*rand(seed*vec2(1)));
    #endif

    // p: position on the ray
    vec3 p = ro+t*rd;
    
    vec2 sc = vec2(0.0);
    // rm loop
    for (int i=0; i<48; i++) {

      // Loop break conditions. 
      if(td>(1.0-1./200.) || d<0.001*t || t>22. ||  dot(p,p) > 125. || dot(p,p) < mass*mass) break;

      // evaluate distance function
      sc = map(p);
      d = sc.x;
      
      // check whether we are close enough (step)
      // compute local density and weighting factor 
      // const float h = .1;
      ld = (h - d) * step(d, h);
      w = (1. - td) * ld;   
	       
      // accumulate color and density
      tc.x += (w*w + 1./50.) * sc.y * step(d, 0.55) * 3.0; // Jets color
      tc.y += (w*w + 1./50.) * step(sc.y, h) * step(d, 1.95); // Star and disk color
        
      td += w +1./200.;

	  #ifdef DITHERING 
      d=abs(d)*(0.4+0.2*rand(seed*vec2(i)));
      #endif         
         
      d = min(d, (length(p)-mass)*.5);
        
      // enforce minimum stepsize
      d = max(d, 0.04); 
      
      p += d*rd;

      // step forward
      t +=  max(d * 0.5, 0.02);
        
      // Approximated event horizon.
	  vec3 fieldDeriv = p * mass / pow(length(p),3.) / (2.*(1. - mass/length(p)));
	  rd = normalize(rd-d*fieldDeriv);

    }
    
	// Coloring
	tc = mix(firePalette(tc.y),vec3(0.0,0.0,tc.x),0.4)*2.0;
	    
    // stars background
    if (td>0.6) {gl_FragColor = vec4(tc, 1.0); return;}
    vec3 stars = vec3(pn(rd*500.0)*0.5+0.5);
    vec3 col = vec3(0.0);
    col = mix(col, vec3(0.8,0.9,1.0), smoothstep(0.95, 1.0, stars)*clamp(dot(vec3(0.0),rd)+0.75,0.0,1.0));
    col = clamp(col, 0.0, 1.0);
    tc += col; 
    
    gl_FragColor = vec4(tc, 1.0);
}
