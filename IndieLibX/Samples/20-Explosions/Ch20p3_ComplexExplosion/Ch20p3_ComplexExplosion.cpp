/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*
#############################################################################

  Ch20p3_ComplexExplosion.cpp: a program that demonstrates the fire algorithm,
  without any annoying bells and/or whistles.
  
#############################################################################
*/

// include files ////////////////////////////////////////////////////////////
#define STRICT
#include <stdio.h>
#include <math.h>
#include <D3DXFROMWINE8.h>
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "D3DHelperFuncs.h"
#include "Ch20p3_resource.h"
#include "Camera.h"
#include "InputManager.h"
#include "AnimSequence.h"
#include "Sprite.h"
#include "SkyBox.h"
#include "Ch19p1_ParticleSystem.h"
#include "Ch20p3_Shockwave.h"

const int NUMSPRITES = 5;

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
  // Font for drawing text
  CD3DFont* m_pFont;
  CD3DFont* m_pFontSmall;

  // Scene
  CUserControlledCamera m_Camera;

  // Mouse Input
  CInputManager m_InputManager;

  // object mesh
  CD3DMesh* m_pObject;
  CTimer m_timObjectRebirth;

  // explosion animation sprites, particle system, and shockwave
  CAnimSequence *m_pExploAnim;
  CSprite m_sprExplo[NUMSPRITES];
	CParticleEmitter m_ParticleEmitter;
	CShockwave m_Shockwave;

  // skybox
  CSkyBox m_SkyBox;

protected:
  int OneTimeSceneInit();
  int InitDeviceObjects();
  int RestoreDeviceObjects();
  int InvalidateDeviceObjects();
  int DeleteDeviceObjects();
  int FinalCleanup();
  int Render();
  int FrameMove();
  int ConfirmDevice( D3DCAPS8* pCaps, __DWORD__ dwBehavior, D3DFORMAT Format );
  LRESULT MsgProc( HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam );

  void CreateExplosion(CSprite &spr,
                       float PosX, float PosY, float PosZ,
                       float RadiusX, float RadiusY, float RadiusZ,
                       float Size, float SizeDelta,
                       float Time, float TimeDelta);

  void ProcessInput();

