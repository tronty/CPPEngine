//-----------------------------------------------------------------------------
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS 
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL 
// NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR 
// CONSEQUENTIAL DAMAGES WHATSOEVER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR 
// LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS 
// INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR 
// INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGES.
//
// This file shows how to render a scene using D3D Instancing.  This shader
//		implements a Blinn diffuse, specular and bump equation
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float4 g_MaterialAmbientColor;      // Material's ambient color
float4 g_MaterialDiffuseColor;      // Material's diffuse color

float3 g_LightDir;					// Light's direction in world space
float4 g_LightColor;				// Light's ambient color

texture g_MeshTexture;              // Color texture for mesh
texture g_MeshBumpTexture;			// Mesh bump texture

float4x4 g_mWorld;                  // World matrix for object
float4x4 g_mViewProj;
float4x4 g_mViewInverse;

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;    
};

sampler MeshBumpTextureSampler = 
sampler_state
{
    Texture = <g_MeshBumpTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;    
};

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
    float3 WorldNormal	: TEXCOORD1;
    float3 WorldView	: TEXCOORD2;
    float3 WorldTangent	: TEXCOORD3;
    float3 WorldBinorm	: TEXCOORD4;
    float4 Color		: COLOR;
};

/*
	This shared shader code
*/
VS_OUTPUT SharedVS(		 float4 vPos, 
                         float3 vNormal,
                         float3 vTangent,
                         float3 vBinormal,
                         float2 vTexCoord0,
                         float4x4 mWorld,
                         float4 cInstanceColor)
{
    VS_OUTPUT Output;

	// Put object info into world space									
    Output.WorldNormal = (0.5*mul(vNormal,mWorld).xyz)+0.5.xxx;
    Output.WorldTangent = (0.5*mul(vTangent,mWorld).xyz)+0.5.xxx;
    Output.WorldBinorm = (0.5*mul(vBinormal,mWorld).xyz)+0.5.xxx;
    
    float4 Po = float4(vPos.xyz,1.0);				// object coordinates
    float3 Pw = mul(Po,mWorld).xyz;					// world coordinates

    Output.WorldView = (0.5*normalize(g_mViewInverse[3].xyz - Pw))+0.5.xxx;	// obj coords
    Output.Position = mul(float4(Pw,1.0),g_mViewProj);			// screen clipspace coords
										
    Output.Color = cInstanceColor;
      
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    return Output;
    
}

/**
	A Regular VS
*/
VS_OUTPUT NormalVS(float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float3 vTangent : TANGENT,
                         float3 vBinormal : BINORMAL,                         
                         float2 vTexCoord0 : TEXCOORD0)
{
	return SharedVS(vPos,vNormal,vTangent,vBinormal,vTexCoord0,g_mWorld,g_MaterialDiffuseColor); 
}


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting and uses a matrix passed in on 2nd stream
//		to position object
//--------------------------------------------------------------------------------------

VS_OUTPUT InstancedVS( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float3 vTangent : TANGENT,
                         float3 vBinormal : BINORMAL,
                         float2 vTexCoord0 : TEXCOORD0,
                         float4 vInstanceMatrix1 : TEXCOORD1,
                         float4 vInstanceMatrix2 : TEXCOORD2,
                         float4 vInstanceMatrix3 : TEXCOORD3,
                         float4 cInstanceColor : COLOR)
{
    VS_OUTPUT Output;

	// We've encoded the 4x3 world matrix in a 3x4, so do a quick transpose so we can use it in DX
	float4 row1 = float4(vInstanceMatrix1.x,vInstanceMatrix2.x,vInstanceMatrix3.x,0);
	float4 row2 = float4(vInstanceMatrix1.y,vInstanceMatrix2.y,vInstanceMatrix3.y,0);
	float4 row3 = float4(vInstanceMatrix1.z,vInstanceMatrix2.z,vInstanceMatrix3.z,0);
	float4 row4 = float4(vInstanceMatrix1.w,vInstanceMatrix2.w,vInstanceMatrix3.w,1);
	float4x4 mInstanceMatrix = float4x4(row1,row2,row3,row4);
										
	return SharedVS(vPos,vNormal,vTangent,vBinormal,vTexCoord0,mInstanceMatrix,cInstanceColor);
}

//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
float SpecExpon : SpecularPower = 120.0;
float Ks = 1.2;
float Bumpy = 1.0;
float Kd = 1.0;

