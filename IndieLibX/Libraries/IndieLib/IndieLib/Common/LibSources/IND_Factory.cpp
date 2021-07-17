/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include "IND_Factory.h"	

IWindow* 	IND_Factory::GetWindowInstance(tWindow a)	{return STX_Service::GetWindowInstance(a);}
IInput* 	IND_Factory::GetInputInstance(tInput a)		{return STX_Service::GetInputInstance(a);}
IAudio * 	IND_Factory::GetAudioInstance(tAudio a)		{return STX_Service::GetAudioInstance(a);}
STX_TIMER* 	IND_Factory::GetTimerInstance()			{return STX_Service::GetTimerInstance();}

#ifdef BUILD_MESHRENDERER
IND_3dMesh*	IND_Factory::Get3dMesh(){return new IND_3dMesh();}
IND_Entity3d*	IND_Factory::GetEntity3d(){return new IND_Entity3d();}
IND_Camera3d*	IND_Factory::GetCamera3d(){return new IND_Camera3d();}
IND_Camera3d*	IND_Factory::GetCamera3d(float pX, float pY, float pZ){return new IND_Camera3d(pX, pY, pZ);}
#endif
IND_Animation*	IND_Factory::GetAnimation(){return new IND_Animation();}
IND_Camera2d*	IND_Factory::GetCamera2d(){return new IND_Camera2d();}
IND_Camera2d*	IND_Factory::GetCamera2d(int pX, int pY){return new IND_Camera2d(pX, pY);}
IND_Entity2d*	IND_Factory::GetEntity2d(){return new IND_Entity2d();}
IND_Font*	IND_Factory::GetFont(){return new IND_Font();}
IND_Image*	IND_Factory::GetImage(){return new IND_Image();}
IND_Light*	IND_Factory::GetLight(){return new IND_Light();}
IND_Math*	IND_Factory::GetMath(){return new IND_Math();}
IND_Matrix*	IND_Factory::GetMatrix(){return new IND_Matrix();}
IND_Point*	IND_Factory::GetPoint(int n){return new IND_Point[n]();}
IND_Surface*	IND_Factory::GetSurface(){return new IND_Surface();}
IND_Timer*	IND_Factory::GetTimer(){return new IND_Timer();}
IND_Window*	IND_Factory::GetWindow(){return new IND_Window();}

