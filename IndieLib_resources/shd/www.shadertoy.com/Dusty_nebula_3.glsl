uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

// "Dusty nebula 3" by Duke
//-------------------------------------------------------------------------------------
// Based on "Protoplanetary disk" (https://www.shadertoy.com/view/MdtGRl) 
// otaviogood's "Alien Beacon" (https://www.shadertoy.com/view/ld2SzK)
// and Shane's "Cheap Cloud Flythrough" (https://www.shadertoy.com/view/Xsc3R4) shaders
// Some ideas came from other shaders from this wonderful site
// Press 1-2-3 to zoom in and zoom out.
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0
//-------------------------------------------------------------------------------------

#define ROTATION

//#define MOUSE_CAMERA_CONTROL

#define DITHERING
#define BACKGROUND

//-------------------
#define pi 3.14159265
#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)

// iq's noise
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = texture2D( iChannel0, (uv+ 0.5)/256.0).yx;
	return 1. - 0.82*mix( rg.x, rg.y, f.z );
}

float fbm(vec3 p)
{
   return noise(p*.06125)*.5 + noise(p*.125)*.25 + noise(p*.25)*.125 + noise(p*.4)*.2;
}

float rand(vec2 co)
{
	return fract(sin(dot(co*0.123,vec2(12.9898,78.233))) * 43758.5453);
}

//=====================================
// otaviogood's noise from https://www.shadertoy.com/view/ld2SzK
//--------------------------------------------------------------
// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
const float nudge = 0.739513;	// size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p)
{
    float n = 0.0;	// noise amount
    float iter = 1.0;
    for (int i = 0; i < 8; i++)
    {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter;	// abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += vec2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        // increase the frequency
        iter *= 1.733733;
    }
    return n;
}

float SpiralNoise3D(vec3 p)
{
    float n = 0.0;
    float iter = 1.0;
    for (int i = 0; i < 5; i++)
    {
        n += (sin(p.y*iter) + cos(p.x*iter)) / iter;
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        iter *= 1.33733;
    }
    return n;
}

float Nebulae(vec3 p)
{
	float final = p.y + 4.5;
    final += SpiralNoiseC(p.zxy*0.123+100.0)*3.0;	// large scale features
    final -= SpiralNoise3D(p);	// more large scale features, but 3d

    return final;
}

float map(vec3 p) 
{
   #ifdef ROTATION
   R(p.xz, iMouse.x*0.008*pi+iTime*0.1);
   #endif
   p.y+=4.1;
   return Nebulae(p) + fbm(p*50.+iTime);
}

// assign color to the media
vec3 computeColor( float density, float radius )
{
	// color based on density alone, gives impression of occlusion within
	// the media
	vec3 result = mix( vec3(1.0,0.9,0.8), vec3(0.4,0.15,0.1), density );
	
	// color added to the media
	vec3 colCenter = 7.*vec3(0.8,1.0,1.0);
	vec3 colEdge = 1.5*vec3(0.48,0.53,0.5);
	result *= mix( colCenter, colEdge, min( (radius+.05)/1.30, 1.15 ) );
	
	return result;
}

bool RaySphereIntersect(vec3 org, vec3 dir, out float near, out float far)
{
	float b = dot(dir, org);
	float c = dot(org, org) - 20.;
	float delta = b*b - c;
	if( delta < 0.0) 
		return false;
	float deltasqrt = sqrt(delta);
	near = -b - deltasqrt;
	far = -b + deltasqrt;
	return far > 0.0;
}

