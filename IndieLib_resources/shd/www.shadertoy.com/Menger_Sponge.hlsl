/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
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
// https://www.shadertoy.com/view/4sX3Rn
// Menger Sponge
float iTime=0;
float2 iResolution=float2(1,1);
float2 iMouse=float2(1,1);

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

// https://www.shadertoy.com/view/4sX3Rn'
// The MIT License
// https://www.youtube.com/c/InigoQuilez
// https://iquilezles.org/
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// http://www.iquilezles.org/www/articles/menger/menger.htm


#if HW_PERFORMANCE==0
#define AA 1
#else
#define AA 2
#endif


float maxcomp(in float3 p ) { return max(p.x,max(p.y,p.z));}
float sdBox( float3 p, float3 b )
{
    float3  di = abs(p) - b;
    float mc = maxcomp(di);
    return min(mc,length(max(di,0.0)));
}

float2 iBox( in float3 ro, in float3 rd, in float3 rad ) 
{
    float3 m = 1.0/rd;
    float3 n = m*ro;
    float3 k = abs(m)*rad;
    float3 t1 = -n - k;
    float3 t2 = -n + k;
	return float2( max( max( t1.x, t1.y ), t1.z ),
	             min( min( t2.x, t2.y ), t2.z ) );
}

float3x3 ma = float3x3( 0.60, 0.00,  0.80,
                      0.00, 1.00,  0.00,
                     -0.80, 0.00,  0.60 );
float4 map( in float3 p_ )
{
	float3 p=p_;
    float d = sdBox(p,float3(1,1,1));
    float4 res = float4( d, 1.0, 0.0, 0.0 );

    float ani = smoothstep( -0.2, 0.2, -cos(0.5*iTime) );
	float off = 1.5*sin( 0.01*iTime );
	
    float s = 1.0;
    for( int m=0; m<4; m++ )
    {
    	float3 v3=p+off;

        #p = mix( p, ma*(p+off), ani );
    	
        p = lerp( p, mul(ma, v3), ani );
	   
        float3 a = mod( p*s, 2.0 )-1.0;
        s *= 3.0;
        float3 r = abs(1.0 - 3.0*abs(a));
        float da = max(r.x,r.y);
        float db = max(r.y,r.z);
        float dc = max(r.z,r.x);
        float c = (min(da,min(db,dc))-1.0)/s;

        if( c>d )
        {
          d = c;
          res = float4( d, min(res.y,0.2*da*db*dc), (1.0+float(m))/4.0, 0.0 );
        }
    }

    return res;
}

float4 intersect( in float3 ro, in float3 rd )
{
    float2 bb = iBox( ro, rd, float3(1.05,1.05,1.05) );
    if( bb.y<bb.x ) return float4(-1.0,-1.0,-1.0,-1.0);
    
    float tmin = bb.x;
    float tmax = bb.y;
    
    float t = tmin;
    float4 res = float4(-1.0,-1.0,-1.0,-1.0);
    for( int i=0; i<64; i++ )
    {
        float4 h = map(ro + rd*t);
		if( h.x<0.002 || t>tmax ) break;
        res = float4(t,h.yzw);
        t += h.x;
    }
	if( t>tmax ) res=float4(-1.0,-1.0,-1.0,-1.0);
    return res;
}

float softshadow( in float3 ro, in float3 rd, float mint, float k )
{
    float2 bb = iBox( ro, rd, float3(1.05,1.05,1.05) );
    float tmax = bb.y;
    
    float res = 1.0;
    float t = mint;
    for( int i=0; i<64; i++ )
    {
        float h = map(ro + rd*t).x;
        res = min( res, k*h/t );
        if( res<0.001 ) break;
		t += clamp( h, 0.005, 0.1 );
        if( t>tmax ) break;
    }
    return clamp(res,0.0,1.0);
}

