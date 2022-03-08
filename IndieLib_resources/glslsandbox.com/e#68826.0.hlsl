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

float2 mouse=float2(1, 1);
float2 resolution=float2(1, 1);
float time=0.0;
float pi = 3.141592653589793;

float hash( in float2 p ) 
{
    return frac(sin(p.x*15.32+p.y*35.78) * 43758.23);
}

float3 noised( in float2 p ) 
{
		
    float2 g = floor(p);
    float2 f = frac(p);
    float2 k = f*f*(3.0-2.0*f);
	float a = hash(g+float2(0.0,0.0));
	float b = hash(g+float2(1.0,0.0));
	float c = hash(g+float2(0.0,1.0));
	float d = hash(g+float2(1.0,1.0));
	return float3(a+(b-a)*k.x+(c-a)*k.y+(a-b-c+d)*k.x*k.y,
				6.0*f*(1.0-f)*(float2(b-a,c-a)+(a-b-c+d)*k.yx));
}
float3 light1 = normalize( float3(-0.8,0.1,-0.3) );
float2x2 m2 = float2x2(0.8,-0.6,0.6,0.8);

float terrain( in float2 x )
{
	float2  p = x*0.003;
    float a = 0.0;
    float b = 1.0;
	float2  d = float2(0,0);
    for( int i=0; i<5; i++ )
    {
        float3 n = noised(p);
        d += n.yz;
        a += b*n.x/(1.0+dot(d,d));
		b *= 0.5;
        p = mul(m2,p)*2.0;
    }

	return 140.0*a;
}

float terrain2( in float2 x ) 
{
	float2  p = x*0.003;
    float a = 0.0;
    float b = 1.0;
	float2  d = float2(0,0);
    for( int i=0; i<11; i++ ) {
        float3 n = noised(p);
        d += n.yz;
        a += b*n.x/(1.0+dot(d,d));
		b *= 0.5;
        p = mul(m2,p)*2.0;
    }

	return 140.0*a;
}

float map( in float3 p )
{
    return p.y - terrain(p.xz);
}

float interesct( in float3 ro, in float3 rd, in float tmin, in float tmax )
{
    float t = tmin;
	for( int i=0; i<120; i++ )
	{
		float h = map( ro + t*rd );
		if( h<(0.002*t) || t>tmax ) break;
		t += 0.5*h;
	}

	return t;
}

float softShadow(in float3 ro, in float3 rd )
{
    // real shadows	
    float res = 1.0;
    float t = 0.001;
	for( int i=0; i<48; i++ )
	{
	    float3  p = ro + t*rd;
        float h = map( p );
		res = min( res, 16.0*h/t );
		t += h;
		if( res<0.001 ||p.y>200.0 ) break;
	}
	return clamp( res, 0.0, 1.0 );
}

float3 calcNormal( in float3 pos, float t )
{
    float2  eps = float2( 0.002*t, 0.0 );
    return normalize( float3( terrain2(pos.xz-eps.xy) - terrain2(pos.xz+eps.xy),
                            2.0*eps.x,
                            terrain2(pos.xz-eps.yx) - terrain2(pos.xz+eps.yx) ) );
}

float3 camPath( float time )
{
	return 1100.0*float3( cos(0.10-0.23*time), 0.0, cos(1.5+0.21*time) );
}

