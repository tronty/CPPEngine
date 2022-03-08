/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* shdTxt="[Vertex shader]\n"
"struct VsIn {\n"
"	float2 position: POSITION;\n"
"	float2 texCoord: TEXCOORD0;\n"
"};\n"
"struct VsOut {\n"
"	float4 position: POSITION;\n"
"	float2 texCoord: TEXCOORD0;\n"
"};\n"
"ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;\n"
"VsOut main(VsIn In){\n"
"	VsOut Out=(VsOut)0;\n"
"	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));\n"
"	Out.texCoord = In.texCoord;\n"
"	return Out;\n"
"}\n"
"[Fragment shader]\n"
"struct VsOut {\n"
"	float4 position: POSITION;\n"
"	float2 texCoord: TEXCOORD0;\n"
"};\n"
"sampler2D Base;\n"
"float4 main(VsOut IN): COLOR {\n"
"	return tex2D(Base, IN.texCoord);\n"
"}\n";

int ApplicationLogic()
{
	#if 0
	LOG_FNLN;
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture");	
	LOG_FNLN;
	ShaderID shd = IRenderer::GetRendererInstance()->addHLSLShader(shdTxt, "main", "main");
	LOG_FNLN;
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	LOG_FNLN;
	VertexFormatID vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);

	LOG_FNLN;
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();

	LOG_FNLN;
	TextureID texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
	LOG_FNLN;
	ShaderID sha = IRenderer::GetRendererInstance()->addShaderFromFile("/RadeonTerrainDemo/LitVertex.hlsl", "mainVS", "mainPS");
	LOG_FNLN;
	//ShaderID sha = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.GLSL.hlsl", "main", "main");
	LOG_FNLN;
	stx_exit(0);
	ShaderID sha = IRenderer::GetRendererInstance()->addShaderFromFile("/DDSAnimation/DDSAnimation.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SkyFromSpace.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SkyFromAtmosphere.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/GroundFromSpace.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/GroundFromAtmosphere.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SpaceFromSpace.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SpaceFromAtmosphere.hlsl", "main", "main");
	LOG_FNLN;
	
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/cg_explosion/cg_disc_shock.cg", "main", "main");  
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/cg_explosion/cg_Volumelines.cg", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/cg_explosion/cg_disc.cg", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/cg_explosion/cg_explosion.cg", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/cg_ShinySprite/cg_ShinySprite.cg", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/cg_Volumelines/cg_Volumelines.cg", "main", "main");
	LOG_FNLN;



	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/vbomb.fx", "mainVS", "hotPS");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise1.hlsl", "mainVS", "fragmentShaderPNoise");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise2.hlsl", "mainVS", "fragmentShaderPNoise");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise3.hlsl", "mainVS", "fragmentShaderPNoise");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise4.hlsl", "mainVS", "fragmentShaderPNoise");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise5.hlsl", "mainVS", "fragmentShaderPNoise"); // fBm ???
	#endif
	LOG_FNLN;
	ShaderID sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/SimpleShader.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/DiffuseLighting.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/AmbientLighting.hlsl", "main", "main");
	LOG_FNLN;
/*
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/PhongLighting1.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/PhongLighting2.hlsl", "main", "main");
	LOG_FNLN;
*/
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/rest.hlsl", "main", "main");
	LOG_FNLN;
	sha = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/all.else.hlsl", "main", "main");
	LOG_FNLN;

	return 0;
}

