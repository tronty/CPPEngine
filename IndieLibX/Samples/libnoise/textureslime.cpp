// textureslime.cpp
//
// This program uses the libnoise library to generate texture maps consisting
// of shiny green slime.
//
// Copyright (C) 2004 Jason Bevins
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// (COPYING.txt) for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//

#include <noise/noise.h>

#include "noiseutils.h"

using namespace noise;

// Height of the texture.
const int TEXTURE_HEIGHT = 256;

// Creates the color gradients for the texture.
void CreateTextureColor (utils::RendererImage& renderer);

// Given a noise module, this function renders a flat square texture map and
// writes it to a Windows bitmap (*.bmp) file.  Because the texture map is
// square, its width is equal to its height.  The texture map can be seamless
// (tileable) or non-seamless.
void CreatePlanarTexture (const module::Module& noiseModule, bool seamless,
  int height, const char* filename);

// Given a noise module, this function renders a spherical texture map and
// writes it to a Windows bitmap (*.bmp) file.  The texture map's width is
// double its height.
void CreateSphericalTexture (const module::Module& noiseModule, int height,
  const char* filename);

// Given a noise map, this function renders a texture map and writes it to a
// Windows bitmap (*.bmp) file.
void RenderTexture (const utils::NoiseMap& noiseMap, const char* filename);



/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

TextureID texture=-1;

void CreateILTexture(ubyte *data, unsigned int width, unsigned int height, unsigned int bpp, TextureID& id)
{
	Image3 img;
	ubyte* data_=img.create(FORMAT_RGBA8, width, height, 1, 1);
	stx_memcpy(data_, data, width*height*bpp*sizeof(ubyte));
	img.swap(0, 2);
	id=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
}

int CreateTexture()
{
  // Large slime bubble texture.
  module::Billow largeSlime;
  largeSlime.SetSeed (0);
  largeSlime.SetFrequency (4.0);
  largeSlime.SetLacunarity (2.12109375);
  largeSlime.SetOctaveCount (1);
  largeSlime.SetNoiseQuality (QUALITY_BEST);

  // Base of the small slime bubble texture.  This texture will eventually
  // appear inside cracks in the large slime bubble texture.
  module::Billow smallSlimeBase;
  smallSlimeBase.SetSeed (1);
  smallSlimeBase.SetFrequency (24.0);
  smallSlimeBase.SetLacunarity (2.14453125);
  smallSlimeBase.SetOctaveCount (1);
  smallSlimeBase.SetNoiseQuality (QUALITY_BEST);

  // Scale and lower the small slime bubble values.
  module::ScaleBias smallSlime;
  smallSlime.SetSourceModule (0, smallSlimeBase);
  smallSlime.SetScale (0.5);
  smallSlime.SetBias (-0.5);

  // Create a map that specifies where the large and small slime bubble
  // textures will appear in the final texture map.
  module::RidgedMulti slimeMap;
  slimeMap.SetSeed (0);
  slimeMap.SetFrequency (2.0);
  slimeMap.SetLacunarity (2.20703125);
  slimeMap.SetOctaveCount (3);
  slimeMap.SetNoiseQuality (QUALITY_STD);

  // Choose between the large or small slime bubble textures depending on the
  // corresponding value from the slime map.  Choose the small slime bubble
  // texture if the slime map value is within a narrow range of values,
  // otherwise choose the large slime bubble texture.  The edge falloff is
  // non-zero so that there is a smooth transition between the two textures.
  module::Select slimeChooser;
  slimeChooser.SetSourceModule (0, largeSlime);
  slimeChooser.SetSourceModule (1, smallSlime);
  slimeChooser.SetControlModule (slimeMap);
  slimeChooser.SetBounds (-0.375, 0.375);
  slimeChooser.SetEdgeFalloff (0.5);

  // Finally, perturb the slime texture to add realism.
  module::Turbulence finalSlime;
  finalSlime.SetSourceModule (0, slimeChooser);
  finalSlime.SetSeed (2);
  finalSlime.SetFrequency (8.0);
  finalSlime.SetPower (1.0 / 32.0);
  finalSlime.SetRoughness (2);

  // Given the slime noise module, create a non-seamless texture map, a
  // seamless texture map, and a spherical texture map.
  CreatePlanarTexture (finalSlime, false, TEXTURE_HEIGHT,
    "textureplane.bmp");
  CreatePlanarTexture (finalSlime, true, TEXTURE_HEIGHT,
    "textureseamless.bmp");
  CreateSphericalTexture (finalSlime, TEXTURE_HEIGHT,
    "texturesphere.bmp");

  return 0;
}

