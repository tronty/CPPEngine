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
// https://www.shadertoy.com/view/MsdBD2
// Hashscape
// https://www.shadertoy.com/view/MsdBD2
// LICENSE: http://unlicense.org/
#define SCALE 13

int iFrame=1;
#define SCALE 13

float4 hash( float2 p)
{   
	// uvec4 q = ( p.x + p.y + uvec4(3U*p.x, 2U*p.y, p.x, 0) )  >> SCALE;
	float4 q = abs(( p.x + p.y + abs(float4(3U*p.x, 2U*p.y, p.x, 0)) >> SCALE);
    float4 a = abs(p.x ^ (p.xxxx << q ));
    float4 b = abs(p.y ^ (p.yyyy << q ));
   	float4 n = abs((a & b) - a);
    
    n ^= n << 21;
    n ^= n << (n >> 27);
    
    return float4(n) / float(0xffffffffU);
}

float4 main( in float2 fragCoord:TEXCOORD ) : COLOR
{
    float2 uv = abs(fragCoord);
    float4 col = 0.5*sqrt(hash(uv+abs(float2(4U << SCALE))+abs(float2(1U,3U))*abs(int(iFrame))));
    return float4(col.xyz+col.yzw,1.0);
}

