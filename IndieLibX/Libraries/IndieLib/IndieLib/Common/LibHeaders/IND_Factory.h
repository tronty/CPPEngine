/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __IND_FACTORY__
#define __IND_FACTORY__

#include <CIndieLib.h>

struct IND_3dMesh;
struct IND_Camera3d;
struct IND_Entity3d;
struct IND_Factory
{
static IWindow* GetWindowInstance(tWindow a=eSDL_Window);
static IInput* GetInputInstance(tInput a=eSDL_Input);
static IAudio * GetAudioInstance(tAudio a=eDefault_Audio);
static STX_TIMER* GetTimerInstance();

static IND_3dMesh*	Get3dMesh();
static IND_Animation*	GetAnimation();
static IND_Camera2d*	GetCamera2d();
static IND_Camera2d*	GetCamera2d(int pX, int pY);
static IND_Camera3d*	GetCamera3d();
static IND_Camera3d*	GetCamera3d(float pX, float pY, float pZ);
static IND_Entity2d*	GetEntity2d();
static IND_Entity3d*	GetEntity3d();
static IND_Font*	GetFont();
static IND_Image*	GetImage();
static IND_Light*	GetLight();
static IND_Math*	GetMath();
static IND_Matrix*	GetMatrix();
static IND_Point*	GetPoint(int n);
static IND_Surface*	GetSurface();
static IND_Timer*	GetTimer();
static IND_Window*	GetWindow();
};

#endif

