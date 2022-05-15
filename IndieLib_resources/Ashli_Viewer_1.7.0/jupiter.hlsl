/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
// vertex to fragment shader io
struct VERTEX_OUT {
	float4 Position	: POSITION;
	float2 uv	: TEXCOORD1;
};

[Vertex shader]

struct VERTEX_IN {
    float3 Position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 UV		: TEXCOORD1;
};
ROW_MAJOR 
float4x4 worldViewProj 
MVPSEMANTIC
;

VERTEX_OUT main(VERTEX_IN IN)
{
	VERTEX_OUT OUT = (VERTEX_OUT)0;
    OUT.Position = mul(worldViewProj, float4(IN.Position, 1.0));
#if 0
    OUT.uv.x = IN.UV.y;
    OUT.uv.y = IN.UV.x;
#else
    OUT.uv = IN.UV;
#endif
	return OUT;
}

[Fragment shader]

sampler2D base;
float4 main(VERTEX_OUT IN) : COLOR
{
	return tex2D(base, IN.uv);
}

