/*
  Copyright (c) 2017 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

int ApplicationLogic()
{
	LOG_FNLN;
	ShaderID sha = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.GLSL.hlsl", "main", "main");
	LOG_FNLN;
	
	return 0;
}

