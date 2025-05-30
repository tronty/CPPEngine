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
// https://www.shadertoy.com/view/Xts3WM
// Cheap curvature
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
float2      iResolution=float2(1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float2 iMouse=float2(1,1);
sampler2D iChannel0;
//Cheap curvature by nimitz (twitter: @stormoid)

/*
	Not sure if this method is new, but I haven't seen it around.
	Using a single extra tap to return analytic curvature of an SDF.

	My first implementation required 16 taps, then 12, then thanks 
	to some math help from a friend (austere) I got it down to 7 taps.
	And this is the	final optimization which requires 5 taps or a
	single tap if you're already computing normals.


	Edit (April 2016):

	Coming back to this I know realize that what I am returning is an
	approximation of the Laplacian of the SDF and the Laplacian being 
	the divergence of the gradient of the field means that any point
	on the surface being a sink will return negative "curvature" and
	vice-versa (since the gradient of a SDF should point to the centroid
	at any point in space).

	So now, I include the 7-tap version which is a more accurate Laplacian,
	while the 5-tap version while cheaper, it is less accurate.
*/

#define ITR 80
#define FAR 10.
#define time iTime

float2x2 mm2(in float a){float c = cos(a), s = sin(a);return float2x2(c,s,-s,c);}

float map(float3 p)
{
    p.x += sin(p.y*4.+time+sin(p.z))*0.15;
    float d = length(p)-1.;
    float st = sin(time*0.42)*.5+0.5; 
    const float frq = 10.;
    d += sin(p.x*frq + time*.3 + sin(p.z*frq+time*.5+sin(p.y*frq+time*.7)))*0.075*st;
    
    return d;
}

float march(in float3 ro, in float3 rd)
{
	float precis = 0.001;
    float h=precis*2.0;
    float d = 0.;
    for( int i=0; i<ITR; i++ )
    {
        if( abs(h)<precis || d>FAR ) break;
        d += h;
	    float res = map(ro+rd*d);
        h = res;
    }
	return d;
}

//5 taps total, returns both normal and curvature
float3 norcurv(in float3 p, out float curv)
{
    float2 e = float2(-1., 1.)*0.01;   
    float t1 = map(p + e.yxx), t2 = map(p + e.xxy);
    float t3 = map(p + e.xyx), t4 = map(p + e.yyy);

    curv = .25/e.y*(t1 + t2 + t3 + t4 - 4.0*map(p));
    return normalize(e.yxx*t1 + e.xxy*t2 + e.xyx*t3 + e.yyy*t4);
}

//Curvature only, 5 taps, with epsilon width as input
float curv(in float3 p, in float w)
{
    float2 e = float2(-1., 1.)*w;   
    
    float t1 = map(p + e.yxx), t2 = map(p + e.xxy);
    float t3 = map(p + e.xyx), t4 = map(p + e.yyy);
    
    //return .25/e.y*(t1 + t2 + t3 + t4 - 4.0*map(p));
    return .25/e.y*(t1 + t2 + t3 + t4 - 4.0*map(p));
}

//Curvature in 7-tap (more accurate)
float curv2(in float3 p, in float w)
{
    float3 e = float3(w, 0, 0);
    
    float t1 = map(p + e.xyy), t2 = map(p - e.xyy);
    float t3 = map(p + e.yxy), t4 = map(p - e.yxy);
    float t5 = map(p + e.yyx), t6 = map(p - e.yyx);
    
    return .25/e.x*(t1 + t2 + t3 + t4 + t5 + t6 - 6.0*map(p));
}


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
	float2 p = fragCoord.xy/iResolution.xy-0.5;
	p.x*=iResolution.x/iResolution.y;
	float2 mo = iMouse.xy / iResolution.xy-.5;
	mo.x *= iResolution.x/iResolution.y;
	
    float3 ro = float3(0.,0.,4.);
    // CheapCurvature.shd(161,11): error X3020: type mismatch
    // ro.xz *= mm2(time*0.05+mo.x*3.);
	float3 ta = float3(0, 0, 0);
    float3 ww = normalize(ta - ro);
    float3 uu = normalize(cross(float3(0.,1.,0.), ww));
    float3 vv = normalize(cross(ww, uu));
    float3 rd = normalize(p.x*uu + p.y*vv + 1.5*ww);
	
	float rz = march(ro,rd);
	
    float3 col = tex2D(iChannel0, rd.xy).rgb;
    
    if ( rz < FAR )
    {
        float3 pos = ro+rz*rd;
        float crv;
        float3 nor = norcurv(pos, crv);
        crv = curv2(pos, 0.01);
        float3 ligt = normalize( float3(.0, 1., 0.) );
        float dif = clamp(dot( nor, ligt ), 0., 1.);
        float bac = clamp( dot( nor, -ligt), 0.0, 1.0 );
        float spe = pow(clamp( dot( reflect(rd,nor), ligt ), 0.0, 1.0 ),400.);
        float fre = pow( clamp(1.0+dot(nor,rd),0.0,1.0), 2.0 );
        float3 brdf = float3(0.10,0.11,0.13);
        brdf += bac*float3(0.1, 0.1, 0.1);
        brdf += dif*float3(1.00,0.90,0.60);
        col = abs(sin(float3(0.2,0.5,.9)+clamp(crv*80.,0.,1.)*1.2));
        // ??? col = mix(col,tex2D(iChannel0,reflect(rd,nor)).rgb,.5);
        col = mix(col,tex2D(iChannel0,reflect(rd,nor).xy).rgb,.5);
        col = col*brdf + col*spe +.3*fre*mix(col,float3(1,1,1),0.5);
        col *= smoothstep(-1.,-.9,sin(crv*200.))*0.15+0.85;
    }
	
	fragColor = float4( col, 1.0 );
	return fragColor;
}
