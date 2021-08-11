//-----------------------------------------------------------------------------
// File: D3D10\Global Illumination\Lighting.fx
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------




// Our textures
Texture2D Base: register(t0);
TextureCube ShadowMap: register(t1);
Texture1D LightGrad: register(t2);

Texture3D ShVolumeR[SH_COEFF_VECTORS]: register(t3);
Texture3D ShVolumeG[SH_COEFF_VECTORS]: register(t7);
Texture3D ShVolumeB[SH_COEFF_VECTORS]: register(t11);
TextureCube ShCube[SH_COEFF_VECTORS]: register(t15);

#define INV_RADIUS 0.02
#define CONST_BIAS 0
#define SLOPE_BIAS 0.001

// Sampler states are handled by the application
#if NVIDIA_WORKAROUND
SamplerState shadowFilter: register(s0);
#else
SamplerComparisonState shadowFilter: register(s0);
#endif

SamplerState baseFilter: register(s1);
SamplerState lmFilter: register(s2);
SamplerState volumeFilter: register(s3);

// Some states used in this shader
DepthStencilState dsState
{
	DepthEnable = true;
	DepthWriteMask = true;
	DepthFunc = LESS_EQUAL;
};

BlendState blendState
{
	BlendEnable[0] = false;
};

// Constant buffers separated by frequency of update
cbuffer PerFrame : register(b0)
{
	float4x4 mvp;
	float3 lightPos;
	float intensity;
	float3 camPos;
	float exposure;
	float4 lmModulate[2];
};

cbuffer PerObject : register(b1)
{
	float3 diffuse;
	float4 specular;
};

cbuffer GSMvp : register(b2)
{
	float4x4 mvpArray[PROBE_SLICES_PER_PASS];
}

cbuffer GSMvpInv : register(b3)
{
	float4x4 mvpInvArray[PROBE_SLICES_PER_PASS];
}

// Shader input and output structures for the different passes

struct VsInShadow
{
	float4 pos: Vertex;
	float4 normal: Normal;
};

struct GsInShadow
{
	float4 pos: Vertex;
};

struct PsInShadow
{
	float4 pos: SV_Position;
	float3 lightVec: LightVec;
	float2 texCoord: TexCoord;
	uint target: SV_RenderTargetArrayIndex;
};

struct VsInProbe
{
	float4 pos: Vertex;
	float4 normal: Normal;
};

struct GsInProbe
{
	float4 pos: Vertex;
	float4 normal: Normal;
};

struct PsInProbe
{
	float4 pos: SV_Position;
	float3 normal: Normal;
	float2 texCoord: TexCoord;
	uint target: SV_RenderTargetArrayIndex;
};

struct VsInFinal
{
	float4 pos: Vertex;
	float4 normal: Normal;
};

struct PsInFinal
{
	float4 pos: SV_Position;
	float2 texCoord: TexCoord;
	float3 normal: Normal;
	float3 lightVec: LightVec;
	float3 viewVec: ViewVec;
	float3 shCoord: ShCoord;
};

// We store texture coordinates packed in normal.w, then we use integer operations to extract it.
// This gives us more compact storage and improved performance.
float2 ExpandTexCoord(const float tc)
{
	uint t = asuint(tc);
	return float2((t & 0xFFFF) - 1024.0, (t >> 16) - 1024.0) * (1.0 / 1024.0);
}

// Shadow computation
float Shadow(const float3 lightVec, const float3 normal)
{
	float d = INV_RADIUS * length(lightVec);

	d -= SLOPE_BIAS / dot(normalize(lightVec), normalize(normal));

#if NVIDIA_WORKAROUND
	float depth = ShadowMap.Sample(shadowFilter, -lightVec).x;
	float shadow = float(d < depth + CONST_BIAS);
#else
	float shadow = ShadowMap.SampleCmpLevelZero(shadowFilter, -lightVec, d - CONST_BIAS).x;
#endif

	return shadow;
}

// Attenuation computation
float Attenuation(const float3 lightVec)
{
	return intensity / (1.0 + 0.05 * dot(lightVec, lightVec));
}

// Diffuse lighting computation
float Diffuse(const float3 lightVec, const float3 normal)
{
	float diffuse = saturate(dot(lightVec, normal));

	return diffuse;
}

// Indirect lighting computation
float3 Indirect(const float3 shCoord, const float3 normal)
{
	float3 indirect = 0;
	[unroll]
	for (int i = 0; i < SH_COEFF_VECTORS; i++)
	{
		// Get the SH coefficients from this point in space
		float4 cr = ShVolumeR[i].Sample(volumeFilter, shCoord);
		float4 cg = ShVolumeG[i].Sample(volumeFilter, shCoord);
		float4 cb = ShVolumeB[i].Sample(volumeFilter, shCoord);

		// Get the SH transfer function coefficients for the normal direction
		float4 sh = ShCube[i].Sample(volumeFilter, normal);

		// The indirect lighting is the integral over the environment sphere
		// which is a dot-product in SH space.
		indirect.r += dot(cr, sh);
		indirect.g += dot(cg, sh);
		indirect.b += dot(cb, sh);
	}

	return indirect;
}




