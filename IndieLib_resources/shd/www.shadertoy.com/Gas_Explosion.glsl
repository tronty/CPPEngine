uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;

// Based on https://www.shadertoy.com/view/lsySzd
//
// "Volumetric explosion" by Duke
//-------------------------------------------------------------------------------------
// Based on "Supernova remnant" (https://www.shadertoy.com/view/MdKXzc) 
// and other previous shaders 
// otaviogood's "Alien Beacon" (https://www.shadertoy.com/view/ld2SzK)
// and Shane's "Cheap Cloud Flythrough" (https://www.shadertoy.com/view/Xsc3R4) shaders
// Some ideas came from other shaders from this wonderful site
// Press 1-2-3 to zoom in and zoom out.
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
//-------------------------------------------------------------------------------------
//
// And based on https://www.shadertoy.com/view/XlcGRn
//
// "Space Elevator" by dr2 - 2016
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
//
// And based on https://www.shadertoy.com/view/4djSRW
//
// Hash without Sine
// Creative Commons Attribution-ShareAlike 4.0 International Public License
// Created by David Hoskins.
//
// With varous small tweaks to have the effect play nice against a colorful background,
// keeping the black smoky look.
//
// Explosion colors
const vec3 veDensityBeg  = vec3(0.22,0.16,0.046);
const vec3 veDensityEnd  = vec3(0.403,0.291,0.216);
const vec3 veMediaCenter = vec3(0.305,0.49,0.616);
const vec3 veMediaEdge   = vec3(0.918,0.796,0.495);
// Sky colors
const vec3 veColSkyHigh = vec3(0.0, 0.1, 0.3);
const vec3 veColSkyLow  = vec3(0.2, 0.4, 0.6);
const vec3 veColStars   = vec3(0.8, 0.9, 1.0);
const float accuscale = -0.005;
const float scalestar = 18.1  ;
const float subdiv    = 11.714;
const float variable  = 0.612 ;
const float variable2 = 1.171 ;
// Default
const bool veBOTH         = !false;
const bool veLEFT         = !true ;
const bool veLOW_QUALITY  = false;
const bool veDITHERING    = !true ;
const bool veTONEMAPPING  = false;
const bool veBackground   = true ;
const float veBackgroundCutoff      = 0.891;
const float veBackgroundCutoffStart = 0.387;

vec4 iiMouse;
#define iMouse iiMouse
//-------------------
#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)

#define pi 3.14159265

// Sign function that doesn't return 0
float sgn(float x) {
    return (x < 0.) ? -1. : 1.;
}
vec2 sgn(vec2 v) {
    return vec2(sgn(v.x),sgn(v.y));
}
vec3 sgn(vec3 v) {
    return vec3(sgn(v.x),sgn(v.y),sgn(v.z));
}

void veR( inout vec2 p, in float a) {
	p = cos(a) * p + sin(a) * vec2(p.y, -p.x);
}

// iq's noise
float veNoise( in vec3 x) {
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0 - 2.0 * f);
	vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
	vec2 rg = textureLod( iChannel0, (uv+.5)/256., 0.).yx;
	return 1. - 0.82 * mix(rg.x, rg.y, f.z);
}

float veFBM(vec3 p) {
	return veNoise(p * .06125) * .5 + veNoise(p * .125) * .25
			+ veNoise(p * .25) * .125 + veNoise(p * .4) * .2;
}

float veSphere(vec3 p, float r) {
	return length(p) - r;
}
float veExplosionRadius(float maxRadius, float veProgress) {
    return mix(1.0/8.0,1.0, sqrt(veProgress))*maxRadius;
}

