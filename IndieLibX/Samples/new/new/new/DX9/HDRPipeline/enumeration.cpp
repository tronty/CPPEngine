/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//======================================================================
//
//      HIGH DYNAMIC RANGE RENDERING DEMONSTRATION
//      Written by Jack Hoxley, November 2005
//
//======================================================================

#include "Enumeration.h"

namespace HDREnumeration
{

//--------------------------------------------------------------------------------------
//  FindBestHDRFormat( )
//
//      DESC:
//          Due to High Dynamic Range rendering still being a relatively high-end
//          feature in consumer hardware, several basic properties are still not
//          uniformly available. Two particular examples are texture filtering and
//          post pixel-shader blending.
//
//          Substantially better results can be gained from this sample if texture
//          filtering is available. Post pixel-shader blending is not required. The
//          following function will enumerate supported formats in the following order:
//
//              1. Single Precision (128 bit) with support for linear texture filtering
//              2. Half Precision (64 bit) with support for linear texture filtering
//              3. Single Precision (128 bit) with NO texture filtering support
//              4. Half Precision (64 bit) with NO texture filtering support
//
//          If none of the these can be satisfied, the device should be considered
//          incompatable with this sample code.
//
//      PARAMS:
//          pBestFormat  : A container for the detected best format. Can be 0.
//
//      NOTES:
//          The pBestFormat parameter can be set to 0 if the specific format is
//          not actually required. Because of this, the function can be used as a
//          simple predicate to determine if the device can handle HDR rendering:
//
//              if( ( FindBestHDRFormat( 0 ) ) )
//              {
//                  // This device is not compatable.
//              }
//
//--------------------------------------------------------------------------------------
int FindBestHDRFormat( FORMAT& aBestFormat )
{
#if 0
	aBestFormat = FORMAT_RGBA16F;
#elif 1
	aBestFormat = FORMAT_RGBA32F;
#endif
    return 0;

}



//--------------------------------------------------------------------------------------
//  FindBestLuminanceFormat( )
//
//      DESC:
//          << See notes for 'FindBestHDRFormat' >>
//          The luminance calculations store a single intensity and maximum brightness, and
//          as such don't need to use textures with the full 128 or 64 bit sizes. D3D
//          offers two formats, G32R32F and G16R16F for this sort of purpose - and this
//          function will return the best supported format. The following function will 
//          enumerate supported formats in the following order:
//
//              1. Single Precision (32 bit) with support for linear texture filtering
//              2. Half Precision (16 bit) with support for linear texture filtering
//              3. Single Precision (32 bit) with NO texture filtering support
//              4. Half Precision (16 bit) with NO texture filtering support
//
//          If none of the these can be satisfied, the device should be considered
//          incompatable with this sample code.
//
//      PARAMS:
//          pBestLuminanceFormat    : A container for the detected best format. Can be 0.
//
//      NOTES:
//          The pBestLuminanceFormatparameter can be set to 0 if the specific format is
//          not actually required. Because of this, the function can be used as a
//          simple predicate to determine if the device can handle HDR rendering:
//
//              if( ( FindBestLuminanceFormat( 0 ) ) )
//              {
//                  // This device is not compatable.
//              }
//
//--------------------------------------------------------------------------------------
int FindBestLuminanceFormat( FORMAT& aBestLuminanceFormat )
{
#if 0
	aBestLuminanceormat = FORMAT_RG16F;
#elif 1
	aBestLuminanceFormat = FORMAT_RG32F;
#endif
    return 0;

}

}