GsInShadow vsShadow(VsInShadow In)
{
	GsInShadow Out;
	Out.pos.xyz = In.pos.xyz;
	Out.pos.w = In.normal.w;


	return Out;
}

[maxvertexcount(18)]
void gsShadow(triangle GsInShadow In[3], inout TriangleStream<PsInShadow> Stream, uniform bool masked)
{
	PsInShadow Out;

	[unroll]
	for (int k = 0; k < 6; k++)
	{
		Out.target = k;

		float4 pos[3];
		pos[0] = mul(mvpArray[k], float4(In[0].pos.xyz, 1));
		pos[1] = mul(mvpArray[k], float4(In[1].pos.xyz, 1));
		pos[2] = mul(mvpArray[k], float4(In[2].pos.xyz, 1));

		// Use frustum culling to improve performance
		float4 t0 = saturate(pos[0].xyxy * float4(-1, -1, 1, 1) - pos[0].w);
		float4 t1 = saturate(pos[1].xyxy * float4(-1, -1, 1, 1) - pos[1].w);
		float4 t2 = saturate(pos[2].xyxy * float4(-1, -1, 1, 1) - pos[2].w);
		float4 t = t0 * t1 * t2;

		[branch]
		if (!any(t))
		{
			// Use backface culling to improve performance
			float2 d0 = pos[1].xy * pos[0].w - pos[0].xy * pos[1].w;
			float2 d1 = pos[2].xy * pos[0].w - pos[0].xy * pos[2].w;

			[branch]
			if (masked || d1.x * d0.y > d0.x * d1.y || min(min(pos[0].w, pos[1].w), pos[2].w) < 0.0)
			{
				[unroll]
				for (int i = 0; i < 3; i++)
				{
					Out.pos = pos[i];
					Out.lightVec = INV_RADIUS * (lightPos - In[i].pos.xyz);
					Out.texCoord = ExpandTexCoord(In[i].pos.w);

					Stream.Append(Out);
				}

				Stream.RestartStrip();
			}
		}
	}
}

float psShadow(PsInShadow In, uniform bool masked) : SV_Target
{
	if (masked)
	{
		float4 tex = Base.Sample(baseFilter, In.texCoord);
		clip(tex.a - 0.5);
	}

	return length(In.lightVec);
}

GsInProbe vsProbe(VsInProbe In)
{
	GsInProbe Out;
	Out.pos = In.pos;
	Out.normal = In.normal;

	return Out;
}

[maxvertexcount(3 * PROBE_SLICES_PER_PASS)]
void gsProbe(triangle GsInProbe In[3], inout TriangleStream<PsInProbe> Stream)
{
	PsInProbe Out;

	[unroll]
	for (int k = 0; k < PROBE_SLICES_PER_PASS; k++)
	{
		Out.target = k;

		float4 pos[3];
		pos[0] = mul(mvpArray[k], In[0].pos);
		pos[1] = mul(mvpArray[k], In[1].pos);
		pos[2] = mul(mvpArray[k], In[2].pos);

		// Use frustum culling to improve performance
		float4 t0 = saturate(pos[0].xyxy * float4(-1, -1, 1, 1) - pos[0].w);
		float4 t1 = saturate(pos[1].xyxy * float4(-1, -1, 1, 1) - pos[1].w);
		float4 t2 = saturate(pos[2].xyxy * float4(-1, -1, 1, 1) - pos[2].w);
		float4 t = t0 * t1 * t2;

		[branch]
		if (!any(t))
		{
			// Use backface culling to improve performance
			float2 d0 = pos[1].xy * pos[0].w - pos[0].xy * pos[1].w;
			float2 d1 = pos[2].xy * pos[0].w - pos[0].xy * pos[2].w;

			[branch]
			if (d1.x * d0.y > d0.x * d1.y || min(min(pos[0].w, pos[1].w), pos[2].w) < 0.0)
			{
				[unroll]
				for (int i = 0; i < 3; i++)
				{
					Out.pos = pos[i];
					Out.normal.xyz = In[i].normal;
					Out.texCoord = ExpandTexCoord(In[i].normal.w);

					Stream.Append(Out);
				}
				Stream.RestartStrip();
			}
		}
	}
}

float3 psProbe(PsInProbe In, bool frontFace: SV_IsFrontFace, uniform bool textured, uniform bool lightMapped, uniform bool alphaTest, uniform int lmIndex = 0) : SV_Target
{
	// Compute world position from screen position
	float4 wp = mul(mvpInvArray[In.target], float4(In.pos.xyz, 1));
	float3 worldPos = wp.xyz / wp.w;

	float3 base = diffuse;

	if (textured)
	{
		float4 tex = Base.Sample(baseFilter, In.texCoord);
		if (alphaTest)
		{
			clip(tex.a - 0.5);
			if (!frontFace) In.normal.xyz = -In.normal.xyz;
		}
		else
		{
			base = tex.rgb;
		}
	}

	float3 shCoord = worldPos * SCALE * 0.5 + 0.5;

	float3 lightVec = lightPos - worldPos;

	float direct = Attenuation(lightVec) * Shadow(lightVec, In.normal.xyz) * Diffuse(normalize(lightVec), In.normal.xyz);
	float3 indirect = Indirect(shCoord, In.normal.xyz);
	float3 color = (0.9 * direct + 1.1 * indirect) * base;

	if (lightMapped)
	{
		float2 lm = Base.Sample(lmFilter, In.texCoord).xy * lmModulate[lmIndex];

		float3 lmColor0 = LightGrad.Sample(lmFilter, 0.5 * lmModulate[lmIndex].x);
		float3 lmColor1 = LightGrad.Sample(lmFilter, 0.5 * lmModulate[lmIndex].y);

 		color += diffuse * (lm.x * lmColor0 + lm.y * lmColor1);
	}

	return color;
}

