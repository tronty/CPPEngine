/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

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

float2 iResolution=float2(1,1);
sampler2D iChannel0;

float4 main(VsOut IN) : COLOR0
{
	float4 fragColor;
	float2 fragCoord=IN.uv;

	float2 uv = fragCoord.xy / iResolution.xy;
	fragColor = tex2D(iChannel0, uv);
    
    float3 rd = tex2D(iChannel0, float2(0.5, 0.5)).rgb;
    
    float res = .4;
    float steps = 3.14159*2.;
    float focus = 3.;
 	float depth = fragColor.a;
    
    float dist = smoothstep(0.0, 2., depth-focus)*3.*ddx(uv.x);
    float3 tcol = float3(.0);
    for (float i = 0.; i < steps; i = i + res)
    {
        float2 _uv = uv+float2(cos(i), sin(i))*dist;
        tcol += tex2D(iChannel0, _uv).rgb;    
    }

    fragColor.rgb = tcol/(steps/res);
    fragColor.rgb = smoothstep(0.0, 1.0, fragColor.rgb); // contrast
    fragColor.rgb = pow(fragColor.rgb, float3(1.0 / 2.2));
	return fragColor;
}

