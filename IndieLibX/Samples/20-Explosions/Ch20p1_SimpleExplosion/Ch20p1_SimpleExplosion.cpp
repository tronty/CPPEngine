/*
#############################################################################

  Ch20p1_SimpleExplosion.cpp:  demonstrates how to create a simple explosion
  using billboarded quads.
  
#############################################################################
*/
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>
#include <FW3.h>


//-----------------------------------------------------------------------------
// Name: class App
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. App 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class App: public STXGUI
{
  CTimer m_timObjectRebirth;

  // explosion animation and sprite
  CAnimSequence *m_pExploAnim;
  CSprite m_sprExplo;

protected:
  int OneTimeSceneInit();
  int InitDeviceObjects();
  int RestoreDeviceObjects();
  int InvalidateDeviceObjects();
  int DeleteDeviceObjects();
  int FinalCleanup();
  int Render();
  int FrameMove();
  int ConfirmDevice( __DWORD__ dwBehavior, D3DFORMAT Format );
  void MsgProc(  );

  void CreateExplosion(CSprite &spr,
                       float PosX, float PosY, float PosZ,
                       float RadiusX, float RadiusY, float RadiusZ,
                       float Size, float SizeDelta,
                       float Time, float TimeDelta);

  void ProcessInput();

public:
  App();
};

//-----------------------------------------------------------------------------
// Name: App()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
App::App()
{
  m_pExploAnim       = 0;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
int App::FrameMove()
{
  if (m_timObjectRebirth.GetTime() > 2.0f) m_timObjectRebirth.Stop();
  return 0;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
void render()
{
  
  // these are done here so that you can move the camera around during a freeze
  // frame, ala The Matrix
  D3DXFROMWINEMATRIX matWorld;
  D3DXFROMWINEMatrixIdentity( &matWorld );
  //IRenderer::GetRendererInstance()->SetTransform( D3DTS_WORLD, &matWorld );


	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
  {
    
    // draw explosion
    {

      if (m_timObjectRebirth.IsRunning()) {
        m_sprExplo.Render(m_Camera.GetViewMatrix());
      }
    }

     m_timObjectRebirth.IsRunning() ? "Kablammo!" : "Press B to blow up this poor object." );

    // End the scene.
    IRenderer::GetRendererInstance()->EndScene();
  }
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
int App::RestoreDeviceObjects()
{

  // create explosion animation
  m_pExploAnim = new CAnimSequence();

  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame01.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame02.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame03.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame04.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame05.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame06.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame07.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p1_Explosion_Frame08.dds", 0.03f);

  m_sprExplo.SetAnim(m_pExploAnim);
  m_sprExplo.SetSize(15.0f);

  // Set the world matrix
  D3DXFROMWINEMATRIX matWorld;
  D3DXFROMWINEMatrixIdentity( &matWorld );
  //IRenderer::GetRendererInstance()->SetTransform( D3DTS_WORLD, &matWorld );

  // Set projection matrix
  D3DXFROMWINEMATRIX matProj;
  FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
  D3DXFROMWINEMatrixPerspectiveFovLH( &matProj, D3DXFROMWINE_PI/4, fAspect, 0.1f, 100.0f );
  //IRenderer::GetRendererInstance()->SetTransform( D3DTS_PROJECTION, &matProj );

  return 0;
}

void App::CreateExplosion(CSprite &spr,
                                        float PosX, float PosY, float PosZ,
                                        float RadiusX, float RadiusY, float RadiusZ,
                                        float Size, float SizeDelta,
                                        float Time, float TimeDelta)
{
  
  spr.Pos() = D3DXFROMWINEVECTOR3(PosX, PosY, PosZ) + 
    RandomNumber(
      D3DXFROMWINEVECTOR3(-RadiusX, -RadiusY, -RadiusZ),
      D3DXFROMWINEVECTOR3(RadiusX, RadiusY, RadiusZ));

  spr.SetSize(Size + RandomNumber(-SizeDelta, SizeDelta));

  spr.Timer().BeginWithDelay(Time+RandomNumber(-TimeDelta, TimeDelta));
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
void App::MsgProc( )
{
  if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_b) &&
      !m_timObjectRebirth.IsRunning()) {
    CreateExplosion(m_sprExplo, 
      0.0f, 4.0f, 0.0f, // position
      0.0f, 0.0f, 0.0f, // radius
      15.0f, 5.0f,      // size
      0.0f, 0.0f);      // time            
    m_timObjectRebirth.Begin();
  }
}

int init(const char* aTitle)
{
	return 0;
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleExplosion");	
	IInput*    i=STX_Service::GetInputInstance();
	App app;
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

