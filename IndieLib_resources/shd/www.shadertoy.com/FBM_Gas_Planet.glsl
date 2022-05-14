uniform sampler2D iChannel0;
//precision mediump float;

#define MAX_STEPS 255
#define MIN_DIST 0.0
#define MAX_DIST 100.0
#define EPSILON 0.00001

#define PI 3.141592654


float sphereSDF(vec3 p,  float r) 
{
    return length(p) - r;
}


// Combinations
float unionOp(float d1, float d2) { return min(d1,d2); }
vec2 unionOp(vec2 d1, vec2 d2) { return (d1.x<d2.x) ? d1 : d2; }


vec2 map(vec3 p)
{
	vec2 result = vec2(1e10, -2.0);
	{
		result = unionOp(result, vec2(sphereSDF(p, 3.), 1.));
	}
	
	return result;
}

const float sunIntensity = .7;
const vec3 sunColor = sunIntensity*vec3(1.30,1.00,0.60);

float seed = 437585453.123;
float hash21(vec2 p) 
{
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  float f = dot (p, vec2 (127.1, 351.7));
  return fract(sin(f)*a);
}

vec2 hash2(vec2 p) 
{
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  p = vec2(dot (p, vec2 (127.1, 351.7)), dot(p, vec2(245.5, 304.4)));
  return fract(sin(p)*a);
}

vec3 hash23(vec2 p) {
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  vec3 p3 = vec3(dot (p, vec2 (127.1, 351.7)), dot (p, vec2 (261.5, 183.3)), dot(p, vec2(245.5, 304.4)));
  return fract(sin(p3)*a);
}

vec2 hash32(vec3 p) {
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  vec2 p2 = vec2(dot (p, vec3 (127.1, 351.7, 251.45)), dot (p, vec3 (261.5, 183.3, 376.89)));
  return fract(sin(p2)*a);
}
float hash31(vec3 p) {
  p = mod(p, PI*20.);
  float a = (mod(43758.5453123 + seed, 37562.) + 13548.);
  float f = dot (p, vec3 (127.1, 351.7, 251.45));
  return fract(sin(f)*a);
}


float ease(float t)
{
  return t * t * t * (t * (t * 6. - 15.) + 10.);
}
vec2 ease(vec2 t)
{
  return vec2(ease(t.x), ease(t.y));
}
vec3 ease(vec3 t)
{
  return vec3(ease(t.x), ease(t.y), ease(t.z));
}
vec4 ease(vec4 t)
{
  return vec4(ease(t.x), ease(t.y), ease(t.z), ease(t.w));
}

float noise(vec2 point){
    vec2 fr = ease(fract(point));
    vec2 hi = ceil(point);
    vec2 lo = floor(point);
    
    return mix(
        mix(
            hash21(lo), 
            hash21(vec2(hi.x , lo.y)), 
            fr.x
        ), 
        mix(
            hash21(vec2(lo.x , hi.y)),
            hash21(hi),
            fr.x
        ), 
        fr.y
    );
}

float noise(vec3 point){
    vec3 fr = ease(fract(point));
    vec3 hi = ceil(point);
    vec3 lo = floor(point);
    
    return mix(
        mix(
            mix(
                hash31(lo), 
                hash31(vec3(hi.x , lo.yz)), 
                fr.x
            ), 
            mix(
                hash31(vec3(lo.x , hi.y, lo.z)),
                hash31(vec3(hi.xy, lo.z)),
                fr.x
            ), 
            fr.y
        ),
        mix(
            mix(
                hash31(vec3(lo.xy, hi.z)), 
                hash31(vec3(hi.x , lo.y, hi.z)), 
                fr.x
            ), 
            mix(
                hash31(vec3(lo.x , hi.yz)),
                hash31(hi),
                fr.x
            ), 
            fr.y
        ),
        fr.z
    );
}
/*
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	
	vec2 uv = (p.xy+vec2(37.0,17.0) * p.z) + f.xy;
	vec2 rg = texture( iChannel0, (uv + 0.5)/256.0, -100.0 ).yx;
	return mix( rg.x, rg.y, f.z );
}
*/

// https://www.shadertoy.com/view/Md2SR3

// Return random noise in the range [0.0, 1.0], as a function of x.
float Noise2d( in vec2 x )
{
    float xhash = cos( x.x * 37.0 );
    float yhash = cos( x.y * 57.0 );
    return fract( 415.92653 * ( xhash + yhash ) );
}

// Convert Noise2d() into a "star field" by stomping everthing below fThreshhold to zero.
float NoisyStarField( in vec2 vSamplePos, float fThreshhold )
{
    float StarVal = Noise2d( vSamplePos );
    if ( StarVal >= fThreshhold )
        StarVal = pow( (StarVal - fThreshhold)/(1.0 - fThreshhold), 6.0 );
    else
        StarVal = 0.0;
    return StarVal;
}

