uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
//Most of the code come from https://www.shadertoy.com/view/Xss3DS
//noise from here https://www.shadertoy.com/view/MtXSzS
//License Creative Commons Attribution-NonCommercial-ShareAlike 3.0

// change this to get different nebulas :)
#define EXPLOSION_SEED 2.


// the bounding sphere of the nebula. this is less general but means that
// ray cast is only performed for nearby pixels, and raycast can begin from the sphere
// (instead of walking out from the camera)
const float expRadius = 2.7;
const vec3 expCenter = vec3(0.,expRadius,0.);

/* original
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 permute(vec4 x) { return mod289(((x*34.0)+1.0)*x); }
vec4 taylorInvSqrt(vec4 r){ return 1.79284291400159 - 0.85373472095314 * r; }

float noise(vec3 v)
{
	const vec2  C = vec2(1.0/6.0, 1.0/3.0);
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);
	// First corner
	vec3 i  = floor(v + dot(v, C.yyy));
	vec3 x0 = v - i + dot(i, C.xxx);
	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min(g.xyz, l.zxy);
	vec3 i2 = max(g.xyz, l.zxy);
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y
	// Permutations
	i = mod289(i);
	vec4 p = permute( permute( permute( i.z + vec4(0.0, i1.z, i2.z, 1.0)) + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float n_ = 0.142857142857; // 1.0/7.0
	vec3  ns = n_ * D.wyz - D.xzx;
	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)
	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_);    // mod(j,N)
	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);
	vec4 b0 = vec4(x.xy, y.xy);
	vec4 b1 = vec4(x.zw, y.zw);
	vec4 s0 = floor(b0) * 2.0 + 1.0;
	vec4 s1 = floor(b1) * 2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));
	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;
	vec3 p0 = vec3(a0.xy, h.x);
	vec3 p1 = vec3(a0.zw, h.y);
	vec3 p2 = vec3(a1.xy, h.z);
	vec3 p3 = vec3(a1.zw, h.w);
	//Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	// Mix final noise value
	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	m = m * m;
	return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3)));
}
*/

// iq's noise
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = texture( iChannel1, (uv+ 0.5)/256.0, -100.0 ).yx;
	return -1.0+2.0*mix( rg.x, rg.y, f.z );
}

// assign colour to the media
vec3 computeColour( float density, float radius )
{
	// these are almost identical to the values used by iq
	
	// colour based on density alone. gives impression of occlusion within
	// the media
	vec3 result = mix( 1.1*vec3(1.0,0.9,0.8), vec3(0.4,0.15,0.1), density );
	
	// colour added for nebula
	vec3 colBottom = 3.1*vec3(0.8,1.0,1.0);
	vec3 colTop = 2.*vec3(0.48,0.53,0.5);
	result *= mix( colBottom*2.0, colTop, min( (radius+.5)/1.7, 1.0 ) );
	
	return result;
}

// maps 3d position to colour and density
float densityFn( in vec3 p, in float r, out float rawDens, in float rayAlpha )
{
    float l = length(p);
	// density has dependency on mouse y coordinate (linear radial ramp)
	float mouseIn = 0.75;
	if( iMouse.z > 0.0 )
		mouseIn = iMouse.y/iResolution.y;
	float mouseY = 1.0 - mouseIn;
    float den = 1. - 1.5*r*(4.*mouseY+.5);
    
	// offset noise based on seed
    float t = EXPLOSION_SEED;
    vec3 dir = vec3(0.,1.,0.);
    
    // participating media    
    float f;
    vec3 q = p - dir* t; f  = 0.50000*noise( q );
	q = q*2.02 - dir* t; f += 0.25000*noise( q );
	q = q*2.03 - dir* t; f += 0.12500*noise( q );
	q = q*2.40 - dir* t; f += 0.06250*noise( q );
    q = q*2.50 - dir* t; f += 0.03125*noise( q );
    
	// add in noise with scale factor
	rawDens = den + 4.0*f*l;
	
    den = clamp( rawDens, 0.0, 1.0 );
    
    //if (den>0.9) den = -3.*den;
    
	// thin out the volume at the far extends of the bounding sphere to avoid
	// clipping with the bounding sphere
	den *= l*0.6-smoothstep(0.8,0.,r/expRadius);
	
	return den;
}