float4 ShipPS(VS_OUTPUT IN) : COLOR 
{
    float3 Nn = normalize(2*IN.WorldNormal-1);
    float3 Tn = normalize(2*IN.WorldTangent-1);
    float3 Bn = normalize(2*IN.WorldBinorm-1);
    float4 normalMap = tex2D(MeshBumpTextureSampler,IN.TextureUV);
    float2 bumps = Bumpy * (normalMap.xy-(0.5).xx);
    Nn += (bumps.x * Tn + bumps.y * Bn);
    Nn = normalize(Nn);

    float3 Vn = normalize(2*IN.WorldView-1);    
    Nn = -faceforward(Nn,Vn,Nn);

    float3 Hn = normalize(Vn + g_LightDir);
    float hdn = dot(Hn,Nn);
    float ldn = dot(g_LightDir,Nn);
    float4 litVec = lit(ldn,hdn,SpecExpon);

    float4 colorTex = tex2D(MeshTextureSampler,IN.TextureUV).xyzw;        
    
    float3 diffContrib = litVec.y * g_LightColor;
    //diffContrib = max(colorTex.w,diffContrib);    
    
    float3 specContrib = ((litVec.y * litVec.z * Ks) * g_LightColor);
    
    // add, incorporating ambient light term (alpha channel is a "colorize term")
    float colorize = normalMap.w;
    colorTex.xyzw = colorize*float4(IN.Color.xyz,0) + (1-colorize)*colorTex.xyzw;

	// more metalness will modulate more of the sp[ecular with the color map
	// blue channel is the metalness of object
    float metalness = 1-normalMap.z;
	float3 result = colorTex.xyz*(Kd*diffContrib+g_MaterialAmbientColor+metalness*specContrib) + (1-metalness)*specContrib;
	
    return float4(result.xyz,colorTex.w);
}

float4 RockPS(VS_OUTPUT IN) : COLOR 
{
    float3 Nn = normalize(2*IN.WorldNormal-1);
    float3 Tn = normalize(2*IN.WorldTangent-1);
    float3 Bn = normalize(2*IN.WorldBinorm-1);
    float3 bumps = Bumpy * (tex2D(MeshBumpTextureSampler,IN.TextureUV).xyz-(0.5).xxx);
    Nn += (bumps.x * Tn + bumps.y * Bn);
    Nn = normalize(Nn);
    
    float3 Vn = normalize(2*IN.WorldView-1);
    float3 Hn = normalize(Vn + g_LightDir);
    float hdn = dot(Hn,Nn);
    float ldn = dot(g_LightDir,Nn);
    float4 litVec = lit(ldn,hdn,SpecExpon);
    float3 diffContrib = litVec.y * g_LightColor;
    
    float3 specContrib = ((litVec.y * litVec.z * Ks) * g_LightColor);
    
    // add, incorporating ambient light term
    float4 colorTex = float4(IN.Color.xyz,1.0) * tex2D(MeshTextureSampler,IN.TextureUV).xyzw;

	// more metalness will modulate more of the sp[ecular with the color map
	float3 result = colorTex.xyz*(Kd*diffContrib+g_MaterialAmbientColor);
	
	// write out result, storeing color map alpha in alpha channel
    return float4(result.xyz,colorTex.w);
}

float4 MShipPS(VS_OUTPUT IN) : COLOR 
{
    float3 Nn = normalize(2*IN.WorldNormal-1);
    float3 Tn = normalize(2*IN.WorldTangent-1);
    float3 Bn = normalize(2*IN.WorldBinorm-1);
    float4 normalMap = tex2D(MeshBumpTextureSampler,IN.TextureUV);
    float2 bumps = Bumpy * (normalMap.xy-(0.5).xx);
    Nn += (bumps.x * Tn + bumps.y * Bn);
    Nn = normalize(Nn);
     
    float3 Vn = normalize(2*IN.WorldView-1);
    float3 Hn = normalize(Vn + g_LightDir);
    float hdn = dot(Hn,Nn);
    float ldn = dot(g_LightDir,Nn);
    float4 litVec = lit(ldn,hdn,SpecExpon);
    float3 diffContrib = litVec.y * g_LightColor;
    
    float3 specContrib = ((litVec.y * litVec.z * Ks) * g_LightColor);
    
    // add, incorporating ambient light term (alpha channel is a "colorize term")
    float4 colorTex = tex2D(MeshTextureSampler,IN.TextureUV).xyzw;

	// more metalness will modulate more of the sp[ecular with the color map
	// blue channel is the metalness of object
    float metalness = 1.0;
	float3 result = colorTex.xyz*(Kd*diffContrib+g_MaterialAmbientColor+metalness*specContrib) + (1-metalness)*specContrib;
	
    return float4(result.xyz,colorTex.w);
}

//--------------------------------------------------------------------------------------
// Renders scene to render target
//--------------------------------------------------------------------------------------
technique RenderSceneInstance
{
    pass P0
    {    
		cullmode = none;      
        VertexShader = compile vs_2_0 InstancedVS();
        PixelShader  = compile ps_2_a ShipPS(); 
    }
}

technique RenderSceneNormal
{
    pass P0
    {    
		cullmode = none;          
        VertexShader = compile vs_2_0 NormalVS();
        PixelShader  = compile ps_2_0 ShipPS(); 
    }
}

technique RenderRockInstance
{
    pass P0
    {          
        VertexShader = compile vs_2_0 InstancedVS();
        PixelShader  = compile ps_2_a RockPS(); 
    }
}

technique RenderRockNormal
{
    pass P0
    {          
        VertexShader = compile vs_2_0 NormalVS();
        PixelShader  = compile ps_2_0 RockPS();
    }
}

technique RenderMSNormal
{
    pass P0
    {          
        VertexShader = compile vs_2_0 NormalVS();
        PixelShader  = compile ps_2_0 MShipPS(); 
    }
}