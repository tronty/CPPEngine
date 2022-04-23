/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
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

float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
float iTime=0.0;
int iFrame=0;
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;

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

#define AA 3  // make this 2 or 3 for antialiasing
// Derived from IQ raymarcher and is, as such, educational only

//snoise(float3 uv, float res)
//------------------------------------------------------------------

#define ZERO (min(iFrame,0))

//------------------------------------------------------------------

float hash(float3 p)  // replace this by something better
{
    p  = frac( p*0.3183099+.1 );
	p *= 17.0;
    return frac( p.x*p.y*p.z*(p.x+p.y+p.z) );
}


float sdBuildings(float3 p) {
    float3 box = float3(.7f, 4.7f, .4f);
    float3 q = p;
    
    float modsize = 3.0f;
    
    p.xz = mod(p.xz, modsize) - modsize/2.0f;

    float d =  length( max ( abs(p) - box, .0f  )) - .003f;
    return d;
}



float2 map( in float3 pos )
{

    float2 res = float2( 1e10, 0.0 );
    res.x = sdBuildings(pos);
    if (pos.y + .5f < res.x) {
        res.x = pos.y + .5f;
        res.y = 2.0f;
    }
    return res;
}

const float maxHei = 0.8;

float2 castRay( in float3 ro, in float3 rd )
{
    float2 res = float2(-1.0,-1.0);

    float tmin = 1.0;
    float tmax = 40.0;

    // raymarch primitives   
    {
    
        float t = tmin;
        for( int i=0; i<400 && t<tmax; i++ )
        {
            float2 h = map( ro+rd*t );
            if( abs(h.x)<(0.001*t) )
            { 
                res = float2(t,h.y); 
                 break;
            }
            t += h.x;
        }
    }
    
    return res;
}


// http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float calcSoftshadow( in float3 ro, in float3 rd, in float mint, in float tmax )
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
float3 calcNormal( in float3 pos )
{
    float2 e = float2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map( pos + e.xyy ).x + 
					  e.yyx*map( pos + e.yyx ).x + 
					  e.yxy*map( pos + e.yxy ).x + 
					  e.xxx*map( pos + e.xxx ).x );
 
}

float calcAO( in float3 pos, in float3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=ZERO; i<5; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        float3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 ) * (0.5+0.5*nor.y);
}


float3x3 setCamera( in float3 ro, in float3 ta, float cr )
{
	float3 cw = normalize(ta-ro);
	float3 cp = float3(sin(cr), cos(cr),0.0);
	float3 cu = normalize( cross(cw,cp) );
	float3 cv =          ( cross(cu,cw) );
    return float3x3( cu, cv, cw );
}

float3 lighting(float3 ro, float3 rd, float t, float mat) {

    float3 col = float3(0,0,0);
    float li = 1.3f;
    float3 light_col = li * float3(.5f, .5f, .9f);
    float3 light_dir = float3(0, 0, -1.0);
   
    float3 nor = calcNormal(ro + rd*t);
    float ndotl = max(.2f, dot(nor, light_dir) );
    col =  light_col * ndotl;
    
    float3 l_pos = ro;
    
    float3 l_dir = normalize(- (rd * t));
    
    float ndotl_car = max(.0f, dot(nor, l_dir) );
    float spec = abs( dot (reflect(rd, nor), l_dir ) );
    //return float3(ndotl_car / (t*t));
    //ndotl_car = pow(ndotl_car, 5.0f);
    ndotl_car /= (t*t * t * .1f);
    
    //col += float3(10.0f ) * ndotl_car;
    spec = 40.0f * pow(spec, 5.0f)  / (t * t);
    return float3(spec,spec,spec);
    col += float3(ndotl_car,ndotl_car,ndotl_car);
    return col;
}

float3 background(float3 ro, float3 rd, float2 uv) {
    float f = smoothstep(0.0f, .2f, rd.y);
    uv.y -= iTime / 20.0f;
    float3 sky = float3(0.03,0.03,0.07) + pow( tex2D(iChannel0, uv/1.0f).x, 10.0f) * .5f;
    return lerp(float3(0.05,0.06,0.07), sky, f);
}