// Stabilize NoisyStarField() by only sampling at integer values.
float StableStarField( in vec2 vSamplePos, float fThreshhold )
{
    // Linear interpolation between four samples.
    // Note: This approach has some visual artifacts.
    // There must be a better way to "anti alias" the star field.
    float fractX = fract( vSamplePos.x );
    float fractY = fract( vSamplePos.y );
    vec2 floorSample = floor( vSamplePos );    
    float v1 = NoisyStarField( floorSample, fThreshhold );
    float v2 = NoisyStarField( floorSample + vec2( 0.0, 1.0 ), fThreshhold );
    float v3 = NoisyStarField( floorSample + vec2( 1.0, 0.0 ), fThreshhold );
    float v4 = NoisyStarField( floorSample + vec2( 1.0, 1.0 ), fThreshhold );

    float StarVal =   v1 * ( 1.0 - fractX ) * ( 1.0 - fractY )
        			+ v2 * ( 1.0 - fractX ) * fractY
        			+ v3 * fractX * ( 1.0 - fractY )
        			+ v4 * fractX * fractY;
	return StarVal;
}

vec3 background(vec3 q)
{
	return vec3(StableStarField(q.xy * iResolution.xy * 2., 0.99));
    
}

// ============================================================== //

// intersect ray with the scene
vec2 raycast(vec3 ro, vec3 rd)
{
	vec2 result = vec2(-1.0);
	float t = MIN_DIST;
	for (int i =0; i < MAX_STEPS && t < MAX_DIST; i++) {
		vec2 h = map(ro + t*rd);
		if (abs(h.x) < (EPSILON*t)) {
			result = vec2(t, h.y);
			break;
		}
		t += h.x;
	}
	
	return result;
}

// get direction of ray with just device coordinates
vec3 rayDirection(float fieldOfView, vec2 size, vec2 xlv_TEXCOORD0) {
    vec2 xy = xlv_TEXCOORD0 - size / 2.;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}


// calculate the normal via finite differences
vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map( pos + e.xyy ).x + 
					  e.yyx*map( pos + e.yyx ).x + 
					  e.yxy*map( pos + e.yxy ).x + 
					  e.xxx*map( pos + e.xxx ).x );
}

mat4 viewMatrix(vec3 eye, vec3 center, vec3 up) {
    // Based on gluLookAt man page
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}


float calcSoftshadow( in vec3 ro, in vec3 rd, in float mint, in float tmax )
{
    // bounding volume
    float tp = (0.8-ro.y)/rd.y; if( tp>0.0 ) tmax = min( tmax, tp );

    float res = 1.0;
    float t = mint;
    for( int i=0; i<24; i++ )
    {
		float h = map( ro + rd*t ).x;
        float s = clamp(8.0*h/t,0.0,1.0);
        res = min( res, s*s*(3.0-2.0*s) );
        t += clamp( h, 0.02, 0.2 );
        if( res<0.004 || t>tmax ) break;
    }
    return clamp( res, 0.0, 1.0 );
}




float calcAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float h = 0.01 + 0.12*float(i)/4.0;
        float d = map( pos + h*nor ).x;
        occ += (h-d)*sca;
        sca *= 0.95;
        if( occ>0.35 ) break;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 ) * (0.5+0.5*nor.y);
}


const float R_INNER = 2.85;
const float R = R_INNER + .55;

const int NUM_OUT_SCATTER = 12;
const int NUM_IN_SCATTER = 35;

float density( vec3 p, float ph ) {
	return exp( -max( length( p ) - R_INNER, 0.0 ) / ph );
}

float optic( vec3 p, vec3 q, float ph ) {
	vec3 s = ( q - p ) / float( NUM_OUT_SCATTER );
	vec3 v = p + s * 0.5;
	
	float sum = 0.0;
	for ( int i = 0; i < NUM_OUT_SCATTER; i++ ) {
		sum += density( v, ph );
		v += s;
	}
	sum *= length( s );
	
	return sum;
}


float phase_mie( float g, float c, float cc ) {
	float gg = g * g;
	
	float a = ( 1.0 - gg ) * ( 1.0 + cc );

	float b = 1.0 + gg - 2.0 * g * c;
	b *= sqrt( b );
	b *= 2.0 + gg;	
	
	return ( 3.0 / 8.0 / PI ) * a / b;
}

// Rayleigh
// g : 0
// F = 3/16PI * ( 1 + c^2 )
float phase_ray( float cc ) {
	return ( 3.0 / 16.0 / PI ) * ( 1.0 + cc );
}

