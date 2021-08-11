//--------------------------------------------------------------------------------------
// File: BasicVTF.fx
//
// The effect file for the BasicVTF sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
#define FPTYPE half
#define FPTYPE2 half2
#define FPTYPE3 half3
#define FPTYPE4 half4

float4 g_MaterialDiffuseColor;      // Material's diffuse color

float3 g_LightDir;               // Light's direction in world space
float4 g_LightDiffuse;           // Light's diffuse color
float4 g_LightAmbient;              // Light's ambient color

float4x4 g_mWorld;                  // World matrix for object
float4x4 g_mWorldIT;
float4x4 g_mWorldView;
float4x4 g_mViewProj;
float4x4 g_mProjection;
float4x4 g_mWorldViewProjection;    // World * View * Projection matrix
float4x4 g_mViewInverse;
float3 g_upObjectSpace;

float4 g_SnowColor;

float SceneWidth;
float SceneHeight;
float OffsetAmount = 0;
float SampleDistance = 0;
float BaseSnowPct = 0.4;

float SpecExpon = 120.0;
float Ks = 0.26;
float Bumpy = 1.0;
float Kd = 0.9;
float SnowBias = 0.02;

float4x4 g_mExposureWorldViewOrthoProj;

float normalDistortionAmount = 0.1;
float dotNormalDistortionAmount = 0.2;

//--------------------------------------------------------------------------------------
// Textures & samplers
//--------------------------------------------------------------------------------------
#if 0
texture g_pTerrainTexture;              // Color texture for mesh
texture g_ExposureDepthMapTexture;              // Color texture for mesh
texture g_Noise3D;

sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_pTerrainTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler ExposureDepthMapSampler = 
sampler_state
{
	Texture = <g_ExposureDepthMapTexture>;
	AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = NONE;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

sampler ExposureDepthMapSamplerPixel = 
sampler_state
{
	Texture = <g_ExposureDepthMapTexture>;
	AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler NoiseSampler = 
sampler_state
{
	Texture = <g_Noise3D>;
	AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
#else
sampler2D MeshTextureSampler;
sampler2D ExposureDepthMapSampler;
sampler2D ExposureDepthMapSamplerPixel;
sampler3D NoiseSampler;
#endif

// Rather than stick everything in this file, pulled some util functions out into other files.

#define EXPOSURESAMPLER_VERTEX ExposureDepthMapSampler
#define EXPOSURESAMPLER_PIXEL ExposureDepthMapSamplerPixel
#define NOISESAMPLER NoiseSampler
#define NOISEOCTAVES 3.0
/*
	A couple of functions used by the different classes of snow techniques.
*/

/*
	Implements a perlin-style noise lookup using a 3D texture.  Returns a (-1..1) value
*/

#ifdef NOISESAMPLER
#ifndef NOISEOCTAVES
#define NOISEOCTAVES 3.0
#endif
FPTYPE noise(FPTYPE3 uvw)
{
	// $$ Note, these values, are tweakable.  I tweaked a little off the standard because I liked these better. YMMV
	FPTYPE sum = 0;
	FPTYPE amp = 1;
	FPTYPE3 p = uvw;
	FPTYPE lacunarity = 2;
	FPTYPE gain = 0.75;
	int octaves = (int)NOISEOCTAVES;
	for(int i=0;i<octaves;i++)
	{
		sum += amp * abs(tex3D(NOISESAMPLER, p.xyz));
		p *= lacunarity;
		amp *= gain;
	}
	return 2*sum-1;
}
#endif

/*
	This is used by the vertex shader in the SM3.0 technique to get the exposure for a vertex.
*/
#ifdef EXPOSURESAMPLER_VERTEX
float exposureAtVertex(float2 uv, float depth)
{
	float exposure = 0;
	if(uv.x > 0.0 && uv.x < 1.0 && uv.y > 0.0 && uv.y < 1.0 ) 
	{
		float sampledepth = tex2Dlod(EXPOSURESAMPLER_VERTEX,float4(uv.xy,0,0));
		if(depth < sampledepth+SnowBias)	// SnowBias is for depth fighting artifacts
		{
			exposure = 1;
		}
	}
	return exposure;
}
#endif

#ifdef EXPOSURESAMPLER_PIXEL
FPTYPE exposureAtPixel(FPTYPE2 uv, FPTYPE depth)
{
	FPTYPE exposure = 0;
	FPTYPE sampledepth = tex2D(EXPOSURESAMPLER_PIXEL,FPTYPE4(uv.xy,0,0));
	if(depth < sampledepth+SnowBias)	// SnowBias is for depth fighting artifacts
	{
		exposure = 1;
	}
	return exposure;
}
#endif

/*
	Lights a pixel using the exposure map, and noise to produce a really nice effect.
*/
FPTYPE4 SnowPixel(FPTYPE2 TextureUV, FPTYPE3 Normal, FPTYPE3 OrthoUVnDepth, FPTYPE3 ToView,
	const bool bNoiseNormal, const bool bNoiseIncline, const bool bMultiSample)
{
	// Prescale UV for noise, to have it tile a little more.
	FPTYPE3 noiseUVW = 30.0*OrthoUVnDepth.xyz;
	
	// Then just deterministically remap the coords to get 2 noise values to offset the normal
	FPTYPE3 noiseVec = FPTYPE3(noise(noiseUVW),0,noise(noiseUVW.yzx));
	
    // Calculate the per pixel exposure value.  multi-sampling around us to average
    FPTYPE exposure = 0;
    FPTYPE pixelOrthoDepth = OrthoUVnDepth.z;
	
	// current position
	exposure += exposureAtPixel(OrthoUVnDepth,pixelOrthoDepth);	

	if(bMultiSample)	// We want to sample the exposure map a couple times to blur the egdes a little
	{
		// A box around us.
		FPTYPE2 direction;	
		FPTYPE2 texelSize = SampleDistance / FPTYPE2(SceneWidth,SceneHeight);    
		direction = FPTYPE2(0,1);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(1,0);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(0,-1);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(-1,0);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		
		exposure /= 5;
	}
	
	FPTYPE3 vNormalWorldSpace = ((2*Normal)-FPTYPE3(1,1,1));
	if(bNoiseNormal) vNormalWorldSpace = normalize(normalDistortionAmount*noiseVec + vNormalWorldSpace);	
	
	FPTYPE dotNorm = vNormalWorldSpace.y; // effectively dot the normal with the up vector
	if(dotNorm <= 0) dotNorm = 0;
	else 
	{
		if(bNoiseIncline)
		{
			dotNorm = saturate(dotNorm+dotNormalDistortionAmount*noiseVec.x);
		}
	}

    FPTYPE3 Vn = normalize(2*ToView-1);
    FPTYPE3 Hn = normalize(Vn + g_LightDir);
    FPTYPE hdn = dot(Hn,vNormalWorldSpace);
    FPTYPE ldn = dot(g_LightDir,vNormalWorldSpace);
    FPTYPE4 litVec = lit(ldn,hdn,SpecExpon);
    FPTYPE3 diffContrib = litVec.y * g_LightDiffuse;
    
    // $$4 Note this is calculated before the full exposure and is thus not based on incline...
    //		This is because we dont want to tie the specular highlight to the noise function too directly, it doesn't look good.
    FPTYPE3 specContrib = exposure*((litVec.y * litVec.z * Ks) * g_LightDiffuse);
    
    // now also tweak based on angle of incline, add in a base %
	exposure = BaseSnowPct + exposure*dotNorm*(1-BaseSnowPct);

    // choose a color based on exposure
    FPTYPE3 diffuseColor = exposure*g_SnowColor + (1-exposure)*tex2D(MeshTextureSampler, TextureUV);    
	
	FPTYPE diffuseSpec = saturate(g_LightAmbient+Kd)*diffContrib+Ks*specContrib;
	return FPTYPE4(diffuseColor.xyz*(diffuseSpec),1);
}



//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
    float3 Normal	  : TEXCOORD1;		// vertex normal    
    float3 OrthoUVnDepth	  : TEXCOORD2;
    float3 ToView		: TEXCOORD3;
};


VS_OUTPUT RenderSceneVS_NoSnow( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0)
{
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;
	vNormalWorldSpace = normalize(mul(vNormal, (float3x3)g_mWorldIT)); // normal (world space)    
    
    // Transform the position from world space to homogeneous projection space
	Output.Position = mul(vPos, g_mWorldViewProjection);
    
    Output.Normal = 0.5*vNormalWorldSpace+float3(0.5,0.5,0.5);
    
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0;     
    
    Output.OrthoUVnDepth=float3(0,0,0);
    
    float4 wsP = mul(vPos,g_mWorld);
    Output.ToView = (0.5*normalize(g_mViewInverse[3].xyz - wsP))+0.5.xxx;	// obj coords
    
    return Output;
}

VS_OUTPUT RenderSceneVS_SM20( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0)
{
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;
    
    Output = RenderSceneVS_NoSnow(vPos,vNormal,vTexCoord0);

	// $ Vertex texture is used to pull out the exposure of the point
    float4 orthoPos = mul(vPos,g_mExposureWorldViewOrthoProj);
    orthoPos.y *= -1;	// UV coords have an inverted Y axis
    float2 uv = 0.5*orthoPos.xy + float2(0.5,0.5);
    
    Output.OrthoUVnDepth = float3(uv,orthoPos.z);
    
    return Output;    
}

VS_OUTPUT RenderSceneVS_SM30( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0)
{
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;

	// $ Vertex texture is used to pull out the exposure of the point
    float4 orthoPos = mul(vPos,g_mExposureWorldViewOrthoProj);
    orthoPos.y *= -1;	// UV coords have an inverted Y axis
    float2 uv = 0.5*orthoPos.xy + float2(0.5,0.5);
    
    Output.OrthoUVnDepth = float3(uv,orthoPos.z);
    
    float XSD = SampleDistance/SceneWidth;
    float YSD = SampleDistance/SceneHeight;
    float exposure = 0;
	
	// current position
	exposure += exposureAtVertex(uv,orthoPos.z);	
	
	// A box around us.  Capped to 3 more, since only 4 VTF per VS...
	exposure += exposureAtVertex(uv+float2(XSD,YSD),orthoPos.z);	
	exposure += exposureAtVertex(uv+float2(-XSD,YSD),orthoPos.z);	
	exposure += exposureAtVertex(uv+float2(0,-YSD),orthoPos.z);	
	
	exposure /= 4;
	
	// $ Note these two operations are independant of depth, so mask load lag.
    // Transform the normal from object space to world space    
    vNormalWorldSpace = normalize(mul(vNormal, (float3x3)g_mWorldIT)); // normal (world space)
    
    float normalDotUp = saturate(dot(vNormalWorldSpace,float3(0,1,0)));
//	exposure = exposure*pow(normalDotUp,10);
	exposure = exposure*normalDotUp;
  
  	// $ Offset the vertex a little if it is accumulating snow
  	float4 wsP = mul(vPos,g_mWorld);
  	float4 wsP2 = float4(wsP.xyz + OffsetAmount*exposure*vNormalWorldSpace,vPos.w);
  	
    // Transform the position from world space to homogeneous projection space
    Output.Position = mul(wsP2, g_mViewProj);
    
    Output.Normal = 0.5*vNormalWorldSpace+float3(0.5,0.5,0.5);
    
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    
    Output.ToView = (0.5*normalize(g_mViewInverse[3].xyz - wsP))+0.5.xxx;	// obj coords
    
    return Output;    
}

float4 RenderScenePS_NoSnow( VS_OUTPUT In )  : COLOR
{
	FPTYPE3 vNormalWorldSpace = (2*In.Normal)-FPTYPE3(1,1,1);	

    FPTYPE3 Vn = normalize(2*In.ToView-1);
    FPTYPE3 Hn = normalize(Vn + g_LightDir);
    FPTYPE hdn = dot(Hn,vNormalWorldSpace);
    FPTYPE ldn = dot(g_LightDir,vNormalWorldSpace);
    FPTYPE4 litVec = lit(ldn,hdn,SpecExpon);
    FPTYPE3 diffContrib = litVec.y * g_LightDiffuse;
    
    FPTYPE3 specContrib = ((litVec.y * litVec.z * Ks) * g_LightDiffuse);
    
    // choose a color based on exposure
    FPTYPE3 diffuseColor = tex2D(MeshTextureSampler, In.TextureUV);    
	
	FPTYPE diffuseSpec = saturate(g_LightAmbient+Kd)*diffContrib+Ks*specContrib;
	
	return float4(diffuseColor.xyz*(diffuseSpec),1);
}    


float4 RenderScenePS( VS_OUTPUT In)  : COLOR
{ 
   return SnowPixel(In.TextureUV,In.Normal,In.OrthoUVnDepth,In.ToView,true,true,true);
}

//--------------------------------------------------------------------------------------
// Renders scene to render target
//--------------------------------------------------------------------------------------
[techniques]
<!-- Textures>
	    <Texture sampler="MeshTextureSampler" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
	    <Texture sampler="ExposureDepthMapSampler" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
	    <Texture sampler="ExposureDepthMapSamplerPixel" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
	    <Texture sampler="NoiseSampler" file="/new/x/x/SnowAccumulation/noiseL8_32x32x32.dds" dimension="3" mipmap="false" SamplerState="linear" />
</Textures -->
<Techniques>
<technique name="RenderScene_SM20">
    <pass name="P0">
        <VertexShader  profile="vs_2_0" name="RenderSceneVS_SM20" />
        <PixelShader   profile="ps_2_a" name="RenderScenePS" />
	<VertexFormat>
        	<FormatDesc Type="VERTEX" 	Format="FLOAT4" />
		<FormatDesc Type="NORMAL" 	Format="FLOAT3" />
		<FormatDesc Type="TEXCOORD"	Format="FLOAT2" />
	</VertexFormat>
    </pass>
</technique>
<technique name="RenderScene_SM30">
    <pass name="P0">
        <VertexShader  profile="vs_3_0" name="RenderSceneVS_SM30" />
        <PixelShader   profile="ps_3_0" name="RenderScenePS" />
	<VertexFormat>
        	<FormatDesc Type="VERTEX" 	Format="FLOAT4" />
		<FormatDesc Type="NORMAL" 	Format="FLOAT3" />
		<FormatDesc Type="TEXCOORD"	Format="FLOAT2" />
	</VertexFormat>
    </pass>
</technique>
<technique name="RenderScene_NoSnow">
    <pass name="P0">
        <VertexShader  profile="vs_3_0" name="RenderSceneVS_NoSnow" />
        <PixelShader   profile="ps_3_0" name="RenderScenePS_NoSnow" />
	<VertexFormat>
        	<FormatDesc Type="VERTEX" 	Format="FLOAT4" />
		<FormatDesc Type="NORMAL" 	Format="FLOAT3" />
		<FormatDesc Type="TEXCOORD"	Format="FLOAT2" />
	</VertexFormat>
    </pass>
</technique>
</Techniques>

