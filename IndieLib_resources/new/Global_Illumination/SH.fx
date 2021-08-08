//-----------------------------------------------------------------------------
// File: D3D10\Global Illumination\SH.fx
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------





RasterizerState rsNoCull
{
	CullMode = NONE;
	FrontCounterClockwise = false;
	MultiSampleEnable = false;
};

DepthStencilState dsState
{
	DepthEnable = false;
	DepthWriteMask = false;
};

BlendState blendState
{
	BlendEnable[0] = false;
};

Texture3D YA : register(t0);
Texture3D Probe : register(t1);

struct VsIn
{
	float2 xy: XY;
	uint z: Z;
};

struct PsIn
{
	float4 pos: SV_Position;
	uint slice: Slice;
	uint target: SV_RenderTargetArrayIndex;
};

VsIn vsMain(VsIn In)
{
	return In;
}

[maxvertexcount(1)]
void gsMain(point VsIn In[1], inout PointStream<PsIn> Stream, uint primitiveID : SV_PrimitiveID)
{
	PsIn Out;

	Out.pos = float4(In[0].xy, 0, 1);
	Out.slice = 6 * primitiveID;
	Out.target = In[0].z;

	Stream.Append(Out);
}

struct PsOut
{
	float4 rCoeffs: SV_Target0;
	float4 gCoeffs: SV_Target1;
	float4 bCoeffs: SV_Target2;
};

PsOut psMain(PsIn In)
{
	float4 c0 = 0;
	float4 c1 = 0;
	float4 c2 = 0;

	for (int i = 0; i < 6; i++)
	{
		int slice = In.slice + i;
		for (int y = 0; y < PROBE_SIZE; y++)
		{
			[unroll]
			for (int x = 0; x < PROBE_SIZE; x++)
			{
				float3 l = Probe.Load(int4(x, y, slice, 0)).rgb;
				float4 ya = YA.Load(int4(x, y, i, 0));

				c0 += l.r * ya;
				c1 += l.g * ya;
				c2 += l.b * ya;
			}
		}
	}


	const float nf = (1.0 / (PROBE_SIZE * PROBE_SIZE));

	PsOut Out;
	Out.rCoeffs = c0 * nf;
	Out.gCoeffs = c1 * nf;
	Out.bCoeffs = c2 * nf;

	return Out;
}

[techniques]
<Textures>
	<Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="Render">
	<pass name="Shadow">
		<RasterizerState name="rsNoCull" />
		<BlendState name="blendState" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
		<DepthStencilState name="dsState" X="0" />
		<VertexShader profile="vs_4_0" name="vsMain" />
		<GeometryShader profile="gs_4_0" name="gsMain" />
		<PixelShader profile="ps_4_0" name="psMain" />
		<VertexFormat>
        		<FormatDesc Type="VERTEX" 	Format="FLOAT4" />
			<FormatDesc Type="NORMAL"	Format="FLOAT4" />
		</VertexFormat>
    	</pass>
	</technique>
</Techniques>