vec3 in_scatter( vec3 o, vec3 dir, float t, vec3 l ) {
	const float ph_ray = 0.05;
    const float ph_mie = 0.02;
    
    const vec3 k_ray = vec3( 33.8, 17.5, 14.1 );
    const vec3 k_mie = vec3( 21.0 );
    const float k_mie_ex = 1.1;
    
	vec3 sum_ray = vec3( 0.0 );
    vec3 sum_mie = vec3( 0.0 );
    
    float n_ray0 = 0.0;
    float n_mie0 = 0.0;
    
	float len = 5. / float( NUM_IN_SCATTER );
    vec3 s = dir * len;
	vec3 v = o + dir * ( t + len * 0.5 );
    
    for ( int i = 0; i < NUM_IN_SCATTER; i++, v += s ) {   
		float d_ray = density( v, ph_ray ) * len;
        float d_mie = density( v, ph_mie ) * len;
        
        n_ray0 += d_ray;
        n_mie0 += d_mie;
        
		vec3 u = v + l * t;
        
        float n_ray1 = optic( v, u, ph_ray );
        float n_mie1 = optic( v, u, ph_mie );
		
        vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray - ( n_mie0 + n_mie1 ) * k_mie * k_mie_ex );
        
		sum_ray += d_ray * att;
        sum_mie += d_mie * att;
	}
	
	float c  = dot( dir, -l );
	float cc = c * c;
    vec3 scatter =
        sum_ray * k_ray * phase_ray( cc ) +
     	sum_mie * k_mie * phase_mie( -0.78, c, cc );
    
	
	return 7.0 * scatter;
}

vec3 light = normalize( vec3(-0.5, 0.4, 0.6) );


vec3 render(vec3 ro, vec3 rd, vec3 rdx, vec3 rdy)
{
	// background
	vec3 col = background(rd);
	
	vec2 tmat = raycast(ro, rd);
	float t = tmat.x;
	float m = tmat.y;
    

	if (m > -1.) {
	
		
		col = 0.2 + 0.2*sin( m*2.0 + vec3(0.0,1.0,2.0) );
        float ks = 1.0;
	
		vec3 pos = ro + tmat.x*rd;
		vec3  hal = normalize(light - rd );
		vec3 norm = (m < 1.5) ? vec3(0.0,1.0,0.0) : calcNormal( pos );
		vec3 ref = reflect( rd, norm );
		vec3 lin = vec3(0.0);
        
		if( m<1.5 )
        {
            float u = atan(pos.z, pos.x) / (2.*PI) + 0.5;
			float v = -asin(pos.y / PI);
            vec2 UV = vec2(u,-v);
            
            col = texture2D( iChannel0, UV).xyz;
            lin += in_scatter(ro, rd, t, light);
            ks = 0.1;
        }
        
        
		
		float occ = 1.0; 
		
		
		// sun
		{
            vec3  hal = normalize( light-rd );
            float dif = clamp( dot( norm, light ), 0.0, 1.0 );
          //if( dif>0.0001 )
        	      dif *= calcSoftshadow( pos, light, 0.02, 2.5 );
			float spe = pow( clamp( dot( norm, hal ), 0.0, 1.0 ),16.0);
                  spe *= dif;
                  spe *= 0.04+0.16*pow(clamp(1.0-dot(hal,light),0.0,1.0),2.0);
            lin += col*2.20*dif*sunColor;
            lin +=     5.00*spe*sunColor*ks;
        }
        

        col = lin;

		// falloff (fakes a depth blur)
        col = mix( col, vec3(0.7,0.7,0.9), 1.0-exp( -0.00002*t*t*t ) );
	}
	
	return vec3( clamp(col,0.0,1.0) );
}

void main( ) {
	vec3 viewDir = rayDirection(45.0, iResolution.xy, xlv_TEXCOORD0);
    vec3 eye = vec3(8.0, 6.0, 18.0);
    
    mat4 viewToWorld = viewMatrix(eye, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    
    vec3 worldDir = (viewToWorld * vec4(viewDir, 0.0)).xyz;
    
  	// ray derivatives
    vec2 px = (2.0*(xlv_TEXCOORD0+vec2(1.0,0.0))-iResolution.xy)/iResolution.y;
    vec2 py = (2.0*(xlv_TEXCOORD0+vec2(0.0,1.0))-iResolution.xy)/iResolution.y;
    vec3 rdx = (viewToWorld * normalize( vec4(px,2.5, 1.0) )).xyz;
    vec3 rdy = (viewToWorld * normalize( vec4(py,2.5, 1.0) )).xyz;
    
    vec3 col = render(eye, worldDir, rdx, rdy);

	gl_FragColor = vec4(col, 1.0);
}

