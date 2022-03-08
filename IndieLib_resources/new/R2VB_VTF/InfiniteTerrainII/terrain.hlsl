/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]

float4x4 mvp=float4x4(	0.805070, 0.000000, 0.000000, 0.000000, 
			0.000000, 1.073426, 0.000000, 0.000000, 
			0.000000, -0.800000, 1.000500, 1.000000, 
 			0.000000, 0.000000, -0.001001, 0.000000);
float3 camPos=float3(0.000000, 0.800000, 0.000000);

struct VsOut {
	float4 pos      : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal   : TEXCOORD1;
	float  fog      : TEXCOORD2;
	float2 scale    : TEXCOORD3;
};

SAMPLER2D Terrain;
VsOut main( float2 pos: POSITION0
            ,float2 texCoord : TEXCOORD0){
    	float2 uv;
	//uv=pos.xy;// ??? + 0.5;
	uv=texCoord;
	float4 height = SAMPLE2D(Terrain, uv);
	float4 vertex = float4(4.0 * pos.x, height.w, 4.0 * pos.y, 1);

	VsOut Out;
	Out.pos = mul(mvp, vertex);
	//Out.pos = vertex;
	Out.texCoord = pos * 16.0;
	Out.normal = height.xyz;
	Out.fog = saturate(0.65 * length(vertex.xyz - camPos));
	Out.scale = saturate(height.w * 1.2 - 0.3) * float2(0.6, 0.5);

	return Out;
}

[Fragment shader]

sampler2D Ground;

float3 lightDir=float3(0.000000, 0.600000, 0.800000);
float4 fogColor=float4(1.000000, 0.500000, 0.100000, 1.000000);

float4 main(float2 texCoord: TEXCOORD0, float3 normal: TEXCOORD1, float fog: TEXCOORD2, float2 scale: TEXCOORD3) : COLOR {
	float diffuse = saturate(dot(lightDir, normal) * scale.x + scale.y);
	float4 base = tex2D(Ground, texCoord);

	return lerp(diffuse * base, fogColor, fog);
}