//==============================================================
// otaviogood's noise from https://www.shadertoy.com/view/ld2SzK
//--------------------------------------------------------------
// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
const float veNudge = 100.3 * 4.;	// size of perpendicular vector
float veNormalizer = 1.0 / sqrt(1.0 + veNudge * veNudge);	// pythagorean theorem on that perpendicular to maintain scale
float veSpiralNoiseC(vec3 p) {
	float n = 0.0; // noise amount
	float iter = 2.0;
	for (int i = 0; i < 8; i++) {
		// add sin and cos scaled inverse with the frequency
		n += -abs(sin(p.y * iter) + cos(p.x * iter)) / iter; // abs for a ridged look
		// rotate by adding perpendicular and scaling down
		p.xy += vec2(p.y, -p.x) * veNudge;
		p.xy *= veNormalizer;
		// rotate on other axis
		p.xz += vec2(p.z, -p.x) * veNudge;
		p.xz *= veNormalizer;
		// increase the frequency
		iter *= 1.733733;
	}
	return n;
}

float veVolumetricExplosion(vec3 p, float radius, float maxRadius, float veProgress) {
    float sdSphere = veSphere(p, radius)*(4. * 1.92 *.25/1.92 * 1.0*        veExplosionRadius(8.0,1.0)/maxRadius);
    float noise1 = (veLOW_QUALITY) ? veNoise(p * 12.5) * .2 : veFBM(p * 50.);
    float age = mix(2.0, -7.5, 1.-pow(1.-veProgress,2.5));
    float noise2 = veSpiralNoiseC(p.zxy * 0.4132 + 0.333*vec3(0,0,1) * (25.*veProgress+max(0.,iMouse.x-15.0) * 0.1));
    float result = min(0.,sdSphere)*.999 +
        (0.25+1.75*veProgress+3.*veProgress*veProgress+4.*veProgress*veProgress*veProgress)*max(0.,sdSphere) +
        0.999*noise1 +
        0.999*age +
        0.999*(noise2+1.0/1.25) * 1.25;
	return result;
}

float veMap(vec3 p, float maxRadius, float count, float veProgress) {
	veR(p.yz, -6. * (1.-sqrt(veProgress)));
	veR(p.zx, count + iMouse.x * 0.008 * pi + 0.0*4. * veProgress);
    //p += so;
    //p -= so;
    if(maxRadius == 1.0) {
    return veVolumetricExplosion(p * 1.*8.0, 
                                               4. * 1.92 *.25/1.92 * veProgress* veExplosionRadius(8.0,veProgress),
                                               4. * 1.92 *.25/1.92 * 1.0*        veExplosionRadius(8.0,1.0),
                                               veProgress)/ 1.;
    }
    float VolExplosion = veVolumetricExplosion(p * 1.5, 
                                               4. * 1.92 *.25/1.92 * veProgress* veExplosionRadius(maxRadius,veProgress),
                                               4. * 1.92 *.25/1.92 * 1.0*        veExplosionRadius(maxRadius,1.0),
                                               veProgress)/ 1.5;
	return VolExplosion;
}
//--------------------------------------------------------------

// assign color to the media
vec3 veComputeColor(float density, float radius, float maxRadius) {
	// color based on density alone, gives impression of occlusion within
	// the media
	vec3 result = mix(veDensityBeg, veDensityEnd, 1. - density);

	// color added to the media
	vec3 colCenter = 7. * veMediaCenter;
	vec3 colEdge = 1.5 * veMediaEdge;
	result *= mix(colCenter, colEdge, min((radius + .05) / .9, 1.15));

	return result;
}

vec3 veSphereIntersect(vec3 ro, vec3 rd, vec3 so, float radius) {
    vec3 sr = ro - so;
    float tsr = dot(sr, rd);
    float delta = radius * radius - dot(sr, sr) + tsr * tsr;
    float deltasqrt = (delta < 0.0) ? 0.0f : sqrt(delta);
    float t0 = -tsr - deltasqrt;
    float t1 = -tsr + deltasqrt;
    return vec3(t0, t1, -tsr);
}

// Applies the filmic curve from John Hable's presentation
// More details at : http://filmicgames.com/archives/75
vec3 veToneMapFilmicALU(vec3 _color) {
	_color = max(vec3(0), _color - vec3(0.004));
	_color = (_color * (6.2 * _color + vec3(0.5)))
			/ (_color * (6.2 * _color + vec3(1.7)) + vec3(0.06));
	return _color;
}