PsInFinal vsFinal(VsInFinal In)
{
	PsInFinal Out;

	Out.pos = mul(mvp, In.pos);
	Out.normal = In.normal;
	Out.texCoord = ExpandTexCoord(In.normal.w);

	Out.lightVec = lightPos - In.pos.xyz;
	Out.viewVec  = camPos - In.pos.xyz;
	Out.shCoord  = In.pos * SCALE * 0.5 + 0.5;

	return Out;
}

float3 psFinal(PsInFinal In, bool frontFace: SV_IsFrontFace, uniform bool textured, uniform bool lightMapped, uniform bool alphaTest, uniform int lmIndex = 0) : SV_Target
{
	float3 base = diffuse;

	if (textured)
	{
		float4 tex = Base.Sample(baseFilter, In.texCoord);
		if (alphaTest)
		{
			clip(tex.a - 0.5);
			if (!frontFace) In.normal = -In.normal;
		}
		else
		{
			base = tex.rgb;
		}
	}

	float3 normal = normalize(In.normal);
	float3 lightVec = normalize(In.lightVec);
	float3 viewVec = normalize(In.viewVec);

	float shadow = Attenuation(In.lightVec) * Shadow(In.lightVec, In.normal);

	float direct = shadow * Diffuse(lightVec, normal);
	float3 indirect = Indirect(In.shCoord, In.normal);

	float3 color = (0.9 * direct + 1.1 * indirect) * base;

	color += specular.rgb * (shadow * pow(saturate(dot(reflect(-viewVec, normal), lightVec)), specular.a));

	if (lightMapped)
	{
		float2 lm = Base.Sample(lmFilter, In.texCoord).xy * lmModulate[lmIndex];

		float3 lmColor0 = LightGrad.Sample(lmFilter, 0.5 * lmModulate[lmIndex].x);
		float3 lmColor1 = LightGrad.Sample(lmFilter, 0.5 * lmModulate[lmIndex].y);

 		color += diffuse * (lm.x * lmColor0 + lm.y * lmColor1);
	}

	// Tone-mapping
	color = 1.0 - exp2(-exposure * color);

	return color;
}

VertexShader vsshadow = CompileShader(vs_4_0, vsShadow());

VertexShader vsprobe = CompileShader(vs_4_0, vsProbe());
GeometryShader gsprobe = CompileShader(gs_4_0, gsProbe());

VertexShader vsfinal = CompileShader(vs_4_0, vsFinal());


technique10 Render
{
	pass Shadow
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsshadow);
		SetGeometryShader(CompileShader(gs_4_0, gsShadow(false)));
		SetPixelShader(CompileShader(ps_4_0, psShadow(false)));
	}

	pass ShadowMasked
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsshadow);
		SetGeometryShader(CompileShader(gs_4_0, gsShadow(true)));
		SetPixelShader(CompileShader(ps_4_0, psShadow(true)));
	}

	pass Probe0
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsprobe);
		SetGeometryShader(gsprobe);
		SetPixelShader(CompileShader(ps_4_0, psProbe(false, false, false)));
	}

	pass Probe1
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsprobe);
		SetGeometryShader(gsprobe);
		SetPixelShader(CompileShader(ps_4_0, psProbe(true, false, true)));
	}

	pass Probe2
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsprobe);
		SetGeometryShader(gsprobe);
		SetPixelShader(CompileShader(ps_4_0, psProbe(true, false, false)));
	}

	pass Probe3
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsprobe);
		SetGeometryShader(gsprobe);
		SetPixelShader(CompileShader(ps_4_0, psProbe(false, true, false, 0)));
	}

	pass Probe4
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsprobe);
		SetGeometryShader(gsprobe);
		SetPixelShader(CompileShader(ps_4_0, psProbe(false, true, false, 1)));
	}

	pass Final0
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsfinal);
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psFinal(false, false, false)));
	}

	pass Final1
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsfinal);
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psFinal(true, false, true)));
	}

	pass Final2
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsfinal);
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psFinal(true, false, false)));
	}

	pass Final3
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsfinal);
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psFinal(false, true, false, 0)));
	}

	pass Final4
	{
		SetBlendState(blendState, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState(dsState, 0);

		SetVertexShader(vsfinal);
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psFinal(false, true, false, 1)));
	}
}
