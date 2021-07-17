/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

// find .. -type f -name "*.h" -exec grep -l "ifndef SWIG" {} \;

    /* IndieLib.i */
     %module IndieLib
     %include "carrays.i"
     %array_functions(int, intArray);
     %array_functions(float, floatArray);

     %{
	//include "../Libraries/assimp-3.2/include"
	#include "../Libraries/STX/include/STX/STX.h"
     	#include "../Libraries/STX/include/STX/d3dx9mathFromWine.h"
	#include "../Libraries/STX/include/STX/d3dx9mathFromWine.inl"
     	#include "../Libraries/STX/include/STX/STXinterfaces.h"
     	#include "../Libraries/Framework3/include/Framework3/IRenderer.h"
     	//include "../Libraries/Framework3/include/Framework3/Colors.h"
     	//include "../Libraries/Framework3/include/Framework3/Turtle.h"
     	#include "../Libraries/MeshRenderer2/MeshRenderer2.h"
     	//include "../Libraries/Framework3/include/Framework3/RendererHLSLCg.h"
     	//include "../Libraries/spine-2.3/spine-c/include/spine/Atlas.h"
     	//include "../Libraries/spine-2.3/spine-c/include/spine/SkeletonJson.h"
     	////include "../Libraries/spine-2.3/spine-c/include/spine/spine.h"
	////include "../Libraries/spine-2.3/spine-sfml/src/spine/spine-sfml.h"
	#include "../Libraries/NSGUI/GUI/GUIEvent.h"
	#include "../Libraries/NSGUI/GUI/GUIEventListener.h"
	#include "../Libraries/NSGUI/GUI/GUIUtils.h"
	#include "../Libraries/NSGUI/GUI/GUITexCoordDescriptor.h"
	#include "../Libraries/FW3/include/FW3.h"
	#include "../Libraries/FW3/include/Shape3D.h"
#if 1
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/Defines.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IndieLib.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Window.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Render.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Timer.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Math.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity2d.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity3d.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity2dManager.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity3dManager.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Input.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Image.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Light.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Surface.h"

    	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Animation.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Font.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_3dMesh.h"

     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_ImageManager.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_SurfaceManager.h"
    	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_AnimationManager.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_FontManager.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_3dMeshManager.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Camera2d.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Camera3d.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_LightManager.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Factory.h"
     	#include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/CIndieLib.h"
#endif
     %}

	//include "../Libraries/assimp-3.2/port/swig/assimp.i"
     %include "../Libraries/STX/include/STX/STX.h"
     %include "../Libraries/STX/include/STX/d3dx9mathFromWine.h"
     %include "../Libraries/STX/include/STX/d3dx9mathFromWine.inl"
     %include "../Libraries/STX/include/STX/STXinterfaces.h"
     %include "../Libraries/Framework3/include/Framework3/IRenderer.h"
     	//include "../Libraries/Framework3/include/Framework3/Colors.h"
     	//include "../Libraries/Framework3/include/Framework3/Turtle.h"
     %include "../Libraries/MeshRenderer2/MeshRenderer2.h"
     //include "../Libraries/Framework3/include/Framework3/RendererHLSLCg.h"
     	//include "../Libraries/spine-2.3/spine-c/include/spine/Atlas.h"
     	//include "../Libraries/spine-2.3/spine-c/include/spine/SkeletonJson.h"
	////include "../Libraries/spine-2.3/spine-c/include/spine/spine.h"
     ////include "../Libraries/spine-2.3/spine-sfml/src/spine/spine-sfml.h"
     %include "../Libraries/NSGUI/GUI/GUIEvent.h"
     %include "../Libraries/NSGUI/GUI/GUIEventListener.h"
     %include "../Libraries/NSGUI/GUI/GUIUtils.h"
	%include "../Libraries/NSGUI/GUI/GUITexCoordDescriptor.h"
	%include "../Libraries/FW3/include/FW3.h"
	%include "../Libraries/FW3/include/Shape3D.h"
#if 1
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/Defines.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IndieLib.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Window.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Render.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Timer.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Math.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity2d.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity3d.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity2dManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Entity3dManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Input.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Image.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Light.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Surface.h"

    	%include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Animation.h"
     	%include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Font.h"
     	%include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_3dMesh.h"

     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_ImageManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_SurfaceManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_AnimationManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_FontManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_3dMeshManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Camera2d.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Camera3d.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_LightManager.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/IND_Factory.h"
     %include "../Libraries/IndieLib/IndieLib/Common/LibHeaders/CIndieLib.h"
#endif
     %array_functions(D3DXFROMWINEVECTOR2, D3DXFROMWINEVECTOR2Array);