float fbm( float2 p )
{
    float f = 0.0;
    f += 0.5000*noised(p).x; p = mul(m2,p)*2.02;
    f += 0.2500*noised(p).x; p = mul(m2,p)*2.03;
    f += 0.1250*noised(p).x; p = mul(m2,p)*2.01;
    f += 0.0625*noised(p).x;
    return f/0.9375;
}


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 xy = -1.0 + 2.0*fragCoord.xy/resolution.xy;
	float2 s = xy*float2(resolution.x/resolution.y,1.0);

	float3 light1 = normalize( float3(-0.8,0.4,-0.3) );

    // camera position
	float3 ro = float3(1,1,1);
	ro.x += time*9.0;
	float3 ta = float3(1,1,1);
	ro.y = 200.0;
	ta.y = ro.y - 20.0;
	float cr = 0.0;

    // camera ray    
	float3  cw = normalize(ta-ro);
	float3  cp = float3(sin(cr), cos(cr),0.0);
	float3  cu = normalize( cross(cw,cp) );
	float3  cv = normalize( cross(cu,cw) );
	float3  rd = normalize( s.x*cu + s.y*cv + 2.0*cw );

    // bounding plane
    float tmin = 2.0;
    float tmax = 2000.0;
    float maxh = 210.0;
    float tp = (maxh-ro.y)/rd.y;
    if( tp>0.0 )
    {
        if( ro.y>maxh ) tmin = max( tmin, tp );
        else            tmax = min( tmax, tp );
    }

	float sundot = clamp(dot(rd,light1),0.0,1.0);
	float3 col;
    float t = interesct( ro, rd, tmin, tmax );
    if( t>tmax)
    {
        // sky		
		col = float3(0.3,.55,0.8)*(1.0-0.8*rd.y)*0.9;
        // sun
		col += 0.25*float3(1.0,0.7,0.4)*pow( sundot,5.0 );
		col += 0.25*float3(1.0,0.8,0.6)*pow( sundot,64.0 );
		col += 0.2*float3(1.0,0.8,0.6)*pow( sundot,512.0 );
        // clouds
		float2 sc = ro.xz + rd.xz*(1000.0-ro.y)/rd.y;
		col = lerp( col, float3(1.0,0.95,1.0), 0.5*smoothstep(0.5,0.8,fbm(0.0005*sc)) );
        // horizon
        col = lerp( col, float3(0.7,0.75,0.8), pow( 1.0-max(rd.y,0.0), 8.0 ) );
	}
	else
	{
        // mountains		
		float3 pos = ro + t*rd;
        float3 nor = calcNormal( pos, t );
        float3 ref = reflect( rd, nor );
        float fre = clamp( 1.0+dot(rd,nor), 0.0, 1.0 );
        
        // rock
        float r = hash(127.0*pos.xz);
        col = (r*0.25+0.75)*0.9*lerp( float3(0.08,0.05,0.03), float3(0.10,0.09,0.08), hash(0.00007*float2(pos.x,pos.y*48.0)));
		col = lerp( col, 0.20*float3(0.45,.30,0.15)*(0.50+0.50*r),smoothstep(0.70,0.9,nor.y) );
        col = lerp( col, 0.15*float3(0.30,.30,0.10)*(0.25+0.75*r),smoothstep(0.95,1.0,nor.y) );

		// snow
		float h = smoothstep(55.0,80.0,pos.y + 25.0*fbm(0.01*pos.xz) );
        float e = smoothstep(1.0-0.5*h,1.0-0.1*h,nor.y);
        float o = 0.3 + 0.7*smoothstep(0.0,0.1,nor.x+h*h);
        float s = h*e*o;
        col = lerp( col, 0.29*float3(0.62,0.65,0.7), smoothstep( 0.1, 0.9, s ) );
		
         // lighting		
        float amb = clamp(0.5+0.5*nor.y,0.0,1.0);
		float dif = clamp( dot( light1, nor ), 0.0, 1.0 );
		float bac = clamp( 0.2 + 0.8*dot( normalize( float3(-light1.x, 0.0, light1.z ) ), nor ), 0.0, 1.0 );
		float sh = 1.0; if( dif>=0.0001 ) sh = softShadow(pos+light1*20.0,light1);
		
		float3 lin  = float3(0,0,0);
		lin += dif*float3(7.00,5.00,3.00)*float3( sh, sh*sh*0.5+0.5*sh, sh*sh*0.8+0.2*sh );
		lin += amb*float3(0.40,0.60,0.80)*1.2;
        lin += bac*float3(0.40,0.50,0.60);
		col *= lin;
        
        col += s*0.1*pow(fre,4.0)*float3(7.0,5.0,3.0)*sh * pow( clamp(dot(light1,ref), 0.0, 1.0),16.0);
        col += s*0.1*pow(fre,4.0)*float3(0.4,0.5,0.6)*smoothstep(0.0,0.6,ref.y);

		// fog
        float fo = 1.0-exp(-0.0000011*t*t );
        float3 fco = 0.8*float3(0.5,0.7,0.9) + 0.1*float3(1.0,0.8,0.5)*pow( sundot, 4.0 );
		col = lerp( col, fco, fo );

        // sun scatter
		col += 0.3*float3(1.0,0.8,0.4)*pow( sundot, 818.0 )*(1.0-exp(-0.002*t));
	}

    // gamma
	col = pow(col,float3(0.4545,0.4545,0.4545));

    // vignetting	
	col *= 0.5 + 0.5*pow( (xy.x+1.0)*(xy.y+1.0)*(xy.x-1.0)*(xy.y-1.0), 0.1 );
	
	fragColor=float4(col.x,col.y,col.z,1.0);
	return fragColor;
}