vec3 veAddZoom(vec3 rd, vec3 ro) {
	const float KEY_1 = 49.5 / 256.0;
	const float KEY_2 = 50.5 / 256.0;
	const float KEY_3 = 51.5 / 256.0;
	float key = 0.0;
	key += 0.7 * texture(iChannel1, vec2(KEY_1, 0.25)).x;
	key += 1.4 * texture(iChannel1, vec2(KEY_2, 0.25)).x;
	key += 2.1 * texture(iChannel1, vec2(KEY_3, 0.25)).x;
	return ro - (ro - rd) * key * 1.6 / 6.;
}

const vec4 vecHashA4 = vec4(0., 1., 57., 58.);
const vec3 vecHashA3 = vec3(1., 57., 113.);
const float vecHashM = 43758.54;

vec4 veHashv4f(float p) {
	return fract(sin(p + vecHashA4) * vecHashM);
}

float veNoisefv2(vec2 p) {
	vec4 t;
	vec2 ip, fp;
	ip = floor(p);
	fp = fract(p);
	fp = fp * fp * (3. - 2. * fp);
	t = veHashv4f(dot(ip, vecHashA3.xy));
	return mix(mix(t.x, t.y, fp.x), mix(t.z, t.w, fp.x), fp.y);
}

float veNoisefv3(vec3 p) {
	vec4 t1, t2;
	vec3 ip, fp;
	float q;
	ip = floor(p);
	fp = fract(p);
	fp = fp * fp * (3. - 2. * fp);
	q = dot(ip, vecHashA3);
	t1 = veHashv4f(q);
	t2 = veHashv4f(q + vecHashA3.z);
	return mix(mix(mix(t1.x, t1.y, fp.x), mix(t1.z, t1.w, fp.x), fp.y),
			mix(mix(t2.x, t2.y, fp.x), mix(t2.z, t2.w, fp.x), fp.y), fp.z);
}

float veFbm3(vec3 p) {
	float f, a;
	f = 0.;
	a = 1.;
	for (int i = 0; i < 5; i++) {
		f += a * veNoisefv3(p);
		a *= 0.5;
		p *= 2.;
	}
	return f;
}

float veFbmn(vec3 p, vec3 n) {
	vec3 s;
	float a;
	s = vec3(0.);
	a = 1.;
	for (int i = 0; i < 5; i++) {
		s += a * vec3(veNoisefv2(p.yz), veNoisefv2(p.zx), veNoisefv2(p.xy));
		a *= 0.5;
		p *= 2.;
	}
	return dot(s, abs(n));
}