float roadline(float3 p ) {
    return smoothstep(0.01f, .0f, abs((p).x)) + 
    smoothstep(0.01f, .0f, abs((p).x + .5f))
    + smoothstep(0.01f, .0f, abs((p).x - .5f));
           
}

float3 applyFog( in float3  rgb,       // original color of the pixel
               in float distance ) // camera to point distance
{

    float b = .1f;
   // distance *= 2.0f;
    float fogAmount = 1.0 - exp( -distance*b );
    float3  fogColor  = float3(0.05,0.06,0.07);
    return lerp( rgb, fogColor, fogAmount );
}
float fog(float3 ro, float3 rd, float t)
{
    return .1f / t;
}

float hash1( float2 p )
{
    p  = 50.0*frac( p*0.3183099 );
    return frac( p.x*p.y*(p.x+p.y) );
}

float4 shadeBuilding(float3 p, float3 nor) {
    
    float2 qf = round ( frac( ( p.xy - float2(.0f, .0f) )  / .2f) );
    float2 qid =  round( ( ( ( p.xy - float2(.0f, .0f) )  / .2f) ));
    float l = clamp( .0f, 1.0f, round ( hash1( qid.xy ) * 2.0f));
    float2 q = mod(p.xy, .8) - .4f;
    if (abs(nor.x) > .3f)
        qf = round ( frac( ( p.yz - float2(.0f, .0f) )  / .2f) );
    float f = smoothstep(.03f, .2f, p.y) * qf.x * qf.y;
    f = f * .1f + f * .9f * l;

    float2 tuv = lerp(p.xy, p.zy, nor.x);
    float3 bcol = tex2D(iChannel1, tuv).xyz / 10.0f;
    float3 col =  lerp(bcol, float3(1.0f, 1.0f, 3.0f / 5.0f), f);
    return (float4(col, f));     
}

float3 render( in float3 ro, in float3 rd, float2 uv )
{ 
    float3 col = float3(0,0,0);
    float2 res = castRay(ro,rd);
    float t = res.x;
	float m = res.y;
    float3 nor;
    if (res.x > .0f) {
    float3 pos = ro+rd*t;
    nor = calcNormal(ro + rd * t);
    float emi = .0f;
        if (res.y == 2.0f) {
            col = float3(.1f, .1f, .1f);
            float a =  roadline(ro + rd * t) * .1f;
            
            col = float3(a,a,a)     +  tex2D(iChannel2,  (ro+rd*t).xz * 2.0f ).xyz * 0.01f * smoothstep(.5f, 0.51f, abs(pos.x));
;
            }
        else{
        float4 r = shadeBuilding(ro + rd * t, nor);
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
   	return float3(col);
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 mo = float2(0,0);//iMouse.xy/iResolution.xy;
	float time = .0f; //iTime;

    // camera	
    float3 ro = float3(0, -0.3, 0.0f);//float3( 4.6*cos(0.1*time + 6.0*mo.x), 1.0 + 2.0*mo.y, 0.5 + 4.6*sin(0.1*time + 6.0*mo.x) );
    float3 ta = float3( 0, -0.4, 1.0 );
    // camera-to-world transformation
    float3x3 ca = setCamera( ro, ta, 0.0 );

    float3 tot = float3(0,0,0);
#if AA>1
    for( int m=ZERO; m<AA; m++ )
    for( int n=ZERO; n<AA; n++ )
    {
        // pixel coordinates
        float2 o = float2(float(m),float(n)) / float(AA) - 0.5;
        float2 p = (-iResolution.xy + 2.0*(fragCoord+o))/iResolution.y;
#else    
        float2 p = (-iResolution.xy + 2.0*fragCoord)/iResolution.y;
#endif

        // ray direction
        ro.z = iTime * 1.3f;
        ro.x = -.2f;
        float3 rd = normalize( mul(ca, float3(p.xy,2.0)  ));

        // render	
        float3 col = render( ro, rd, p );

		// gamma
        col = pow( col, float3(1.5545,1.5545,1.5545) );
        tot += col;
#if AA>1
    }
    tot /= float(AA*AA);
#endif

    
    return float4( tot, 1.0 );
}
