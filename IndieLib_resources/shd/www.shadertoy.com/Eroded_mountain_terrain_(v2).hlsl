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

/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]
ROW_MAJOR 
float4x4 worldViewProj 
MVPSEMANTIC
;
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
#define textureLod tex2D
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
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

// https://www.shadertoy.com/view/stS3zD
#define SC (250.0)

float noise(in float2 uv)
{
    return sin(uv.x)+cos(uv.y);
}


#define OCTAVES 8
float fbm(in float2 uv)
{
    //this function generates the terrain height
    uv /= 1.5;
    float value = 0.;
    float amplitude = 1.;
    float freq = 0.8;
    
    for (int i = 0; i < OCTAVES; i++)
    {
        // From Dave_Hoskins https://www.shadertoy.com/user/Dave_Hoskins
        value = max(value,value+(.25-abs(noise(uv * freq)-.3) * amplitude));
        
        amplitude *= .37;
        
        freq *= 2.05;
        
        uv = uv.yx;
    }
    
    return value*2.0-2.0;
}

float f(in float3 p)
{
    
    float h = fbm(p.xz);
    return h;
}

float3 getNormal(float3 p, float t)
{
    float3 eps=float3(.001 * t, .0, .0);
    float3 n=float3(f(p - eps.xyy) - f(p + eps.xyy),
                2. * eps.x,
                f(p - eps.yyx) - f(p + eps.yyx));
  
    return normalize(n);
}

float rayMarching(in float3 ro, in float3 rd, float tMin, float tMax)
{
    float t = tMin;
	for( int i = 0; i < 300; i++ )
	{
        float3 pos = ro + t * rd;
		float h = pos.y - f(pos);
		if( abs(h) < (0.0015 * t) || t > tMax ) 
            break;
		t += 0.4 * h;
	}

	return t;
}

float3 lighting(float3 p, float3 normal, float3 L, float3 V)
{
    float3 sunColor = float3(1., .956, .839);
    float3 albedo = float3(1,1,1);
   	float3 diff = max(dot(normal, L) * albedo, 0.);
    
    float3 refl = normalize(reflect(L, normal));
    float spec = max(dot(refl, -normalize(V)), 0.);
    spec = pow(spec, 18.);
    spec = clamp(spec, 0., 1.);
    float sky = max(0.0, dot(float3(0.,1.,0.), normal));
    
    //float amb = 0.5 * smoothstep(0.0, 2.0, p.y);
    
    float3 col = diff * sunColor;
    col += spec * sunColor;
    col += sky * float3(0., .6, 1.) * .1;
    //col += amb * .2;
    
   	return col;
}

float3x3 lookAt(float3 origin, float3 target, float roll)
{
    float3 rr = float3(sin(roll), cos(roll), 0.0);
    float3 ww = normalize(target - origin);
    float3 uu = normalize(cross(ww, rr));
    float3 vv = normalize(cross(uu, ww));

    return float3x3(uu, vv, ww);
}

float3 camerapath(float t)
{
    float3 p=float3(-13.0+3.5*cos(t),3.3,-1.1+2.4*cos(2.4*t+2.0));
	return p;
} 

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 uv = (fragCoord - iResolution.xy * .5) / iResolution.y;
	
    float3 lightDir = normalize(float3(-.8, .15, -.3));
    
    float3 camStep = float3(lightDir.x, 0., lightDir.z) * iTime;
    float3 camPos = float3(8., 2., 5.) + camStep;
    float3 camTarget = float3(1., 1., 4.) + camStep;
    float3x3 mat = lookAt(camPos, camTarget, 0.0);
    
    float3 ro = camPos;
    float3 rd = normalize(mul(mat , float3(uv.xy, 1.0)));
    
    float tMin = .1;
    float tMax = 20.;
    float t = rayMarching(ro, rd, tMin, tMax);
    
    float3 col = float3(0,0,0);
    
    if (t > tMax)
    {
        // from iq's shader, https://www.shadertoy.com/view/MdX3Rr
        float sundot = clamp(dot(rd, lightDir), 0.0, 1.0);
        col = float3(0.3,0.5,0.85) - rd.y*rd.y*0.5;
        col = lerp( col, 0.85*float3(0.7,0.75,0.85), pow( 1.0-max(rd.y,0.0), 4.0 ) );
        // sun
		col += 0.25*float3(1.0,0.7,0.4)*pow( sundot,5.0 );
		col += 0.25*float3(1.0,0.8,0.6)*pow( sundot,64.0 );
		col += 0.2*float3(1.0,0.8,0.6)*pow( sundot,512.0 );
        // clouds
		float2 sc = ro.xz + rd.xz*(SC*1000.0-ro.y)/rd.y;
		col = lerp( col, float3(1.0,0.95,1.0), 0.5*smoothstep(0.5,0.8,fbm(0.0005*sc/SC)) );
        // horizon
        col = lerp( col, 0.68*float3(0.4,0.65,1.0), pow( 1.0-max(rd.y,0.0), 16.0 ) );
    }
    else
    {
        float3 p = ro + rd * t;
        float3 normal = getNormal(p, t);
        float3 viewDir = normalize(ro - p);
        
        // lighting terrian
        col = lighting(p, normal, lightDir, viewDir);
        
        // fog
        float fo = 1.0-exp(-pow(30. * t/SC,1.5) );
        float3 fco = 0.65*float3(0.4,0.65,1.0);
        col = lerp( col, fco, fo);
    }
    
    // Gama correction
    col = pow(clamp(col, 0., 1.), float3(.45, .45, .45)); 
    
    return float4(float3(col), 1.0);
}
