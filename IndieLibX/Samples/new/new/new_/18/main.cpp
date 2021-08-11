/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include "CRenderer.hpp"

CRenderer g_renderer;
bool bLeftButtonDown    = false;
int  iOldMouseXPosition = 0;
int  iOldMouseYPosition = 0;
int  iMouseXPosition    = 0;
int  iMouseYPosition    = 0;
int  wireframe = 0;
float rRotationX        = 0;
float rRotationY        = 0;
float CamX  = +200;
float CamY  = +650;
float CamZ  = 1.5;
float Weather = 0;

bool bUseDisplayFragmentProgram = true;
int iFrameCount = 0;

D3DXFROMWINEMATRIX                  g_ProjMatrix;
D3DXFROMWINEMATRIX                  g_ViewMatrix;

void Display()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMatrixIdentity(&g_ViewMatrix);
  
  glRotatef(+90.0, 1.0, 0.0, 0);    

  glScalef(1,1,-1);

  glRotatef(rRotationY, 1, 0, 0);
  glRotatef(rRotationX, 0, 0, 1);

  glTranslatef();

		D3DXFROMWINEMatrixScaling(&Sm, 1,1,-1);
		D3DXFROMWINEMatrixRotationYawPitchRoll(&Rm, R.x, R.y, R.z);
  D3DXFROMWINEMatrixTranslation(&Tm, -CamX, -CamY, -CamZ); 
		

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glColor3f(1.0f, 1.0f, 1.0f);

  // Render reflection to texture
  struct {
     int x, y, width, height;
  } GLViewPort;

  glGetIntegerv(GL_VIEWPORT, (int*)&GLViewPort.x);


  static int InitReflectionTex = 0;
  int ReflTexSize = 512;

  glViewport(0, 0, ReflTexSize, ReflTexSize);

  

  glPushMatrix(); 
    glScalef(1,1, -1);
    g_renderer.Render(1);  
  glPopMatrix();
  
    
  glBindTexture(GL_TEXTURE_2D, g_renderer.GetReflID());  

  if (!InitReflectionTex) {
      InitReflectionTex = 1;
      glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 0, 0, ReflTexSize, ReflTexSize, 0);
    }
   
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, ReflTexSize, ReflTexSize);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(GLViewPort.x, GLViewPort.y, GLViewPort.width, GLViewPort.height);
    
    glPushMatrix(); 
      g_renderer.Render(0);

    glPopMatrix();

	IRenderer::GetRendererInstance()->EndScene();

	void render()
	{
		drawFrame();
		IRenderer::GetRendererInstance()->Present();
	}	
	int init(const char* aTitle)//800x600
	{
        float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
        D3DXFROMWINEMatrixPerspectiveFovLH(&g_ProjMatrix, D3DXFROMWINE_PI / 3.0f, fAspectRatio, 0.5f, 20000.0f);
		setup();
		resetCamera();
		load();
		return 0;
	}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("18");	
	IInput*    i=STX_Service::GetInputInstance();
	App app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

