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
// https://www.shadertoy.com/view/4ts3z2
// Xyptonjtroz
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
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
// Xyptonjtroz by nimitz (twitter: @stormoid)
// https://www.shadertoy.com/view/4ts3z2

//Audio by Dave_Hoskins

#define ITR 100
#define FAR 30.
#define time iTime

/*
	Believable animated volumetric dust storm in 7 samples,
	blending each layer in based on geometry distance allows to
	render it without visible seams. 3d Triangle noise_ is 
	used for the dust volume.

	Also included is procedural bump mapping and glow based on
	curvature*fresnel. (see: https://www.shadertoy.com/view/Xts3WM)


	Further explanation of the dust generation (per Dave's request):
		
	The basic idea is to have layers of gradient shaded volumetric
	animated noise_. The problem is when geometry is intersected
	before the ray reaches the far plane. A way to smoothly blend
	the low sampled noise_ is needed.  So I am blending (smoothstep)
	each dust layer based on current ray distance and the solid 
	interesction distance. I am also scaling the noise_ taps	as a 
	function of the current distance so that the distant dust doesn't
	appear too noisy and as a function of current height to get some
	"ground hugging" effect.
	
*/

float2x2 mm2(in float a){float c = cos(a), s = sin(a);return float2x2(c,s,-s,c);}

float height(in float2 p)
{
    p *= 0.2;
    return sin(p.y)*0.4 + sin(p.x)*0.4;
}

//smooth min form iq
float smin( float a, float b)
{
    float k = 0.7;
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return lerp( b, a, h ) - k*h*(1.0-h);
}

//form Dave
float2 hash22(float2 p)
{
	p  = frac(p * float2(5.3983, 5.4427));
    p += dot(p.yx, p.xy +  float2(21.5351, 14.3137));
	return frac(float2(p.x * p.y * 95.4337, p.x * p.y * 97.597));
}

float vine(float3 p, in float c, in float h)
{
    p.y += sin(p.z*0.2625)*2.5;
    p.x += cos(p.z*0.1575)*3.;
    float2 q = float2(mod(p.x, c)-c/2., p.y);
    return length(q) - h -sin(p.z*2.+sin(p.x*7.)*0.5+time*0.5)*0.13;
}

float map(float3 p)
{
    p.y += height(p.zx);
    
    float3 bp = p;
    float2 hs = hash22(floor(p.zx/4.));
    p.zx = mod(p.zx,4.)-2.;
    
    float d = p.y+0.5;
    p.y -= hs.x*0.4-0.15;
    p.zx += hs*1.3;
    d = smin(d, length(p)-hs.x*0.4);
    
    d = smin(d, vine(bp+float3(1.8,0.,0),15.,.8) );
    d = smin(d, vine(bp.zyx+float3(0.,0,17.),20.,0.75) );
    
    return d*1.1;
}

