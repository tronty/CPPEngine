//--------------------------------------------------------------------------------------
// File: skyboxNEW.fx
//
// 
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
cbuffer cb0
{
	float4x4 g_mViewProjection;			// View * Projection matrix 
	float g_SunAlpha, g_SunTheta, g_SunShininess, g_SunStrength; // variables for Skybox sun
}

TextureCube g_EnvironmentMap;					// Sky texture

//--------------------------------------------------------------------------------------
// SamplerStates
//--------------------------------------------------------------------------------------

SamplerState samSkybox
{  
    Filter = MIN_MAG_MIP_LINEAR;
	AddressU  = CLAMP;		
    AddressV  = CLAMP;
    AddressW  = CLAMP;
};

//--------------------------------------------------------------------------------------
// Input/Output Structures
//--------------------------------------------------------------------------------------

struct RenderSkyboxVSOUT
{
    float4  Position    : SV_POSITION0; // position of sphere
	float3  SunPosition	: TEXCOORD0;	// position of sun
	float3	SkyTexUVW	: TEXCOORD1;	// texture coordinates
};

//--------------------------------------------------------------------------------------
// Shader Functions
//--------------------------------------------------------------------------------------

RenderSkyboxVSOUT RenderSkyboxVS( float4 vPos : POSITION )
{
	RenderSkyboxVSOUT Output;
	float zfar = 4000;
			
	Output.Position = mul( zfar*vPos, g_mViewProjection );
	
	Output.SkyTexUVW = vPos;
	Output.SunPosition.x = cos(g_SunTheta)*sin(g_SunAlpha);
	Output.SunPosition.y = sin(g_SunTheta);
	Output.SunPosition.z = cos(g_SunTheta)*cos(g_SunAlpha);
	
	return Output;
}

float4 RenderSkyboxPS( RenderSkyboxVSOUT Input ) : SV_TARGET
{
	float4 Output;
	
    float3 sunlight = g_SunStrength*pow( saturate(dot(normalize(Input.SkyTexUVW), Input.SunPosition)),
		g_SunShininess)*float3(1.2, 0.4, 0.1);
	
	Output.a = 1;
	Output.rgb = g_EnvironmentMap.Sample( samSkybox,Input.SkyTexUVW )  + sunlight;
	return Output;
}

//--------------------------------------------------------------------------------------
// Shader Techniques
//--------------------------------------------------------------------------------------
[techniques]
<Textures>
	    <Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="RenderSkybox">
	<pass name="P0">
        	<VertexShader profile="vs_4_0" name="RenderSkyboxVS" />
        	<GeometryShader name="" />
        	<PixelShader profile="ps_4_0" name="RenderSkyboxPS" />
	<VertexFormat>
        	<FormatDesc Type="VERTEX" 	Format="FLOAT2" />
		<FormatDesc Type="TEXCOORD"	Format="FLOAT2" />
	</VertexFormat>
    </pass>
</technique>
</Techniques>

