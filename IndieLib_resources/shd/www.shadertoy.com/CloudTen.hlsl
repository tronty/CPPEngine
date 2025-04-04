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
// https://www.shadertoy.com/view/XtS3DD
// Cloud Ten
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

const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
float3      iChannelResolution[4];        // channel resolution (in pixels)
sampler2D iChannel0;
//Cloud Ten by nimitz (twitter: @stormoid)

#define time iTime
float2x2 mm2(in float a){float c = cos(a), s = sin(a);return float2x2(c,s,-s,c);}
float noise_(float t){return tex2D(iChannel0,float2(t,.0)/iChannelResolution[0].xy).x;}
float moy = 0.;

float noise_(in float3 x) //3d noise_ from iq
{
    float3 p = floor(x);
    float3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
	float2 rg = tex2D( iChannel0, (uv+ 0.5)/256.0 ).yx;
	return mix( rg.x, rg.y, f.z );
}

float fbm(in float3 x)
{
    float rz = 0.;
    float a = .35;
    for (int i = 0; i<2; i++)
    {
        rz += noise_(x)*a;
        a*=.35;
        x*= 4.;
    }
    return rz;
}

float path(in float x){ return sin(x*0.01-3.1415)*28.+6.5; }
float map(float3 p){
    return p.y*0.07 + (fbm(p*0.3)-0.1) + sin(p.x*0.24 + sin(p.z*.01)*7.)*0.22+0.15 + sin(p.z*0.08)*0.05;
}

float march(in float3 ro, in float3 rd)
{
    float precis = .3;
    float h= 1.;
    float d = 0.;
    for( int i=0; i<17; i++ )
    {
        if( abs(h)<precis || d>70. ) break;
        d += h;
        float3 pos = ro+rd*d;
        pos.y += .5;
	    float res = map(pos)*7.;
        h = res;
    }
	return d;
}

float3 lgt = float3(0, 0, 0);
float mapV( float3 p ){ return clamp(-map(p), 0., 1.);}
float4 marchV(in float3 ro, in float3 rd, in float t, in float3 bgc)
{
	float4 rz = float4( 0, 0, 0, 0 );
	
	for( int i=0; i<150; i++ )
	{
		if(rz.a > 0.99 || t > 200.) break;
		
		float3 pos = ro + t*rd;
        float den = mapV(pos);
        
        float4 col = float4(mix( float3(.8,.75,.85), float3(0,0,0), den ),den);
        col.xyz *= mix(bgc*bgc*2.5,  mix(float3(0.1,0.2,0.55),float3(.8,.85,.9),moy*0.4), clamp( -(den*40.+0.)*pos.y*.03-moy*0.5, 0., 1. ) );
        col.rgb += clamp((1.-den*6.) + pos.y*0.13 +.55, 0., 1.)*0.35*mix(bgc,float3(1,1,1),0.7); //Fringes
        col += clamp(den*pos.y*.15, -.02, .0); //Depth occlusion
        col *= smoothstep(0.2+moy*0.05,.0,mapV(pos+1.*lgt))*.85+0.15; //Shadows
        
		col.a *= .95;
		col.rgb *= col.a;
		rz = rz + col*(1.0 - rz.a);

        t += max(.3,(2.-den*30.)*t*0.011);
	}

	return clamp(rz, 0., 1.);
}

float pent(in float2 p){    
    float2 q = abs(p);
    return max(max(q.x*1.176-p.y*0.385, q.x*0.727+p.y), -p.y*1.237)*1.;
}

float3 flare(float2 p, float2 pos) //Inspired by mu6k's lens flare (https://www.shadertoy.com/view/4sX3Rs)
{
	float2 q = p-pos;
    float2 pds = p*(length(p))*0.75;
	float a = atan(q.x,q.y);
	
    float rz = .55*(pow(abs(fract(a*.8+.12)-0.5),3.)*(noise_(a*15.)*0.9+.1)*exp2((-dot(q,q)*4.))); //Spokes
	
    rz += max(1.0/(1.0+32.0*pent(pds+0.8*pos)),.0)*00.2; //Projected ghost (main lens)
    float2 p2 = mix(p,pds,-.5); //Reverse distort
	rz += max(0.01-pow(pent(p2 + 0.4*pos),2.2),.0)*3.0;
	rz += max(0.01-pow(pent(p2 + 0.2*pos),5.5),.0)*3.0;	
	rz += max(0.01-pow(pent(p2 - 0.1*pos),1.6),.0)*4.0;
    rz += max(0.01-pow(pent(-(p2 + 1.*pos)),2.5),.0)*5.0;
    rz += max(0.01-pow(pent(-(p2 - .5*pos)),2.),.0)*4.0;
    rz += max(0.01-pow(pent(-(p2 + .7*pos)),5.),.0)*3.0;
	float f=clamp(rz,0.,1.);
    return float3(f,f,f);
}

