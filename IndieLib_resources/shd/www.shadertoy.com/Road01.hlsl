/*
  Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
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
// https://www.shadertoy.com/view/4lBBWw
// Road01
float3 mod(float3 x, float y)
{
  //return x;
  return x - y * floor(x / y); // ???
}

float snoise(float3 uv, float res)
{
	//return 1;
	const float3 s = float3(1e0, 1e2, 1e3);
	
	uv *= res;
	
	float3 uv0 = floor(mod(uv, res))*s;
	float3 uv1 = floor(mod(uv+float3(1,1,1), res))*s;
	
	float3 f = fract(uv); f = f*f*(3.0-2.0*f);

	float4 v = float4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);

	float4 r = fract(sin(v*1e-1)*1e3);
	float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	r = fract(sin((v + uv1.z - uv0.z)*1e-1)*1e3);
	float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	return mix(r0, r1, f.z)*2.-1.;
}

const float2 iResolution=float2(1, 1);
float iTime=0.0;

const float speed = 7.0;

float map_z(float z)
{
    return z + iTime * speed;
}
float get_x(float3 p)
{
    return p.x + 1.2 * cos(map_z(p.z) * 0.1);
}

float f(float x, float z)
{
    return 0.5 * sin(map_z(z) * 0.2);
}

bool castRay(float3 ro, float3 rd, out float resT)
{
	//return true;
    	//return false;
    const float mint = 0.001;
    const float maxt = 50.0;
    const float dt = 1.0;
    float lh = 0.0;
    float ly = 0.0;
    
    float t = mint;
    
    for (float t = mint; t < maxt; t += dt)
    {
        float3 p = ro + rd * t;
        float h = f(p.x, p.z);
        if (p.y < h)
        {
            resT = t - dt + dt * (lh - ly) / (p.y - ly - h + lh);
            return true;
        }
        lh = h;
        ly = p.y;
    }
    return false;
}

float3 getNormal(float3 p)
{
    const float eps = 0.02;
    float3 n = float3(
        f(p.x - eps, p.z) - f(p.x + eps, p.z),
        2.0 * eps,
        f(p.x, p.z - eps) - f(p.x, p.z + eps));
    return normalize(n);  
}

float3 getShading(float3 p, float3 n)
{
    return dot(n, float3(0.0, 1.0, 0.0)) * float3(0.2, 0.7, 0.3);
}

float3 terrainColor(float3 ro, float3 rd, float t)
{
    float3 p = ro + rd * t;
    float3 p1 = float3(p.xy, map_z(p.z));
    float z = p1.z;
    
    float3 n = getNormal(p1);
    float3 s = getShading(p1, n);
    
    float waver = abs(get_x(p));
    float midline = step(0.7, waver);
    float mix_road = 1.0 - midline;
    float mix_line = (1.0 - smoothstep(0.0, 0.0025, pow(waver, 2.5))) * step(0.7, mod(z, 1.5));
    s = mix(s, float3(0.2, -0.1, 0.2), mix_road * 0.7);
    s = mix(s, float3(0.7, 0.7, 0.7), mix_line * 0.7);
    return s;
}

float3 skyColor(float2 fragCoord)
{
    float2 uv = fragCoord.xy / iResolution.xy;
    return float3(0.4, 0.6, 0.9 * uv.y);
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float aspect = iResolution.x / iResolution.y;
    const float hfov = radians(30.0);
    const float tanhfov = tan(hfov);
    const float near = 0.1;
    const float far = 1.0;
    
    float2 uv = fragCoord.xy / (iResolution.xy * 0.5) - float2(1.0, 1.0);
    float dx = tanhfov * uv.x / aspect;
    float dy = tanhfov * uv.y;
    
    float3 viewRayDir = normalize(float3(dx, dy, 1.0) * (far - near));
    
    float z = map_z(0.0);
    float bob = -0.3 + 0.025 * cos(z * 0.2);
    float swivel = 0.05 * sin(z * 0.1);
    #if 1
    float4x4 inverseViewMatrix = float4x4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        swivel, bob, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0);
    #else
    float4x4 inverseViewMatrix = float4x4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0);
    #endif
    
    float x = get_x(float3(0.0, 0.0, 0.0));
    float3 ro = float3(-x, 2.0, 0.0);
    float3 rd = (mul(inverseViewMatrix , float4(viewRayDir.x, viewRayDir.y, viewRayDir.z, 0.0))).xyz;
    //rd = float3(viewRayDir.x, viewRayDir.y, viewRayDir.z);
    float resT;
    
    #if 0
    castRay(ro, rd, resT);
    fragColor = float4(terrainColor(ro, rd, resT), 1.0);
    #else
    if (castRay(ro, rd, resT))
    {
        fragColor = float4(terrainColor(ro, rd, resT), 1.0);
    }
    else
    {
        fragColor = float4(skyColor(fragCoord), 1.0);
    }
    #endif
    //return float4(1,0,0,1);
    return fragColor;
}