vec4 raymarch( in vec3 rayo, in vec3 rayd, in float expInter, in vec2 FragCoord )
{
    vec4 sum = vec4( 0.0 );
     
    float step = 0.075;
     
    // dither start pos to break up aliasing
	vec3 pos = rayo + rayd * (expInter + step*texture( iChannel0, FragCoord.xy/iChannelResolution[0].x ).x);
	
    for( int i=0; i<48; i++ )
    {
        if( sum.a > 0.99 ) continue;
		
		float radiusFromExpCenter = length(pos - expCenter);
		
		if( radiusFromExpCenter > expRadius+0.01 ) continue;
		
		float dens, rawDens;
		
        dens = densityFn( pos, radiusFromExpCenter, rawDens, sum.a );
		
		vec4 col = vec4( computeColour(dens,radiusFromExpCenter), dens );
		
		// uniform scale density
		col.a *= 0.2;
		
		// colour by alpha
		col.rgb *= col.a/0.8;
		
		// alpha blend in contribution
		sum = sum + col*(1.0 - sum.a);  
		
		// take larger steps through negative densities.
		// something like using the density function as a SDF.
		float stepMult = 1. + 2.5*(1.-clamp(rawDens+1.,1.,1.));
		
		// step along ray
		pos += rayd * step * stepMult;
    }
	
    return clamp( sum, 0.0, 1.0 );
}


// iq's sphere intersection
float iSphere(in vec3 ro, in vec3 rd, in vec4 sph)
{
	//sphere at origin has equation |xyz| = r
	//sp |xyz|^2 = r^2.
	//Since |xyz| = ro + t*rd (where t is the parameter to move along the ray),
	//we have ro^2 + 2*ro*rd*t + t^2 - r2. This is a quadratic equation, so:
	vec3 oc = ro - sph.xyz; //distance ray origin - sphere center
	
	float b = dot(oc, rd);
	float c = dot(oc, oc) - sph.w * sph.w; //sph.w is radius
	float h = b*b - c; // delta
	if(h < 0.0) 
		return -1.0;
	float t = (-b - sqrt(h)); //Again a = 1.

	return t;
}

vec3 computePixelRay( in vec2 p, out vec3 cameraPos )
{
    // camera orbits around nebula
	
    float camRadius = 3.8;
	// use mouse x coord
	float a = iTime*20.;
	if( iMouse.z > 0. )
		a = iMouse.x;
	float theta = -(a-iResolution.x)/80.;
    float xoff = camRadius * cos(theta);
    float zoff = camRadius * sin(theta);
    cameraPos = vec3(xoff,expCenter.y,zoff);
     
    // camera target
    vec3 target = vec3(0.,expCenter.y,0.);
     
    // camera frame
    vec3 fo = normalize(target-cameraPos);
    vec3 ri = normalize(vec3(fo.z, 0., -fo.x ));
    vec3 up = normalize(cross(fo,ri));
     
    // multiplier to emulate a fov control
    float fov = .5;
	
    // ray direction
    vec3 rayDir = normalize(fo + fov*p.x*ri + fov*p.y*up);
	
	return rayDir;
}

void main( )
{
	// get aspect corrected normalized pixel coordinate
    vec2 q = gl_FragCoord.xy / iResolution.xy;
    vec2 p = -1.0 + 2.0*q;
    p.x *= iResolution.x / iResolution.y;
    
	vec3 rayDir, cameraPos;
	
    rayDir = computePixelRay( p, cameraPos );
	
	vec4 col = vec4(0.);
	
    // does pixel ray intersect with exp bounding sphere?
	float boundingSphereInter = iSphere( cameraPos, rayDir, vec4(expCenter,expRadius) );
	if( boundingSphereInter > 0. )
	{
		// yes, cast ray
	    col = raymarch( cameraPos, rayDir, boundingSphereInter,gl_FragCoord );
	}
	
    // smoothstep final color to add contrast
    gl_FragColor = vec4(col.xyz*col.xyz*(3.0-2.0*col.xyz),1.0);
}