void main( )
{  
    const float KEY_1 = 49.5/256.0;
	const float KEY_2 = 50.5/256.0;
	const float KEY_3 = 51.5/256.0;
    float key = 0.0;
    key += 0.7*texture2D(iChannel1, vec2(KEY_1,0.25)).x;
    key += 0.7*texture2D(iChannel1, vec2(KEY_2,0.25)).x;
    key += 0.7*texture2D(iChannel1, vec2(KEY_3,0.25)).x;

	// ro: ray origin
	// rd: direction of the ray
	vec3 rd = normalize(vec3((xlv_TEXCOORD0.xy-0.5*iResolution.xy)/iResolution.y, 1.));
	vec3 ro = vec3(0., 0., -6.+key*1.6);

    #ifdef MOUSE_CAMERA_CONTROL
    R(rd.yz, -iMouse.y*0.01*pi*2.);
    R(rd.xz, iMouse.x*0.01*pi*2.);
    R(ro.yz, -iMouse.y*0.01*pi*2.);
    R(ro.xz, iMouse.x*0.01*pi*2.);
    #else
    R(rd.yz, -pi*3.93);
    R(rd.xz, pi*3.2);
    R(ro.yz, -pi*3.93);
   	R(ro.xz, pi*3.2);    
    #endif 
    
    #ifdef DITHERING
	vec2 dpos = ( xlv_TEXCOORD0.xy / iResolution.xy );
	vec2 seed = dpos + fract(iTime);
   	// randomizing the length 
    //rd *= (1. + fract(sin(dot(vec3(7, 157, 113), rd.zyx))*43758.5453)*0.1-0.03);
	#endif 
    
	// ld, td: local, total density 
	// w: weighting factor
	float ld=0., td=0., w=0.;

	// t: length of the ray
	// d: distance function
	float d=1., t=0.;
    
    const float h = 0.1;
   
	vec4 sum = vec4(0.0);
   
    float min_dist=0.0, max_dist=0.0;

    if(RaySphereIntersect(ro, rd, min_dist, max_dist))
    {
       
	t = min_dist*step(t,min_dist);
   
	// raymarch loop
	for (int i=0; i<64; i++) 
	{
	 
		vec3 pos = ro + t*rd;
  
		// Loop break conditions.
        if(td>0.9 || d<0.1*t || t>10. || sum.a > 0.99 || t>max_dist) break;
	    
        // evaluate distance function
        float d = map(pos);
		       
		// change this string to control density 
		d = max(d,0.08);
      
		if (d<h) 
		{
			// compute local density 
			ld = h - d;
            
            // compute weighting factor 
			w = (1. - td) * ld;
     
			// accumulate density
			td += w + 1./200.;
		
			float radiusFromCenter = length(pos - vec3(0.0));
			vec4 col = vec4( computeColor(td,radiusFromCenter), td );
		
			// uniform scale density
			col.a *= 0.185;
			// colour by alpha
			col.rgb *= col.a;
			// alpha blend in contribution
			sum = sum + col*(1.0 - sum.a);  
       
		}
      
		td += 1./70.;
       
        // point light calculations
        vec3 ldst = vec3(0.0)-pos;
        float lDist = max(length(ldst), 0.001);

        // star in center
        vec3 lightColor=vec3(1.0,0.5,0.25);
        sum.rgb+=lightColor/(lDist*lDist*6.); //add a bloom around the light

        sum.a *= 0.8;
        
        // enforce minimum stepsize
        d = max(d, 0.1); 
      
        #ifdef DITHERING
        // add in noise to reduce banding and create fuzz
        d=abs(d)*(1.+0.2*rand(seed*vec2(i)));
        #endif 
	  
        //t += max(d * 0.25, 0.02);
        t += max(d * 0.1 * max(length(ldst),2.0), 0.02);
      
	}
    
    // simple scattering
	sum *= 1. / exp( ld * 0.2 ) * 0.6;
        
   	sum = clamp( sum, 0.0, 1.0 );
   
    sum.xyz = sum.xyz*sum.xyz*(3.0-2.0*sum.xyz);
    
	}

    #ifdef BACKGROUND
    // stars background
    if (td<.8)
    {
        vec3 stars = vec3(noise(rd*500.0)*0.5+0.5);
        vec3 starbg = vec3(0.0);
        starbg = mix(starbg, vec3(0.8,0.9,1.0), smoothstep(0.99, 1.0, stars)*clamp(dot(vec3(0.0),rd)+0.75,0.0,1.0));
        starbg = clamp(starbg, 0.0, 1.0);
        sum.xyz += starbg; 
    }
	#endif
    
   gl_FragColor = vec4(sum.xyz,1.0);
}
