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
// https://www.shadertoy.com/view/ltBXDm
// Emission clouds
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
sampler2D iChannel0;
// based on https://www.shadertoy.com/view/ls2SDd
// noise_ from https://www.shadertoy.com/view/XslGRr
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0

int MAX_RAY_STEPS = 64;

float noise_( in float3 x )
{
    float3 p = floor(x);
    float3 f = frac(x);
	f = f*f*(3.0-2.0*f);
	float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
	float2 rg = tex2D( iChannel0, (uv+ 0.5)/256.0 ).yx;
	return -1.0+2.0*lerp( rg.x, rg.y, f.z );
}

float2 rotate2d(float2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return float2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
	float2 screenPos = (fragCoord.xy / iResolution.xy) * 2.0 - 1.0;
	float3 cameraDir = float3(0.0, 0.0, 1.4);
	float3 cameraPlaneU = float3(1.0, 0.0, 0.0);
	float3 cameraPlaneV = float3(0.0, 1.0, 0.0) * iResolution.y / iResolution.x;
	float3 rayDir = cameraDir + screenPos.x * cameraPlaneU + screenPos.y * cameraPlaneV;
	float3 rayPos = float3(80.0, 12.0 * sin(iTime / 4.7), 0.0);
		
    
    rayDir.y -= .2 * sin(iTime / 4.7);
    rayDir = normalize(rayDir);
    
	rayPos.xz = rotate2d(rayPos.xz, iTime / 2.0);
	rayDir.xz = rotate2d(rayDir.xz, iTime / 2.0 + 3.14 / 2.0);
    
    
    float dis = 0.0;
    float t1 = 2.0;
    float3 dir = float3(0.,1.,0.);
    float val;
    
    float3 col = float3(0,0,0);
    for(int i=0;i<MAX_RAY_STEPS;i++){
	    //////////////////////////////////
    	// participating media    
    	float3 q = rayPos - dir* t1; val  = 0.50000*noise_( q * .05 );
		q = q*2.0 - dir* t1; val += 0.25000*noise_( q * .05  );
		q = q*2.0 - dir* t1; val += 0.12500*noise_( q * .05  );
		q = q*2.0 - dir* t1; val += 0.06250*noise_( q * .05  );
        q = q*2.5 - dir* t1; val += 0.03125*noise_( q * .8  );
        //////////////////////////////////
        
        float t = max(5.0 * val - .9, 0.0);
        
        col += sqrt(dis) * .1 * float3(0.5 * t * t * t, .6 * t * t, .7 * t);
        
        dis += 1.0 / float(MAX_RAY_STEPS);
        
        rayPos += rayDir * 1.0/ (.4);
    }
    
    col = min(col, 1.0) - .34 * (log(col + 1.0));
    
    fragColor = float4(sqrt(col.rgb), 1.0);
	return fragColor;
}