void CreateTextureColor (utils::RendererImage& renderer)
{
  // Create a green slime palette.  A dirt brown color is used for very low
  // values while green is used for the rest of the values.
  renderer.ClearGradient ();
  renderer.AddGradientPoint (-1.0000, utils::Color (160,  64,  42, 255));
  renderer.AddGradientPoint ( 0.0000, utils::Color ( 64, 192,  64, 255));
  renderer.AddGradientPoint ( 1.0000, utils::Color (128, 255, 128, 255));
}

void CreatePlanarTexture (const module::Module& noiseModule, bool seamless,
  int height, const char* filename)
{
  // Map the output values from the noise module onto a plane.  This will
  // create a two-dimensional noise map which can be rendered as a flat
  // texture map.
  utils::NoiseMapBuilderPlane plane;
  utils::NoiseMap noiseMap;
  plane.SetBounds (-1.0, 1.0, -1.0, 1.0);
  plane.SetDestSize (height, height);
  plane.SetSourceModule (noiseModule);
  plane.SetDestNoiseMap (noiseMap);
  plane.EnableSeamless (seamless);
  plane.Build ();

  RenderTexture (noiseMap, filename);
}

void CreateSphericalTexture (const module::Module& noiseModule, int height,
  const char* filename)
{
  // Map the output values from the noise module onto a sphere.  This will
  // create a two-dimensional noise map which can be rendered as a spherical
  // texture map.
  utils::NoiseMapBuilderSphere sphere;
  utils::NoiseMap noiseMap;
  sphere.SetBounds (-90.0, 90.0, -180.0, 180.0); // degrees
  sphere.SetDestSize (height * 2, height);
  sphere.SetSourceModule (noiseModule);
  sphere.SetDestNoiseMap (noiseMap);
  sphere.Build ();

  RenderTexture (noiseMap, filename);
}

void RenderTexture (const utils::NoiseMap& noiseMap, const char* filename)
{
  // Create the color gradients for the texture.
  utils::RendererImage textureRenderer;
  CreateTextureColor (textureRenderer);

  // Set up us the texture renderer and pass the noise map to it.
  utils::Image destTexture;
  textureRenderer.SetSourceNoiseMap (noiseMap);
  textureRenderer.SetDestImage (destTexture);
  textureRenderer.EnableLight (true);
  textureRenderer.SetLightAzimuth (135.0);
  textureRenderer.SetLightElev (60.0);
  textureRenderer.SetLightContrast (2.0);
  textureRenderer.SetLightColor (utils::Color (255, 255, 255, 0));

  // Render the texture.
  textureRenderer.Render ();

#if 1
  int width  = destTexture.GetWidth  ();
  int height = destTexture.GetHeight ();
  int bpp = 4;// ??? CalcWidthByteCount (width)/width;
	ubyte *data=(ubyte *)destTexture.GetSlabPtr (0);
	CreateILTexture(data, width, height, bpp, texture);
#else
  // Write the texture as a Windows bitmap file (*.bmp).
  utils::WriterBMP textureWriter;
  textureWriter.SetSourceImage (destTexture);
  textureWriter.SetDestFilename (filename);
  textureWriter.WriteDestFile ();
#endif
}

ShaderID shd=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/libnoise/rendersample.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	//texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
	CreateTexture();
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("textureslime");	
	IInput*    i=STX_Service::GetInputInstance();
	{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

		const char* txt = "Initializing. Please wait a moment.";
		IRenderer::GetRendererInstance()->drawText(txt, 250, 250, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	}
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
