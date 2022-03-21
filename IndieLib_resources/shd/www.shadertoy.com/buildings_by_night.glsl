uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

#define AA 3  // make this 2 or 3 for antialiasing
// Derived from IQ raymarcher and is, as such, educational only

//snoise(vec3 uv, float res)
//------------------------------------------------------------------

#define ZERO (min(iFrame,0))

//------------------------------------------------------------------

float hash(vec3 p)  // replace this by something better
{
    p  = fract( p*0.3183099+.1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}


float sdBuildings(vec3 p) {
    vec3 box = vec3(.7f, 4.7f, .4f);
    vec3 q = p;
    
    float modsize = 3.0f;
    
    p.xz = mod(p.xz, modsize) - modsize/2.0f;

    float d =  length( max ( abs(p) - box, .0f  )) - .003f;
    return d;
}



vec2 map( in vec3 pos )
{

    vec2 res = vec2( 1e10, 0.0 );
    res.x = sdBuildings(pos);
    if (pos.y + .5f < res.x) {
        res.x = pos.y + .5f;
        res.y = 2.0f;
    }
    return res;
}

const float maxHei = 0.8;

vec2 castRay( in vec3 ro, in vec3 rd )
{
    vec2 res = vec2(-1.0,-1.0);

    float tmin = 1.0;
    float tmax = 40.0;

    // raymarch primitives   
    {
    
        float t = tmin;
        for( int i=0; i<400 && t<tmax; i++ )
        {
            vec2 h = map( ro+rd*t );
            if( abs(h.x)<(0.001*t) )
            { 
                res = vec2(t,h.y); 
                 break;
            }
            t += h.x;
        }
    }
    
    return res;
}


// http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float calcSoftshadow( in vec3 ro, in vec3 rd, in float mint, in float tmax )
{
    // bounding volume
    float tp = (maxHei-ro.y)/rd.y; if( tp>0.0 ) tmax = min( tmax, tp );

    float res = 1.0;
    float t = mint;
    for( int i=ZERO; i<16; i++ )
    {
		float h = map( ro + rd*t ).x;
        res = min( res, 8.0*h/t );
        t += clamp( h, 0.02, 0.10 );
        if( res<0.005 || t>tmax ) break;
    }
    return clamp( res, 0.0, 1.0 );
}

// http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map( pos + e.xyy ).x + 
					  e.yyx*map( pos + e.yyx ).x + 
					  e.yxy*map( pos + e.yxy ).x + 
					  e.xxx*map( pos + e.xxx ).x );
 
}

float calcAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=ZERO; i<5; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 ) * (0.5+0.5*nor.y);
}


mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv =          ( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

vec3 lighting(vec3 ro, vec3 rd, float t, float mat) {

    vec3 col = vec3(.0f);
    float li = 1.3f;
    vec3 light_col = li * vec3(.5f, .5f, .9f);
    vec3 light_dir = vec3(0, 0, -1.0);
   
    vec3 nor = calcNormal(ro + rd*t);
    float ndotl = max(.2f, dot(nor, light_dir) );
    col =  light_col * ndotl;
    
    vec3 l_pos = ro;
    
    vec3 l_dir = normalize(- (rd * t));
    
    float ndotl_car = max(.0f, dot(nor, l_dir) );
    float spec = abs( dot (reflect(rd, nor), l_dir ) );
    //return vec3(ndotl_car / (t*t));
    //ndotl_car = pow(ndotl_car, 5.0f);
    ndotl_car /= (t*t * t * .1f);
    
    //col += vec3(10.0f ) * ndotl_car;
    spec = 40.0f * pow(spec, 5.0f)  / (t * t);
    return vec3(spec);
    col += vec3(ndotl_car);
    return col;
}

vec3 background(vec3 ro, vec3 rd, vec2 uv) {
    float f = smoothstep(0.0f, .2f, rd.y);
    uv.y -= iTime / 20.0f;
    vec3 sky = vec3(0.03,0.03,0.07) + pow( texture(iChannel0, uv/1.0f).x, 10.0f) * .5f;
    return mix(vec3(0.05,0.06,0.07), sky, f);
}

float roadline(vec3 p ) {
    return smoothstep(0.01f, .0f, abs((p).x)) + 
    smoothstep(0.01f, .0f, abs((p).x + .5f))
    + smoothstep(0.01f, .0f, abs((p).x - .5f));
           
}

vec3 applyFog( in vec3  rgb,       // original color of the pixel
               in float distance ) // camera to point distance
{

    float b = .1f;
   // distance *= 2.0f;
    float fogAmount = 1.0 - exp( -distance*b );
    vec3  fogColor  = vec3(0.05,0.06,0.07);
    return mix( rgb, fogColor, fogAmount );
}
float fog(vec3 ro, vec3 rd, float t)
{
    return .1f / t;
}

float hash1( vec2 p )
{
    p  = 50.0*fract( p*0.3183099 );
    return fract( p.x*p.y*(p.x+p.y) );
}

vec4 shadeBuilding(vec3 p, vec3 nor) {
    
    vec2 qf = round ( fract( ( p.xy - vec2(.0f, .0f) )  / .2f) );
    vec2 qid =  round( ( ( ( p.xy - vec2(.0f, .0f) )  / .2f) ));
    float l = clamp( .0f, 1.0f, round ( hash1( qid.xy ) * 2.0f));
    vec2 q = mod(p.xy, .8) - .4f;
    if (abs(nor.x) > .3f)
        qf = round ( fract( ( p.yz - vec2(.0f, .0f) )  / .2f) );
    float f = smoothstep(.03f, .2f, p.y) * qf.x * qf.y;
    f = f * .1f + f * .9f * l;

    vec2 tuv = mix(p.xy, p.zy, nor.x);
    vec3 bcol = texture(iChannel1, tuv).xyz / 10.0f;
    vec3 col =  mix(bcol, vec3(1.0f, 1.0f, 3.0f / 5.0f), f);
    return (vec4(col, f));     
}

vec3 render( in vec3 ro, in vec3 rd, vec2 uv )
{ 
    vec3 col = vec3(.0f);
    vec2 res = castRay(ro,rd);
    float t = res.x;
	float m = res.y;
    vec3 nor;
    if (res.x > .0f) {
    vec3 pos = ro+rd*t;
    nor = calcNormal(ro + rd * t);
    float emi = .0f;
        if (res.y == 2.0f) {
            col = vec3(.1f, .1f, .1f);
            float a =  roadline(ro + rd * t) * .1f;
            
            col = vec3(a)     +  texture(iChannel2,  (ro+rd*t).xz * 2.0f ).xyz * 0.01f * smoothstep(.5f, 0.51f, abs(pos.x));
;
            }
        else{
        vec4 r = shadeBuilding(ro + rd * t, nor);
            col = r.xyz;
            emi = r.w;
            }
         //   return lighting(ro, rd, res.x, res.y);
    	col = lighting(ro, rd, res.x, res.y) * col + col * emi;
    }
    else 
        col = background(ro, rd, uv);
    col = applyFog(col, t);
    //col *=.7f;
   	return vec3(col);
}



void main( )
{
    vec2 mo = vec2(0);//iMouse.xy/iResolution.xy;
	float time = .0f; //iTime;

    // camera	
    vec3 ro = vec3(0, -0.3, 0.0f);//vec3( 4.6*cos(0.1*time + 6.0*mo.x), 1.0 + 2.0*mo.y, 0.5 + 4.6*sin(0.1*time + 6.0*mo.x) );
    vec3 ta = vec3( 0, -0.4, 1.0 );
    // camera-to-world transformation
    mat3 ca = setCamera( ro, ta, 0.0 );

    vec3 tot = vec3(0.0);
#if AA>1
    for( int m=ZERO; m<AA; m++ )
    for( int n=ZERO; n<AA; n++ )
    {
        // pixel coordinates
        vec2 o = vec2(float(m),float(n)) / float(AA) - 0.5;
        vec2 p = (-iResolution.xy + 2.0*(xlv_TEXCOORD0+o))/iResolution.y;
#else    
        vec2 p = (-iResolution.xy + 2.0*xlv_TEXCOORD0)/iResolution.y;
#endif

        // ray direction
        ro.z = iTime * 1.3f;
        ro.x = -.2f;
        vec3 rd = normalize( vec3(p.xy,2.0)  ) *ca;

        // render	
        vec3 col = render( ro, rd, p );

		// gamma
        col = pow( col, vec3(1.5545) );
        tot += col;
#if AA>1
    }
    tot /= float(AA*AA);
#endif

    
    gl_FragColor = vec4( tot, 1.0 );
}
