/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
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

// https://www.shadertoy.com/view/fsXczN
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
float4      iMouse=float4(1,1,1,1);
float4      iDate;                        // (year, month, day, time in seconds)

#define S smoothstep

float2x2 Rotate(float angle) {
    float s = sin(angle), c = cos(angle);
    return float2x2(c, -s, s, c);

}

float line(float2 uv, float len, float width) {
    
    float d = length(uv - float2(0, clamp(uv.y, 0.0, len))) ;
    float w = lerp(width, 0.001, S(0.0, 0.45, uv.y));
    d = S(0.005, 0.0, d-w);
    return d;
}

float divs(float2 uv, float len) {

    float d = length(uv - float2(0, clamp(uv.y, 0.45-len, 0.45))) ;
    d = S(0.005, 0.0, d);
    return  d;

}


float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    // Normalized pixel coordinates (from 0 to 1)
    float2 uv = (fragCoord - 0.5*iResolution.xy)/iResolution.y;
    

    // Time varying pixel color
    
    float board = 0.;
    
    for (float i = 0.; i < 2.0*3.141927; i += 2.0*3.141927/60.) {
        board = max(board, divs(uv*Rotate(i), 0.03));        
    }
    
    for (float i = 0.; i < 2.0*3.141927; i += 2.0*3.141927/12.) {
        board = max(board, divs(uv*Rotate(i), 0.05));        
    }

    
    
    float s = floor(iDate.w)/60.*2.0*3.1415927;
    float arrowS = line(uv*Rotate(s), 0.45, 0.01);

    float m = iDate.w/3600.*2.0*3.1415927;
    float arrowM = line(uv*Rotate(m), 0.40, 0.02)*0.9;

    float h = iDate.w/3600./12.*2.0*3.1415927;
    float arrowH = line(uv*Rotate(h), 0.35, 0.03)*0.8;

    float arrow = max(arrowS, arrowM);
    arrow = max(arrow, arrowH);
    
    float f=max(arrow,board);
    float3 col = float3(f,f,f);

    // Output to screen
    return float4(col, 1.0);
}

