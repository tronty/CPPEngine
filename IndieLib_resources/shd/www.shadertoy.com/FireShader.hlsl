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
// https://www.shadertoy.com/view/XsXXRN
// Fire Shader
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
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;


        float rand(float2 n) {
            return fract(cos(dot(n, float2(12.9898, 4.1414))) * 43758.5453);
        }

        float noise_(float2 n) {
            const float2 d = float2(0.0, 1.0);
            float2 b = floor(n), f = smoothstep(float2(0, 0), float2(1, 1), fract(n));
            return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
        }

        float fbm(float2 n) {
            float total = 0.0, amplitude = 1.0;
            for (int i = 0; i < 4; i++) {
                total += noise_(n) * amplitude;
                n += n;
                amplitude *= 0.5;
            }
            return total;
        }



float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
            const float3 c1 = float3(0.5, 0.0, 0.1);
            const float3 c2 = float3(0.9, 0.0, 0.0);
            const float3 c3 = float3(0.2, 0.0, 0.0);
            const float3 c4 = float3(1.0, 0.9, 0.0);
            const float3 c5 = float3(0.1, 0.1, 0.1);
            const float3 c6 = float3(0.9, 0.9, 0.9);
			
			float2 speed = float2(0.7, 0.4);
			float shift = 1.6;
			float alpha = 1.0;

            float2 p = fragCoord.xy * 8.0 / iResolution.xx;
            float q = fbm(p - iTime * 0.1);
            float2 r = float2(fbm(p + q + iTime * speed.x - p.x - p.y), fbm(p + q - iTime * speed.y));
            float3 c = mix(c1, c2, fbm(p + r)) + mix(c3, c4, r.x) - mix(c5, c6, r.y);
            fragColor = float4(c * cos(shift * fragCoord.y / iResolution.y), alpha);
	return fragColor;
        }