public:
  CMyD3DApplication();
};



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
  CMyD3DApplication d3dApp;

  if( FAILED( d3dApp.Create( hInst ) ) )
    return 0;

  return d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
  m_strWindowTitle    = _T("Ch20p3_ComplexExplosion");
  m_bUseDepthBuffer   = TRUE;
  m_pObject           = new CD3DMesh;
  m_pFont            = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
  m_pFontSmall       = new CD3DFont( _T("Arial"),  9, D3DFONT_BOLD );
  m_pExploAnim       = 0;
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
int CMyD3DApplication::OneTimeSceneInit()
{
  return 0;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
int CMyD3DApplication::FrameMove()
{
	if (m_fElapsedTime < 0.0001f) {
		m_fElapsedTime = 0.0001f;
	}
  CTimer::UpdateAll(m_fElapsedTime);
	m_ParticleEmitter.Update(m_fTime, m_fElapsedTime);
	m_Shockwave.Update(m_fElapsedTime);

  if (m_timObjectRebirth.GetTime() > 2.0f) m_timObjectRebirth.Stop();
  return 0;
}

void CMyD3DApplication::ProcessInput()
{
  const float fSpeed = 0.5f;
  unsigned char m_bKey[256];
  ZeroMemory( m_bKey, 256 );
  GetKeyboardState(m_bKey);
  // Process keyboard input
  if(m_bKey['D'] & 128) m_Camera.AddToVelocity(D3DXFROMWINEVECTOR3(fSpeed, 0.0f, 0.0f)); // Slide Right
  if(m_bKey['A'] & 128) m_Camera.AddToVelocity(D3DXFROMWINEVECTOR3(-fSpeed, 0.0f, 0.0f));// Slide Left
  if(m_bKey['Q'] & 128) m_Camera.AddToVelocity(D3DXFROMWINEVECTOR3(0.0f, fSpeed, 0.0f)); // Slide Up
  if(m_bKey['Z'] & 128) m_Camera.AddToVelocity(D3DXFROMWINEVECTOR3(0.0f, -fSpeed, 0.0f));// Slide Down
  if(m_bKey['W'] & 128) m_Camera.AddToVelocity(D3DXFROMWINEVECTOR3(0.0f, 0.0f, fSpeed)); // Slide Foward
  if(m_bKey['S'] & 128) m_Camera.AddToVelocity(D3DXFROMWINEVECTOR3(0.0f, 0.0f, -fSpeed));// Slide Back
  if(m_bKey['L'] & 128) m_Camera.AddToYawPitchRoll(fSpeed, 0.0f, 0.0f);  // Turn Right
  if(m_bKey['J'] & 128) m_Camera.AddToYawPitchRoll(-fSpeed, 0.0f, 0.0f); // Turn Left
  if(m_bKey['K'] & 128) m_Camera.AddToYawPitchRoll(0.0f, fSpeed, 0.0f); // Turn Down
  if(m_bKey['I'] & 128) m_Camera.AddToYawPitchRoll(0.0f, -fSpeed, 0.0f);// Turn Up

  // mouse look
  DIMOUSESTATE2 dims2;
  m_InputManager.ReadMouse(dims2);

  // play with the divisor constants to change the mouselook sensitivity.
  // I've found that these values most accurately simulate my beloved Q3A setup. :)
  m_Camera.AddToYawPitchRoll((float)dims2.lX/0.8f, (float)dims2.lY/0.8f, 0.0f);
  
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::Render()
{
  
  // these are done here so that you can move the camera around during a freeze
  // frame, ala The Matrix
  D3DXFROMWINEMATRIX matWorld;
  D3DXFROMWINEMatrixIdentity( &matWorld );
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

  ProcessInput();
  m_Camera.Update(m_fElapsedTime);

  m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
  m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
  
  // Clear the backbuffer
  m_pd3dDevice->Clear( 0L, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                       0x000000, 1.0f, 0L );
  
  // Now that our fire texture's updated, we can begin rendering the scene
  if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
  {
    // draw skybox
		m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
    m_SkyBox.Render(m_Camera.GetViewMatrix());
    
    // draw the ground
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_Camera.GetViewMatrix());
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ_DIFFUSE_TEX1 );
    
    // draw Object
    D3DXFROMWINEMatrixTranslation(&matWorld, 0, 4, 0);
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
    if (!m_timObjectRebirth.IsRunning()) m_pObject->Render( m_pd3dDevice );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);

		
		// draw explosion
    {
      // uncomment these lines for additive alpha blending
      // comment them for normal alpha blending
      // (I like additive blending)
      m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
      m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

      if (m_timObjectRebirth.IsRunning()) {
        for (int q=0; q < NUMSPRITES; q++) {
          m_sprExplo[q].Render(m_Camera.GetViewMatrix());
        }
      }

			m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
			D3DXFROMWINEMatrixTranslation(&matWorld, 0, 4, 0);
			m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			m_Shockwave.Render();
			m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );

			m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
      m_ParticleEmitter.Render();
 			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
      
      m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
      
    }
 
    char buf[256];
    _snprintf(buf, 256, "Position: (%.2f, %.2f, %.2f)", 
      (float)m_Camera.GetPosition().x, (float)m_Camera.GetPosition().y, 
      (float)m_Camera.GetPosition().z);

    m_pFontSmall->DrawText( 2,  0,  D3DCOLOR_ARGB(255,255,255,0), buf );
    m_pFontSmall->DrawText( 2,  20, D3DCOLOR_ARGB(255,255,0,0), 
      m_timObjectRebirth.IsRunning() ? "Kablammo!" : "Press B to blow up this poor object." );

    // End the scene.
    m_pd3dDevice->EndScene();
  }
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
int CMyD3DApplication::InitDeviceObjects()
{
  m_pFont->InitDeviceObjects( m_pd3dDevice );
  m_pFontSmall->InitDeviceObjects( m_pd3dDevice );
	m_SkyBox.SetSize(100.0f);
  if( FAILED( m_pObject->Create(m_pd3dDevice, _T("Ch20p3_Object.x") ) ) )
    return E_FAIL;

  return 0;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
int CMyD3DApplication::RestoreDeviceObjects()
{
  m_InputManager.CreateDevices(m_hWnd, false, true);

  // create explosion animation
  m_pExploAnim = new CAnimSequence(m_pd3dDevice);

  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame01.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame02.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame03.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame04.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame05.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame06.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame07.dds", 0.03f);
  m_pExploAnim->AddFrame("Ch20p3_Explosion_Frame08.dds", 0.03f);

  for (int q=0; q < NUMSPRITES; q++) { 
    m_sprExplo[q].SetAnim(m_pExploAnim);
    m_sprExplo[q].SetSize(15.0f);
  }
  
	m_SkyBox.RestoreDeviceObjects(m_pd3dDevice,
    "Ch20p3_SkyBox_Top.bmp",
    "Ch20p3_SkyBox_Bottom.bmp",
    "Ch20p3_SkyBox_Front.bmp",
    "Ch20p3_SkyBox_Back.bmp",
    "Ch20p3_SkyBox_Left.bmp",
    "Ch20p3_SkyBox_Right.bmp"
    );

	// initialize particle system
	std::string strScript;
	LoadFileIntoString("Ch20p3_ParticleScript.txt", strScript);
	m_ParticleEmitter.RestoreDeviceObjects(m_pd3dDevice);
	m_ParticleEmitter.Compile(strScript.c_str());

	// vary shockwave thickness, speed, and lifetime to taste
	m_Shockwave.RestoreDeviceObjects(m_pd3dDevice, "Ch20p3_Shockwave.png", 10.0f, 4.0f, 16, 6.0f, 1.0f);
  m_Shockwave.Pos() = D3DXFROMWINEVECTOR3(0.0f, 4.0f, 0.0f);
	m_pObject->RestoreDeviceObjects( m_pd3dDevice );

  
  m_pFont->RestoreDeviceObjects();
  m_pFontSmall->RestoreDeviceObjects();
  m_Camera.SetPosition(D3DXFROMWINEVECTOR3(0.0f, 7.0f, -20.0f));
	
  // Set the world matrix
  D3DXFROMWINEMATRIX matWorld;
  D3DXFROMWINEMatrixIdentity( &matWorld );
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

  // Set projection matrix
  D3DXFROMWINEMATRIX matProj;
  FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
  D3DXFROMWINEMatrixPerspectiveFovLH( &matProj, D3DXFROMWINE_PI/4, fAspect, 0.1f, 100.0f );
  m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
  m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
  m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

  m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
  m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
  m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
  m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
  
  // create a light for our Object
  D3DLIGHT8 light;
  light.Ambient.a = light.Ambient.g = light.Ambient.b = light.Ambient.r = 1.0;
  light.Diffuse.a = light.Diffuse.g = light.Diffuse.b = light.Diffuse.r = 1.0;
  light.Position = D3DXFROMWINEVECTOR3(0.0,5.0,2.0);
  light.Type = D3DLIGHT_SPOT;

  m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
  m_pd3dDevice->SetLight(0, &light);
  m_pd3dDevice->LightEnable(0, true);
  return 0;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
int CMyD3DApplication::InvalidateDeviceObjects()
{
  delete m_pExploAnim;
	m_ParticleEmitter.InvalidateDeviceObjects();
	m_Shockwave.InvalidateDeviceObjects();
  m_InputManager.DestroyDevices();
  m_pObject->InvalidateDeviceObjects();
  m_pFont->InvalidateDeviceObjects();
  m_pFontSmall->InvalidateDeviceObjects();
  m_SkyBox.InvalidateDeviceObjects();
  return 0;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
int CMyD3DApplication::DeleteDeviceObjects()
{
  m_pFont->DeleteDeviceObjects();
  m_pFontSmall->DeleteDeviceObjects();
  m_pObject->Destroy();
    
  return 0;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
int CMyD3DApplication::FinalCleanup()
{
  SAFE_DELETE( m_pFont );
  SAFE_DELETE( m_pFontSmall );
  return 0;
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
int CMyD3DApplication::ConfirmDevice( D3DCAPS8* pCaps, __DWORD__ dwBehavior,
                                          D3DFORMAT Format )
{
  return 0;
}

void CMyD3DApplication::CreateExplosion(CSprite &spr,
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
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, unsigned int uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
  if (uMsg == WM_KEYUP && (wParam == 'b' || wParam == 'B') &&
    !m_timObjectRebirth.IsRunning()) {

    // one big explosion in the middle
    CreateExplosion(m_sprExplo[0], 
      0.0f, 4.0f, 0.0f, // position
      0.0f, 0.0f, 0.0f, // radius
      15.0f, 5.0f,      // size
      0.0f, 0.0f);      // time

    // several smaller explosions around it
    for (int q=1; q < NUMSPRITES; q++) {
    CreateExplosion(m_sprExplo[q], 
      0.0f, 4.0f, 0.0f, // position
      1.0f, 1.0f, 1.0f, // radius
      10.0f, 5.0f,      // size
      0.0f, 0.25f);     // time
    }
    m_timObjectRebirth.Begin();

		// start particle system
		m_ParticleEmitter.SetPos(D3DXFROMWINEVECTOR3(0.0f, 4.0f, 0.0f));
		m_ParticleEmitter.Stop();
		m_ParticleEmitter.Start();
		m_Shockwave.Stop();
		m_Shockwave.Begin();

  }
  // Pass remaining messages to default handler
  return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}
