/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _FOREST_SDL_FRAMEWORK_H_
#define _FOREST_SDL_FRAMEWORK_H_

#include <Framework3/IRenderer.h>
#include "renderer/renderer.h"

/// \addtogroup Forest
/// @{

class Gamelet;

class SDL_FrameWork
{
private:
  Renderer renderer;
  Gamelet* gamelet;

  bool running;
  
  void processEvents();
  void updateTree();
  void drawFrame();
  
public:
  SDL_FrameWork(Gamelet* gamelet, const char* atitle="OTLForest");
  ~SDL_FrameWork();
  
  int run();

  static void quit();
};

/// }@

#endif // _FOREST_SDL_FRAMEWORK_H_

