/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
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

// Inclusion Guards
#ifndef INC_HDRSCENE_H
#define INC_HDRSCENE_H

// Standard Library Headers

// Operating System Headers
#include <d3dx9.h>

// Forward Declarations
class stx_CModelViewerCamera;


// To provide some logical grouping to the functionality offered by this
// code it is wrapped up in a C++ namespace. As such, all code relating
// to the rendering of the original 3D scene containing the raw High Dynamic
// Range values is accessed via the HDRScene qualifier.
namespace HDRScene
{

// This function is called when the application is first initialized or
// shortly after a restore/lost-device situation. It is responsible for
// creating all the geometric and rendering resources required to generate
// the HDR scene.
int CreateResources(, const D3DSURFACE_DESC* pDisplayDesc );

// This following function is effectively the opposite to the above. It is
// invoked when it is necessary to remove any resources that were previously
// created. Typically called during a lost-device or termination event.
int DestroyResources();

// This method takes all the necessary resources and renders them
// to the HDR render target for later processing.
int RenderScene();

// This method allows the code to update any internal variables, especially
// movement/animation based ones.
int UpdateScene(, float fTime, stx_CModelViewerCamera* pCamera );

// The results of rendering the HDR scene feed into several other parts
// of the rendering pipeline used by this example. As such it is necessary
// for them to be able to retrieve a reference to the HDR source.
int GetOutputTexture( TextureID pTexture );

// This puts the HDR source texture onto the correct part of the GUI.
int DrawToScreen(, ID3DXFont* pFont, ID3DXSprite* pTextSprite,
                      TextureID pArrowTex );

// A useful utility method for the GUI - HDR pipelines can take up a large
// amount of VRAM, such that it can be useful to monitor.
__DWORD__ CalculateResourceUsage();

}
;


#endif // INC_HDRSCENE_H

	