float3x3 rot_x(float a){float sa = sin(a); float ca = cos(a); return float3x3(1.,.0,.0,    .0,ca,sa,   .0,-sa,ca);}
float3x3 rot_y(float a){float sa = sin(a); float ca = cos(a); return float3x3(ca,.0,sa,    .0,1.,.0,   -sa,.0,ca);}
float3x3 rot_z(float a){float sa = sin(a); float ca = cos(a); return float3x3(ca,sa,.0,    -sa,ca,.0,  .0,.0,1.);}


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;	
    float2 q = fragCoord.xy / iResolution.xy;
    float2 p = q - 0.5;
	float asp =iResolution.x/iResolution.y;
    p.x *= asp;
	float2 mo = iMouse.xy / iResolution.xy;
	float moy = mo.y;
    float st = sin(time*0.3-1.3)*0.2;
    float3 ro = float3(0.,-2.+sin(time*.3-1.)*2.,time*30.);
    ro.x = path(ro.z);
    float3 ta = ro + float3(0,0,1);
    float3 fw = normalize( ta - ro);
    float3 uu = normalize(cross( float3(0.0,1.0,0.0), fw ));
    float3 vv = normalize(cross(fw,uu));
    const float zoom = 1.;
    float3 rd = normalize( p.x*uu + p.y*vv + -zoom*fw );
    
    float rox = sin(time*0.2)*0.6+2.9;
    rox += smoothstep(0.6,1.2,sin(time*0.25))*3.5;
   	float roy = sin(time*0.5)*0.2;
    float3x3 rotation = rot_x(-roy)*rot_y(-rox+st*1.5)*rot_z(st);
	float3x3 inv_rotation = rot_z(-st)*rot_y(rox-st*1.5)*rot_x(roy);
    rd=mul(rotation, rd);
    rd.y -= dot(p,p)*0.06;
    rd = normalize(rd);
    
    float3 col = float3(0,0,0);
    float3 lgt = normalize(float3(-0.3,mo.y+0.1,1.));  
    float rdl = clamp(dot(rd, lgt),0.,1.);
  
    float3 hor = mix( float3(.9,.6,.7)*0.35, float3(.5,0.05,0.05), rdl );
    hor = mix(hor, float3(.5,.8,1),mo.y);
    col += mix( float3(.2,.2,.6), hor, exp2(-(1.+ 3.*(1.-rdl))*max(abs(rd.y),0.)) )*.6;
    col += .8*float3(1.,.9,.9)*exp2(rdl*650.-650.);
    col += .3*float3(1.,1.,0.1)*exp2(rdl*100.-100.);
    col += .5*float3(1.,.7,0.)*exp2(rdl*50.-50.);
    col += .4*float3(1.,0.,0.05)*exp2(rdl*10.-10.);  
    float3 bgc = col;
    
    float rz = march(ro,rd);
    
    if (rz < 70.)
    {   
        float4 res = marchV(ro, rd, rz-5., bgc);
    	col = col*(1.0-res.w) + res.xyz;
    }
    
    float3 projected_flare = mul(inv_rotation,-lgt);
    col += 1.4*float3(0.7,0.7,0.4)*max(flare(p,-projected_flare.xy/projected_flare.z*zoom)*projected_flare.z,0.);
    
    float g = smoothstep(0.03,.97,mo.x);
    col = mix(mix(col,col.brg*float3(1,0.75,1),clamp(g*2.,0.0,1.0)), col.bgr, clamp((g-0.5)*2.,0.0,1.));
    
	col = clamp(col, 0., 1.);
    col = col*0.5 + 0.5*col*col*(3.0-2.0*col); //saturation
    col = pow(col, float3(0.416667, 0.416667, 0.416667))*1.055 - 0.055; //sRGB
	col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12 ); //Vign

	fragColor = float4( col, 1.0 );
	return fragColor;
}