float march(in float3 ro, in float3 rd)
{
	float precis = 0.002;
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

float tri(in float x){return abs(frac(x)-.5);}
float3 tri3(in float3 p){return float3( tri(p.z+tri(p.y*1.)), tri(p.z+tri(p.x*1.)), tri(p.y+tri(p.x*1.)));}
                                 
float2x2 m2 = float2x2(0.970,  0.242, -0.242,  0.970);

float triNoise3d(in float3 p, in float spd)
{
    float z=1.4;
	float rz = 0.;
    float3 bp = p;
	for (float i=0.; i<=3.; i++ )
	{
        float3 dg = tri3(bp*2.);
        p += (dg+time*spd);

        bp *= 1.8;
		z *= 1.5;
		p *= 1.2;
        //p.xz*= m2;
        
        rz+= (tri(p.z+tri(p.x+tri(p.y))))/z;
        bp += 0.14;
	}
	return rz;
}

float fogmap(in float3 p, in float d)
{
    p.x += time*1.5;
    p.z += sin(p.x*.5);
    return triNoise3d(p*2.2/(d+20.),0.2)*(1.-smoothstep(0.,.7,p.y));
}

float3 fog(in float3 col, in float3 ro, in float3 rd, in float mt)
{
    float d = .5;
    for(int i=0; i<7; i++)
    {
        float3  pos = ro + rd*d;
        float rz = fogmap(pos, d);
		float grd =  clamp((rz - fogmap(pos+.8-float(i)*0.1,d))*3., 0.1, 1. );
        float3 col2 = (float3(.1,0.8,.5)*.5 + .5*float3(.5, .8, 1.)*(1.7-grd))*0.55;
        col = lerp(col,col2,clamp(rz*smoothstep(d-0.4,d+2.+d*.75,mt),0.,1.) );
        d *= 1.5+0.3;
        if (d>mt)break;
    }
    return col;
}

float3 normal(in float3 p)
{  
    float2 e = float2(-1., 1.)*0.005;   
	return normalize(e.yxx*map(p + e.yxx) + e.xxy*map(p + e.xxy) + 
					 e.xyx*map(p + e.xyx) + e.yyy*map(p + e.yyy) );   
}

float bnoise(in float3 p)
{
    float n = sin(triNoise3d(p*.3,0.0)*11.)*0.6+0.4;
    n += sin(triNoise3d(p*1.,0.05)*40.)*0.1+0.9;
    return (n*n)*0.003;
}

float3 bump(in float3 p, in float3 n, in float ds)
{
    float2 e = float2(.005,0);
    float n0 = bnoise(p);
    float3 d = float3(bnoise(p+e.xyy)-n0, bnoise(p+e.yxy)-n0, bnoise(p+e.yyx)-n0)/e.x;
    n = normalize(n-d*2.5/sqrt(ds));
    return n;
}

float shadow(in float3 ro, in float3 rd, in float mint, in float tmax)
{
	float res = 1.0;
    float t = mint;
    for( int i=0; i<10; i++ )
    {
		float h = map(ro + rd*t);
        res = min( res, 4.*h/t );
        t += clamp( h, 0.05, .5 );
        if(h<0.001 || t>tmax) break;
    }
    return clamp( res, 0.0, 1.0 );

}

float curv(in float3 p, in float w)
{
    float2 e = float2(-1., 1.)*w;   
    
    float t1 = map(p + e.yxx), t2 = map(p + e.xxy);
    float t3 = map(p + e.xyx), t4 = map(p + e.yyy);
    
    return .125/(e.x*e.x) *(t1 + t2 + t3 + t4 - 4. * map(p));
}



float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;	
	float2 p = fragCoord.xy/iResolution.xy-0.5;
    float2 q = fragCoord.xy/iResolution.xy;
	p.x*=iResolution.x/iResolution.y;
    float2 mo = iMouse.xy / iResolution.xy-.5;
    mo = (mo==float2(-.5, -.5))?mo=float2(-0.1,0.07):mo;
	mo.x *= iResolution.x/iResolution.y;
	
	float3 ro = float3(smoothstep(0.,1.,tri(time*.45)*2.)*0.1, smoothstep(0.,1.,tri(time*0.9)*2.)*0.07, -time*0.6);
    ro.y -= height(ro.zx)+0.05;
    mo.x += smoothstep(0.6,1.,sin(time*.6)*0.5+0.5)-1.5;
    float3 eyedir = normalize(float3(cos(mo.x),mo.y*2.-0.2+sin(time*0.45*1.57)*0.1,sin(mo.x)));
    float3 rightdir = normalize(float3(cos(mo.x+1.5708),0.,sin(mo.x+1.5708)));
    float3 updir = normalize(cross(rightdir,eyedir));
	float3 rd=normalize((p.x*rightdir+p.y*updir)*1.+eyedir);
	
    float3 ligt = normalize( float3(.5, .05, -.2) );
    float3 ligt2 = normalize( float3(.5, -.1, -.2) );
    
	float rz = march(ro,rd);
	
    float3 fogb = lerp(float3(.7,.8,.8	)*0.3, float3(1.,1.,.77)*.95, pow(dot(rd,ligt2)+1.2, 2.5)*.25);
    fogb *= clamp(rd.y*.5+.6, 0., 1.);
    float3 col = fogb;
    
    if ( rz < FAR )
    {
        float3 pos = ro+rz*rd;
        float3 nor= normal( pos );
        float d = distance(pos,ro);
        nor = bump(pos,nor,d);
        float crv = clamp(curv(pos, .4),.0,10.);
        float shd = shadow(pos,ligt,0.1,3.);
        float dif = clamp( dot( nor, ligt ), 0.0, 1.0 )*shd;
        float spe = pow(clamp( dot( reflect(rd,nor), ligt ), 0.0, 1.0 ),50.)*shd;
        float fre = pow( clamp(1.0+dot(nor,rd),0.0,1.0), 1.5 );
        float3 brdf = float3(0.10,0.11,0.13);
        brdf += 1.5*dif*float3(1.00,0.90,0.7);
        col = lerp(float3(0.1,0.2,1),float3(.3,.5,1),pos.y*.5)*0.2+.1;
        col *= (sin(bnoise(pos)*900.)*0.2+0.8);
        col = col*brdf + col*spe*.5 + fre*float3(.7,1.,0.2)*.3*crv;
    }
    
    //ordinary distance fog first
    col = lerp(col, fogb, smoothstep(FAR-7.,FAR,rz));
    
    //then volumetric fog
    col = fog(col, ro, rd, rz);
    
    //post
    col = pow(col,float3(0.8, 0.8, 0.8));
    col *= 1.-smoothstep(0.1,2.,length(p));
    
	fragColor = float4( col.x, col.y, col.z, 1.0 );
	return fragColor;
}

