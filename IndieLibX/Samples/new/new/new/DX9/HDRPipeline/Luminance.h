/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

//======================================================================
//
//      HIGH DYNAMIC RANGE RENDERING DEMONSTRATION
//      Written by Jack Hoxley, October 2005
//
//======================================================================

// System Includes
#include <d3dx9.h>

// Inclusion Guards
#ifndef INC_LUMINANCE_H
#define INC_LUMINANCE_H

// All functionality offered by this particular module
// is being wrapped up in a C++ namespace so as to make
// it clear what functions/etc.. belong together.
namespace Luminance
{

// This function creates the various textures and shaders used
// to compute the overall luminance of a given scene.
int CreateResources(, const D3DSURFACE_DESC* pDisplayDesc );

// The functional opposite to the above function - it's job is
// to make sure that all resources are cleanly and safely tidied up.
int DestroyResources();

// This is the core function for this module - it will perform all
// of the necessary rendering and sampling to compute the 1x1
// luminance texture.
int MeasureLuminance();

// This function will display all stages of the luminance chain
// so that the end-user can see exactly what happened.
int DisplayLuminance(, ID3DXFont* pFont, ID3DXSprite* pTextSprite,
                          TextureID pArrowTex );

// The final, 1x1 texture, result of this module is needed to
// compute the final image that the user sees, as such other
// parts of this example need access to the luminance texture.
int GetLuminanceTexture( TextureID pTex );

// A simple statistic that reveals how much texture memory is
// used by this particular module.
__DWORD__ ComputeResourceUsage();

}
;

#endif // INC_LUMINANCE_H

