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
// https://www.shadertoy.com/view/ssK3zt
// Sandstone canyons
float iTime=0;
float2 iResolution=float2(1,1);
// Sandstone_canyons: https://www.shadertoy.com/view/ssK3zt
////////////////////////////////
// Terrain generation section //
////////////////////////////////

float snoise(float2 p) {
	float2 f = frac(p);
	p = floor(p);
	float v = p.x+p.y*1000.0;
	float4 r = float4(v, v+1.0, v+1000.0, v+1001.0);
	r = frac(100000.0*sin(r*.001));
	f = f*f*(3.0-2.0*f);
	return 2.0*(lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y))-1.0;
}

float noise(in float2 uv)
{
    return sin(uv.x)+cos(uv.y);
}

float terrain(in float2 uv,int octaves)
{
    //float scale_factor = 1.;
    //uv /= scale_factor;
    //this function generates the terrain height
    float value = 0.;
    float amplitude = 1.;
    float freq = .5;
    float n1 = 0.;
    
    //float f2 = .1; //this constant changes the variation in mountain height
    
    //float f1 = f2;
    //float f3 = 1.-f1;
    //float2 uv1 = uv;
    for (int i = 0; i < octaves; i++)
    {
        //uv += float2(amplitude,freq);
        n1 = (noise((uv) * freq-n1*value)-n1);
        // From Dave_Hoskins https://www.shadertoy.com/user/Dave_Hoskins
        
        
        value = (value+n1 * amplitude);
        if(amplitude<2.22045e-015) return value;
        freq *= 2.5-amplitude;
        amplitude *= 1./2.;
        
        
        //float f1 = f2*(f3+f1)*noise(uv);
        //uv = uv.yx;
        uv = uv.yx-n1/freq;
        //uv1 = uv1.yx;
        //uv = uv.yx/(1.+amplitude);
        //uv = uv.yx+noise(uv/freq)*amplitude+noise(uv.yx*amplitude)*freq;
        //value *= f3+f1;
        //freq += f1;
        
        //value *= .9;

}
    return value;
}


float2 map(float3 p, int octaves) {
	float dMin = 28.0;
	float d;
	float mID = -1.0;
	
	// Mountains
	float h = terrain(p.xz, octaves);
	//h += smoothstep(0.0, 1.1, h);
    //h += smoothstep(-0.1, 1.0, p.y)*0.6;
	d = p.y - h;
	if (d<dMin) { 
		dMin = d;
		mID = 0.0;
	}

	return float2(dMin, mID);
}

////////////////////
// Render section //
////////////////////

float2 castRay(float3 ro, float3 rd, int octaves) {
	const float p = 0.001;
	float t = 0.0;
	float h = p * 2.0;
	float m = -1.0;
	for (int i=0; i<36; i++) {
		if (abs(h)<=p || t>=28.0) break;
        t += h;
        float2 res = map(ro + rd*t, octaves);
        h = res.x;
        m = res.y;
	}
	if (t>28.0) m = -1.0;
	return float2(t, m);
}

float3 calcNormal(float3 p, int octaves) {
	const float3 eps = float3(0.002, 0.0, 0.0);
	return normalize( float3(map(p+eps.xyy, octaves).x - map(p-eps.xyy, octaves).x,
			       map(p+eps.yxy, octaves).x - map(p-eps.yxy, octaves).x,
			       map(p+eps.yyx, octaves).x - map(p-eps.yyx, octaves).x) );
}

float shadows(float3 ro, float3 rd, float tMax, float k, int octaves) {
    float res = 1.0;
	float t = 0.1;
	for(int i=0; i<22; i++) {
        if (t>=tMax) break;
        float h = map(ro + rd*t, octaves).x;
        res = min( res, k*h/t );
        t += h;
    }
    return clamp(res, 0.2, 1.0);
}

float3 render(float3 ro, float3 rd) {
	const int geoLOD = 4;
	
	float2 res = castRay(ro, rd, geoLOD);
	
	float3 lPos = normalize(float3(1.0, 0.5, 0.0));
	float3 lCol = float3(1.0, 0.9, 0.8);
	
	float3 pos = ro + rd*res.x;
	
	// mat -1 = Background / sky
    float3 color = float3(0.45,0.5,0.6);
    float sun = clamp(dot(rd,lPos),0.0,1.0);
    color += 0.6 * lCol * sun*sun;
    if (res.y < -0.5) {
		return color;
	}
    
    float3 skyColor = color;
	
	int norLOD = int(max(2.0, 12.0-11.0*res.x/28.0));
	float3 nor = calcNormal(pos, norLOD);
	
	// mat 0 = Rock / mountain
	if (res.y>-0.5 && res.y<0.5) {
		// Base rock
		color = lerp( float3(0.4, 0.1, 0.0), float3(0.7, 0.6, 0.3), step(0.9, nor.y) );
		
		// Layer noise
		float n = 0.5*(snoise(pos.xy*float2(2.0, 15.0))+1.0);
		color = lerp( float3(0.6, 0.5, 0.4), color, n*smoothstep(0.0, 0.7, 1.0-nor.y) );
		
        // Sand on top
        color = lerp(color, float3(0.7, 0.6, 0.3), smoothstep(0.0, 0.2, nor.y-0.8));
	}
	// mat 1 = Sand
	if (res.y>0.5) {
		// Base sand and rock color
		color = lerp( float3(0.3, 0.2, 0.0), float3(0.7, 0.6, 0.3), nor.y );
	}
	
    // Lighting and shadows
    float lAmb = clamp( 0.5 + 0.5 * nor.y, 0.0, 1.0);
    float lDif = clamp( dot( nor, lPos ), 0.0, 2.0);

    if (lDif>0.05) lDif *= shadows(pos, lPos, 8.0, 12.0, geoLOD);

    color += (0.4*lAmb) * lCol;
    color *= (1.8*lDif) * lCol;	
    
	// Fog
	float fog = exp(-0.003 *res.x*res.x);
	color = lerp(skyColor, color, fog);
		
	return color;
}

float4 main( in float2 fragCoord:TEXCOORD ) : COLOR
{
	float2 pos = 2.0 * ( fragCoord.xy / iResolution.xy ) - 1.0;
	pos.x *= iResolution.x / iResolution.y;
    float t1 = iTime;
	// Camera
	float x = 0.0 + (0.5*t1);
	float y = 0.0;
	float z = 0.0 + sin(0.1*t1);
	float3 cPos = float3(x, y, z);
	cPos.y = terrain(cPos.xz, 1) + 2.5;
	
	const float3 cUp = float3(0., 1., 0.);
	float3 cLook = float3(cPos.x + 1.0, cPos.y*0.85, 0.0);
	
	// Camera matrix
	float3 ww = normalize( cLook-cPos );
	float3 uu = normalize( cross(ww, cUp) );
	float3 vv = normalize( cross(uu, ww) );
	
	float3 rd = normalize( pos.x*uu + pos.y*vv + 2.0*ww );
	
	// Render
	float3 color = render(cPos, rd);
	
	return float4( color, 1.0 );
}