float3 calcNormal(in float3 pos)
{
    float3 eps = float3(.001,0.0,0.0);
    return normalize(float3(
    map(pos+eps.xyy).x - map(pos-eps.xyy).x,
    map(pos+eps.yxy).x - map(pos-eps.yxy).x,
    map(pos+eps.yyx).x - map(pos-eps.yyx).x ));
}

float3 render( in float3 ro, in float3 rd )
{
    // background color
    float3 col = lerp( float3(0.3,0.2,0.1)*0.5, float3(0.7, 0.9, 1.0), 0.5 + 0.5*rd.y );
	
    float4 tmat = intersect(ro,rd);
    if( tmat.x>0.0 )
    {
        float3  pos = ro + tmat.x*rd;
        float3  nor = calcNormal(pos);
        
        float3 matcol = 0.5 + 0.5*cos(float3(0.0,1.0,2.0)+2.0*tmat.z);
        
        float occ = tmat.y;

        float3 light = normalize(float3(1.0,0.9,0.3));
        float dif = dot(nor,light);
        float sha = 1.0;
        if( dif>0.0 ) sha=softshadow( pos, light, 0.01, 64.0 );
        dif = max(dif,0.0);
        float3  hal = normalize(light-rd);
        float spe = dif*sha*pow(clamp(dot(hal,nor),0.0,1.0),16.0)*(0.04+0.96*pow(clamp(1.0-dot(hal,light),0.0,1.0),5.0));
        
		float sky = 0.5 + 0.5*nor.y;
        float bac = max(0.4 + 0.6*dot(nor,float3(-light.x,light.y,-light.z)),0.0);

        float3 lin  = float3(0,0,0);
        lin += 1.00*dif*float3(1.10,0.85,0.60)*sha;
        lin += 0.50*sky*float3(0.10,0.20,0.40)*occ;
        lin += 0.10*bac*float3(1.00,1.00,1.00)*(0.5+0.5*occ);
        lin += 0.25*occ*float3(0.15,0.17,0.20);	 
        col = matcol*lin + spe*128.0;
    }

    col = 1.5*col/(1.0+col);
    col = sqrt( col );
    
    return col;
}

float4 main( in float2 fragCoord:TEXCOORD0 ):COLOR
{
    // camera
    float3 ro = 1.1*float3(2.5*sin(0.25*iTime),1.0+1.0*cos(iTime*.13),2.5*cos(0.25*iTime));

#if AA>1
    #define ZERO (min(iFrame,0))
    float3 col = float3(0,0,0);
    for( int m=ZERO; m<AA; m++ )
    for( int n=ZERO; n<AA; n++ )
    {
        // pixel coordinates
        float2 o = float2(float(m),float(n)) / float(AA) - 0.5;
        float2 p = (2.0*(fragCoord+o)-iResolution.xy)/iResolution.y;

        float3 ww = normalize(float3(0,0,0) - ro);
        float3 uu = normalize(cross( float3(0.0,1.0,0.0), ww ));
        float3 vv = normalize(cross(ww,uu));
        float3 rd = normalize( p.x*uu + p.y*vv + 2.5*ww );

        col += render( ro, rd );
    }
    col /= float(AA*AA);
#else   
    float2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    float3 ww = normalize(float3(0,0,0) - ro);
    float3 uu = normalize(cross( float3(0.0,1.0,0.0), ww ));
    float3 vv = normalize(cross(ww,uu));
    float3 rd = normalize( p.x*uu + p.y*vv + 2.5*ww );
    float3 col = render( ro, rd );
#endif        
    
    return float4(col,1.0);
}


float4 main_( in float2 fragCoord:TEXCOORD0, in float3 fragRayOri:TEXCOORD1, in float3 fragRayDir:TEXCOORD2 ) : COLOR
{
    float time = iTime*0.25 + 0.01*iMouse.x;
    float anim = 1.1 + 0.5*smoothstep( -0.3, 0.3, cos(0.1*iTime) );

    float3 col = render( fragRayOri + float3(0.0,1.0,2.5), fragRayDir );
    return float4( col, 1.0 );
}