vec3 veSpaceCol(vec3 ro, vec3 rd) {
	vec3 rds, col;
	rds = floor(2000. * rd);
	rds = 0.00015 * rds + 0.1 * veNoisefv3(0.0005 * rds.yzx);
	for (int j = 0; j < 19; j++)
		rds = abs(rds) / dot(rds, rds) - 0.9;
	col = veColStars * min(1., 0.5e-3 * pow(min(6., length(rds)), 5.));
	float s = pow(max(0., abs(dot(rd, vec3(0.0, 1.0, 0.0)))), 1.60);
	col = veColSkyLow + col * (1. - smoothstep(0.9, 1., s));
	s = pow(s, 128.);
	col += veColSkyHigh * (0.2 * s + 0.9 * pow(s, 8.));
	return col;
}
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//  1 out, 1 in...
float hash11(float p) {
	vec3 p3 = fract(vec3(p) * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash12(vec2 p) {
	vec3 p3 = fract(vec3(p.xyx) * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash13(vec3 p3) {
	p3 = fract(p3 * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  2 out, 1 in...
vec2 hash21(float p) {
	vec3 p3 = fract(vec3(p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract(vec2((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y));
}

//----------------------------------------------------------------------------------------
///  2 out, 2 in...
vec2 hash22(vec2 p) {
	vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract(vec2((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y));
}

//----------------------------------------------------------------------------------------
///  2 out, 3 in...
vec2 hash23(vec3 p3) {
	p3 = fract(p3 * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract(vec2((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y));
}

//----------------------------------------------------------------------------------------
//  3 out, 1 in...
vec3 hash31(float p) {
	vec3 p3 = fract(vec3(p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract(
			vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}

//----------------------------------------------------------------------------------------
///  3 out, 2 in...
vec3 hash32(vec2 p) {
	vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
	p3 += dot(p3, p3.yxz + 19.19);
	return fract(
			vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}

//----------------------------------------------------------------------------------------
///  3 out, 3 in...
vec3 hash33(vec3 p3) {
	p3 = fract(p3 * HASHSCALE3);
	p3 += dot(p3, p3.yxz + 19.19);
	return fract(
			vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}

//----------------------------------------------------------------------------------------
// 4 out, 1 in...
vec4 hash41(float p) {
	vec4 p4 = fract(vec4(p) * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return fract(
			vec4((p4.x + p4.y) * p4.z, (p4.x + p4.z) * p4.y, (p4.y + p4.z) * p4.w,
					(p4.z + p4.w) * p4.x));

}

//----------------------------------------------------------------------------------------
// 4 out, 2 in...
vec4 hash42(vec2 p) {
	vec4 p4 = fract(vec4(p.xyxy) * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return fract(
			vec4((p4.x + p4.y) * p4.z, (p4.x + p4.z) * p4.y, (p4.y + p4.z) * p4.w,
					(p4.z + p4.w) * p4.x));
}

//----------------------------------------------------------------------------------------
// 4 out, 3 in...
vec4 hash43(vec3 p) {
	vec4 p4 = fract(vec4(p.xyzx) * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return fract(
			vec4((p4.x + p4.y) * p4.z, (p4.x + p4.z) * p4.y, (p4.y + p4.z) * p4.w,
					(p4.z + p4.w) * p4.x));
}

//----------------------------------------------------------------------------------------
// 4 out, 4 in...
vec4 hash44(vec4 p4) {
	p4 = fract(p4 * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return fract(
			vec4((p4.x + p4.y) * p4.z, (p4.x + p4.z) * p4.y, (p4.y + p4.z) * p4.w,
					(p4.z + p4.w) * p4.x));
}
// From iq's website
float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
	vec3 pa = p - a, ba = b - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h) - r;
}
float parabola(float x, float k) {
	return pow(4.0 * x * (1.0 - x), k);
}
vec4 parabola(vec4 x, float k) {
	return pow(4.0 * x * (1.0 - x), vec4(k));
}
vec3 parabola(vec3 x, float k) {
	return pow(4.0 * x * (1.0 - x), vec3(k));
}
float pcurve(float x, float a, float b) {
	float k = pow(a + b, a + b) / (pow(a, a) * pow(b, b));
	return k * pow(x, a) * pow(1.0 - x, b);
}
vec3 veStars(vec3 ro, vec3 rd) {
	vec3 bs[8];
	vec3 srd = rd;
	float size = floor(subdiv);
	rd *= length(rd) / max(abs(rd.x), max(abs(rd.y), abs(rd.z)));
	rd *= size;
	vec3 lower = floor(rd + 0.5);
	vec3 point = rd - lower;
	vec3 ba = rd - ro;
	vec3 nba = normalize(ba);
	float ilba = 1.0 / dot(ba, ba);
	vec3 nbatilba = nba * ilba;
	float d = 99999999.0;
    
	for (int xi = -1; xi <= 1; xi++) {
		for (int yi = -1; yi <= 1; yi++) {
			for (int zi = -1; zi <= 1; zi++) {
				for (int i = 0; i < 5; i++) {
					vec3 box = lower + vec3(float(xi), float(yi), float(zi));
					vec4 a = vec4(0.0);
					for (int t = 0; t < 4; t++) {
						float v = float(t + 1) * .152;
						vec3 pos = (box * v + float(5 * i));
						a += hash43(pos);
					}
					vec3 local = a.xyz / 4.0 - 0.5;
					float dist = sdCapsule(ro + srd, ro, ro + box + local,
							a.w * accuscale);
					d = min(d, dist);
				}
			}
		}
	}
	vec3 result = pow(
			clamp(pcurve(d * scalestar, 0.0, 1.0 + 31.0 * variable), 0.0, 1.0),
			variable2) * veColStars;
	return result;
}
// Repeat only a few times: from indices <start> to <stop> (similar to above, but more flexible)
float pModInterval1(inout float p, float size, float start, float stop) {
	float halfsize = size*0.5;
	float c = floor((p + halfsize)/size);
	p = mod(p+halfsize, size) - halfsize;
	if (c > stop) { //yes, this might not be the best thing numerically.
		p += size*(c - stop);
		c = stop;
	}
	if (c <start) {
		p += size*(c - start);
		c = start;
	}
	return c;
}

// http://iquilezles.org/www/articles/boxfunctions/boxfunctions.htm
vec4 iBox( in vec3 ro, in vec3 rd, in mat4 txx, in mat4 txi, in vec3 rad ) 
{
    // convert from ray to box space
	vec3 rdd = (txx*vec4(rd,0.0)).xyz;
	vec3 roo = (txx*vec4(ro,1.0)).xyz;

	// ray-box intersection in box space
    vec3 m = 1.0/rdd;
    vec3 n = m*roo;
    vec3 k = abs(m)*rad;
	
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	
	if( tN > tF || tF < 0.0) return vec4(-1.0);

	vec3 nor = -sign(rdd)*step(t1.yzx,t1)*step(t1.zxy,t1);

    // convert to ray space
	
	nor = (txi * vec4(nor,0.0)).xyz;

	return vec4( tN, nor );
}


// http://iquilezles.org/www/articles/boxfunctions/boxfunctions.htm
float sBox( in vec3 ro, in vec3 rd, in mat4 txx, in vec3 rad ) 
{
    // convert from ray to box space
	vec3 rdd = (txx*vec4(rd,0.0)).xyz;
	vec3 roo = (txx*vec4(ro,1.0)).xyz;

	// ray-box intersection in box space
    vec3 m = 1.0/rdd;
    vec3 n = m*roo;
    vec3 k = abs(m)*rad;
	
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	if( tN > tF || tF < 0.0) return -1.0;
	
	return tN;
}


//-----------------------------------------------------------------------------------------

mat4 rotationAxisAngle( vec3 v, float angle )
{
    float s = sin( angle );
    float c = cos( angle );
    float ic = 1.0 - c;

    return mat4( v.x*v.x*ic + c,     v.y*v.x*ic - s*v.z, v.z*v.x*ic + s*v.y, 0.0,
                 v.x*v.y*ic + s*v.z, v.y*v.y*ic + c,     v.z*v.y*ic - s*v.x, 0.0,
                 v.x*v.z*ic - s*v.y, v.y*v.z*ic + s*v.x, v.z*v.z*ic + c,     0.0,
			     0.0,                0.0,                0.0,                1.0 );
}

mat4 translate( float x, float y, float z )
{
    return mat4( 1.0, 0.0, 0.0, 0.0,
				 0.0, 1.0, 0.0, 0.0,
				 0.0, 0.0, 1.0, 0.0,
				 x,   y,   z,   1.0 );
}
void main( ){
    vec4 oiMouse=clamp(iMouse,vec4(0.),iResolution.xyxy-1.);
    iMouse=vec4(0);
	vec2 uv = xlv_TEXCOORD0 / iResolution.xy;
    float veProgress = mod(iTime * 0.1, 1.);
    float count = floor(iTime * 0.1);
	// ro: ray origin
	// rd: direction of the ray
	vec3 rd = normalize(vec3((xlv_TEXCOORD0.xy - 0.5 * iResolution.xy) / iResolution.y-vec2(0,0.5), 1.));
	vec3 ro = vec3(0., 8., -16.);
	ro = veAddZoom(rd, ro);
    if(iMouse.z>0.) {
        veR(rd.yz, max(0.,iMouse.y-50.) / iResolution.y * 2.508 * pi);
    } else {
        float a = pi*sin(iTime*.5)*.375*0.0;
        veR(rd.yz, a);
    }
    float tmin=-1.0;
    vec3 nor;
    vec3 pos;
    vec3 colb;

    float nearest = 1e9;
    vec3 nearestOrigin = vec3(-1);
    float nearestProgress = 0.0;

	// ld, td: local, total density 
	// w: weighting factor
	float ld = 0., td = 0., w = 0.;

	// t: length of the ray
	// d: distance function
	float density = 1., t = 0.;
	const float h = 0.1;
	vec4 sum = vec4(0.0);
    //veProgress = fract(veProgress*1.+0.5+0.0*((xlv_TEXCOORD0.x>=.5*iResolution.x)?.3:.7));
    vec3 so = 0.0*((uv.x>=0.5)?vec3(7.0+2.0*veProgress,-11.+17.*veProgress,5):vec3(-7.0-2.0*veProgress,-12.+18.*veProgress,5))+0.5;
    float maxRadius = 16.0;
    vec4 fix = vec4(39,59,759,420);//oiMouse;
    vec4 fix1 = exp2((  mod(fix,(2.*vec4(20,15,20,15)))/(2.*vec4(20,15,20,15)-1.))*2.0-1.);
    vec4 fix2 = exp2((floor(fix/(2.*vec4(20,15,20,15)))/(1.*vec4(20,15,20,15)-1.))*2.0-1.);
    fix1=mix(fix1,vec4(0.5),smoothstep(0.5,0.75,veProgress));
    fix2=mix(fix2,vec4(0.5),smoothstep(0.5,0.75,veProgress));
    //fix2.x=1.0;
    vec3 intersect = veSphereIntersect(ro, rd, so, veExplosionRadius(maxRadius,veProgress));
	float min_dist = intersect.x, max_dist = intersect.y;
    const bool veDev = true;
	if (min_dist != max_dist && max_dist > 0.0) {
		t = min_dist;
		// raymarch loop
		for (int i = 0; i < 86; i++) {
			if (veLOW_QUALITY && i >= 56) {
				break;
			}
			vec3 pos = ro + t * rd;
            //pos= (pos-0.5)*pow(vec3(1.+abs(0.5-uv.x)*2.),vec3(0,-0.75,0))+0.5;
			// Loop break conditions.
			if (td > 0.99 || t > max_dist)
				break;
			// evaluate distance function
			float d = veMap(pos-so, maxRadius, count, veProgress);
			if (!veBOTH && veLEFT || veBOTH && uv.y < 0.5) {
                d = (abs(d) + 0.07/fix1.x);
            } else {
                float v = 1.-(1.-veProgress)*sqrt(1.-veProgress);
                v = sqrt(veProgress);
                v = smoothstep(0.03125,0.5,1.0-v);
                v = pcurve(veProgress,0.25,4.5);
                d = mix(abs(d) + 0.07/fix1.x, d, v);
            }
			// change this string to control density
			d = max(mix(d, 10.0, 0.*smoothstep(0.75,1.0,veProgress)), 0.00000003); //0.03 less detail
			// point light calculations
			vec3 ldst = so - pos;
            
			float lDist = max(length(ldst)*fix2.x, 0.01);//0.1, 0.000000000000001 it's all the same
			// the color of light
			vec3 lightColor = vec3(1.0, 0.5, 0.25);
			// bloom
			vec3 halo = lightColor / exp(pow(lDist, 3.) * 0.108/fix1.z);
			sum.rgb += halo * (0.0333*fix2.z * smoothstep(0.5, 0.1, veProgress) + smoothstep(.125, .0, veProgress));
            if(!veDev && h >= d) {
			    td += 1. / 70. + (sin(7.*d+ veProgress)+1.-veProgress)/128.*smoothstep(.3, .125, abs(veProgress-0.7));

//				ld = (d < h*fix) ? h*fix - d : max(0.,0.2*fix - d);
				// compute weighting factor
//				w = (1. - td) * ld;
				// accumulate density
//				td += w/fix;
//				td += (d < h*fix) ? 1. / 200000. : 1. / 70.*fix + (sin(7.*d + veProgress)+1.-veProgress)/128.*smoothstep(.3, .125, abs(veProgress-0.7));

            } else {
				// compute local density
                float choice = smoothstep(-0.5*h,0.5*h, d - h);
				ld = mix(h - d, max(0.2*fix1.w - d,0.), choice);
				// compute weighting factor
				w = (1. - td) * ld;
				// accumulate density
				td += w/fix2.w;
				td += mix(1./((fix1.y-0.5)/1.5*199930.+70.), fix2.y*1. / 70. + (sin(7.*d + veProgress)+1.-veProgress)/128.*smoothstep(.3, .125, abs(veProgress-0.7)),choice);
				vec4 col = vec4(veComputeColor(td, lDist, maxRadius), mix(td, td * w, choice));
				// emission
				sum += sum.a * vec4(sum.rgb, 0.0) * 0.2 / lDist;
				// uniform scale density
				col.a *= 0.2;
				// colour by alpha
				col.rgb *= col.a;
				// alpha blend in contribution
				sum = sum + col * (1.0 - sum.a);
            }
            float dither;
			if (veDITHERING) {
				// idea from https://www.shadertoy.com/view/lsj3Dw
				dither = texture(iChannel2, vec2(120.0* uv.y, -280.0* uv.x+ 0.5* sin(4.* veProgress+ 480.0* uv.y))).r;
            } else {
				dither = 0.5;
            }
            d *= .8 + 0.08 * dither;
			// trying to optimize step size
            float stepMult = (veLOW_QUALITY) ? 0.25 : max(0.08 * min(length(ldst), d), 0.16);
            t += max(d * stepMult, 0.01);
		}
		// simple scattering
        sum *= 1. / exp(ld * 0.2) * ((veLOW_QUALITY) ? 0.9 : 0.8);
		sum = clamp(sum, 0.0, 1.0);
		sum.xyz = sum.xyz * sum.xyz * (3.0 - 2.0 * sum.xyz);
	}

    float pass = smoothstep(veBackgroundCutoff, veBackgroundCutoffStart, td);
    sum.xyz += colb * pass * (1.0 - sum.a);
    if(tmin>0.0){sum.a=1.0;}
	if (veBackground) {
		// stars background
		//vec3 stars = vec3(veNoise(rd * 200.0) * 0.5 + 0.5);
		vec3 stars = veStars(ro, rd);
		vec3 starbg = mix(veColSkyHigh, veColSkyLow,
				abs(1.5 * dot(normalize(rd), vec3(0., 1., 0.))));
		//vec3 starbg = mix(veColSkyHigh, veColSkyLow, uv.y);
		//vec3 brightness = smoothstep(0.95, 1.0, stars);
		//float limits = dot(vec3(0.0), rd) + 0.75;
		//starbg += veColStars * brightness * limits;
//		if (td < veBackgroundCutoff) {
//		}
		//starbg *= pass;
		sum.xyz += (starbg + stars) * pass * (1.0 - sum.a);
		//sum.xyz += (veSpaceCol(ro, rd) * pass + starbg) * (1.0 - sum.a);
		//sum.xyz += veSpaceCol(ro, rd) * (1 - sum.a);
	}

	if (veTONEMAPPING) {
		sum.xyz = veToneMapFilmicALU(sum.xyz * 2.2);
	}
    gl_FragColor = vec4(sum.xyz, 1.0);
    //gl_FragColor = gl_FragColor;
}
#undef iMouse
void mainImage(out vec4 O, in vec2 U){iiMouse=iMouse;mainImage1(O,U);}
