/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "Particles.h"

// Global projection transformation
D3DXFROMWINEMATRIX g_matProj;

// Helicopter smoke, tree, and people particle emitter objects
cParticleEmitter g_ChopperEmitter;
cParticleEmitter g_TreeEmitter;
cParticleEmitter g_PeopleEmitter;
cParticleEmitter* g_FireEmitter=0;
cParticleEmitter* g_ExplosionEmitter=0;

// Helicopter, rotor, shadow, and backdrop mesh objects
MeshRenderer2 g_ChopperMesh;
MeshRenderer2 g_RotorMesh;
MeshRenderer2 g_ShadowMesh;
MeshRenderer2 g_BackdropMesh;

// Helicopter's position and Y-axis rotation
D3DXFROMWINEVECTOR3 g_vecChopper = D3DXFROMWINEVECTOR3(0.0f, 50.0f, 0.0f);
float       g_rotChopper = 0.0f;

// Window class and caption text
char g_szClass[]   = "VSParticlesClass";
char g_szCaption[] = "Vertex Shader Particles Demo by Jim Adams";

TextureID dirtTexture=-1;
ShaderID dirtshd=-1;
VertexFormatID dirtvf=-1;

D3DXFROMWINEVECTOR3 vO(0.0f, 0.0f, 0.0f);

bool DoInit();
void DoShutdown();
void DoFrame();

int init(const char* aTitle)
{
	DoInit();
	return 0;
}

void render()
{
	DoFrame();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("ParticleVS");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	//
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		// Render a single frame
      		render();
	}
	//while(1){}
  	// Call shutdown
  	DoShutdown();
	return 0;
}

bool DoInit()
{
	dirtshd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD1.HLSL.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	dirtvf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), dirtshd);
	dirtTexture=IRenderer::GetRendererInstance()->addImageLibTexture(
				//"/AdvAniCh12/dirtTexture.jpg",
				//"/Orcs/GRYDIRT1.jpg",
				"/NatureScene/Terrain/dirt.png",
				//"/textures/NVIDIA_Corporation/graydirt.png",
				//"/NatureScene/Terrain/grasslayer.png",
				//"/textures/NVIDIA_Corporation/grass.png",
			false,
			IRenderer::GetRendererInstance()->Getlinear()
			//, 0xFFFF
			);
  // Get the projection transformation for use later
  //g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &g_matProj);
  D3DXFROMWINEMatrixIdentity(&g_matProj);

    D3DXFROMWINEMatrixPerspectiveFovLH( &g_matProj, D3DXFROMWINEToRadian( 45.0f ),
                                //640.0f / 480.0f, 0.1f, 100.0f
								IRenderer::GetRendererInstance()->GetAspect(), 0.001f, 20000.0f
								);

  #if 1
  // Load the helicopter, rotor, and shadow meshes
  g_ChopperMesh.Load("/AdvAniCh12/chopper.X", eSimpleShader, false, true);
  //g_ChopperMesh.Load("/AdvAniCh12/heli.x", eSimpleShader, false, true);
  g_RotorMesh.Load("/AdvAniCh12/Rotor.X", eSimpleShader, false, true);
  g_ShadowMesh.Load("/AdvAniCh12/Shadow.X", eSimpleShader, false, true);

  // Load the backdrop mesh
  g_BackdropMesh.Load("/AdvAniCh12/backdrop.x", eSimpleShader, true, true);
  #endif

  // Create the particle emitters
  g_ChopperEmitter.Create(&vO, EMITTER_CLOUD);
  g_TreeEmitter.Create(&vO, EMITTER_TREE);
  g_PeopleEmitter.Create(&vO, EMITTER_PEOPLE);
  if(g_FireEmitter) g_FireEmitter->Create(&vO, EMITTER_FIRE);
  if(g_ExplosionEmitter) g_ExplosionEmitter->Create(&vO, EMITTER_EXPLOSION);
	D3DXFROMWINEVECTOR4 Color(1.0f, 1.0f, 1.0f, 1.0f);

  // Populate the ground with trees and people
  __DWORD__ Type = PARTICLE_TREE1;
  for(__DWORD__ i=0;i<50;i++) {
    float rot  = (float)(stx_rand() % 628) / 100.0f;
    float dist = (float)(stx_rand() % 400);
    {D3DXFROMWINEVECTOR3 v((float)cos(rot)*dist,25.0f,(float)sin(rot)*dist);

    g_TreeEmitter.Add(Type++,
                      &v,
                      50.0f, Color, 0, &vO);}


    if(Type > PARTICLE_TREE3)
      Type = PARTICLE_TREE1;

    rot  = (float)(stx_rand() % 628) / 100.0f;
    dist = (float)(stx_rand() % 400);
    {D3DXFROMWINEVECTOR3 v((float)cos(rot)*dist,7.5f,(float)sin(rot)*dist);
    g_PeopleEmitter.Add(PARTICLE_PEOPLE1,
                      &v,
                      15.0f, Color, 0, &vO);}
  }
#if 0
  // Setup a light
  D3DLIGHT9 Light;
  ZeroMemory(&Light, sizeof(D3DLIGHT9));
  Light.Type = D3DLIGHT_DIRECTIONAL;
  Light.Diffuse.r = Light.Diffuse.g = Light.Diffuse.b = Light.Diffuse.a = 1.0f;
  Light.Direction = D3DXFROMWINEVECTOR3(0.0f, -1.0f, 0.0f);
  g_pD3DDevice->SetLight(0, &Light);
  g_pD3DDevice->LightEnable(0, true);

  // Play a chopper sound
  PlaySound("..\\Data\\Chopper.wav", 0, SND_LOOP | SND_ASYNC);
#endif
  STX_Service::GetAudioInstance()->Load("/AdvAniCh12/chopper.wav", "Chopper");
  return true;
}

void DoShutdown()
{
  // Stop the sound
  //PlaySound(0, 0, 0);

  // Release particle emitters
  //g_ChopperEmitter.Free();
  //g_TreeEmitter.Free();
  //g_PeopleEmitter.Free();
}

void DoFrame()
{
	STX_Service::GetAudioInstance()->Play("Chopper");
  static __DWORD__ LastTime = timeGetTime();
  __DWORD__ Time = timeGetTime();
  __DWORD__ Elapsed;
  D3DXFROMWINEMATRIX matWorld;
  static __DWORD__ SmokeTimer = 0;

  // Get elapsed time and update last frame's time
  Elapsed = Time - LastTime;
  LastTime = Time;

  // Rotate and move the helicopter
  g_rotChopper = (float)Time / 5000.0f;
  g_vecChopper.x += (float)cos(g_rotChopper) * ((float)Elapsed / 30.0f);
  g_vecChopper.z -= (float)sin(g_rotChopper) * ((float)Elapsed / 30.0f);

  // Call special function that creates smoke under helicopter
  g_ChopperEmitter.HandleSmoke(&g_vecChopper, Elapsed);
  D3DXFROMWINEVECTOR3 _v1(0.0f, 0.0f, 0.0f);
  if(g_FireEmitter) g_FireEmitter->HandleFire(&_v1, Elapsed);
  D3DXFROMWINEVECTOR3 _v2(25.0f, 0.0f, 0.0f);
  if(g_ExplosionEmitter) g_ExplosionEmitter->HandleExplosion(&_v2, Elapsed);

  // Call special function that makes people duck when chopper is too close
  g_PeopleEmitter.HandleDucking(&g_vecChopper);

  // Update the helicopter's particle emitter
  g_ChopperEmitter.Process(Elapsed);
  if(g_FireEmitter) g_FireEmitter->Process(Elapsed);
  if(g_ExplosionEmitter) g_ExplosionEmitter->Process(Elapsed);

  // Calculate a view transformation matrix
  D3DXFROMWINEMATRIX matView;{
  D3DXFROMWINEVECTOR3 v1(g_vecChopper.x, 80.0f, g_vecChopper.z - 150.0f);
  D3DXFROMWINEVECTOR3 v2(g_vecChopper.x, 0.0f, g_vecChopper.z);
  D3DXFROMWINEVECTOR3 v3(0.0f, 1.0f, 0.0f);
  D3DXFROMWINEMatrixLookAtLH(&matView, &v1, &v2, &v3);}
  //g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

  // Clear the device and start drawing the scene
  IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(
	0.35f, 0.53f, 0.7, 1.0f
	//0.0f, 0.0f, 64.0f/255.0f, 1.0f
	));
  IRenderer::GetRendererInstance()->BeginScene();{

	IRenderer::GetRendererInstance()->setShader(dirtshd);
	IRenderer::GetRendererInstance()->setTexture("Sampler", dirtTexture);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", color);
	float dim=1500.0f;
	D3DXFROMWINEMATRIX T;
	D3DXFROMWINEMatrixTranslation(&T, 0.0f, 0.0f, 0.0f);
	D3DXFROMWINEMATRIX W=T*matView*g_matProj;
	D3DXFROMWINEMatrixTranspose(&W, &W);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	IRenderer::GetRendererInstance()->setVertexFormat(dirtvf);
	//		   x	     y     z         tx    ty
	float v[20] =  {  1.0f*dim, 0.0f,-1.0f*dim, 1.0f, 1.0f,
			  1.0f*dim, 0.0f, 1.0f*dim, 1.0f, 0.0f,
			 -1.0f*dim, 0.0f,-1.0f*dim, 0.0f, 1.0f,
			 -1.0f*dim, 0.0f, 1.0f*dim, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 5*sizeof(float));

    // Draw the backdrop mesh at origin
    D3DXFROMWINEMatrixIdentity(&matWorld);
    {g_BackdropMesh.render(&matWorld);}

    // Draw the helicopter's shadow
    D3DXFROMWINEMatrixTranslation(&matWorld, g_vecChopper.x, 0.5f, g_vecChopper.z);
    {g_ShadowMesh.render(&matWorld);}

    // Draw the helicopter
    D3DXFROMWINEMatrixRotationYawPitchRoll(&matWorld, g_rotChopper, 0.0f, -0.0872222f);
    matWorld._41 = g_vecChopper.x;  // Set position manually in matrix
    matWorld._42 = g_vecChopper.y;
    matWorld._43 = g_vecChopper.z;
    //{g_ChopperMesh.render(&matWorld);}
		D3DXFROMWINEMATRIX S;
		float s=10000.0f;
		D3DXFROMWINEMatrixScaling(&S, s, s, s);
		{g_ChopperMesh.render(&S);}

    // Draw the rotor (rotate and tilt it forward a bit and position above helicopter)
    D3DXFROMWINEMATRIX matRotY, matRotZ, matRotYY;
    D3DXFROMWINEMatrixRotationY(&matRotY, (float)Time / 10.0f);  // Turn rotor
    D3DXFROMWINEMatrixRotationZ(&matRotZ, -0.174444f);           // Angle forward
    D3DXFROMWINEMatrixRotationY(&matRotYY, g_rotChopper);        // Orient to chopper
    D3DXFROMWINEMatrixTranslation(&matWorld, g_vecChopper.x, g_vecChopper.y, g_vecChopper.z);
    {g_RotorMesh.render(&matWorld);}

    /////////////////////////////////////////////////////////////////////////
    //
    // Draw all particles last
    //
    /////////////////////////////////////////////////////////////////////////

    #if 0
    // Enable alpha testing for transparent blits
    g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    #endif

    #if 0
    D3DXFROMWINEMatrixIdentity(&g_matProj);
    D3DXFROMWINEMatrixIdentity(&matView);
    #endif

    #if 0
    // Enable alpha blending for the smoke particles
    g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    #endif

    // Draw the smoke particles
    g_ChopperEmitter.Render(1);
    if(g_FireEmitter) g_FireEmitter->Render(1);
    if(g_ExplosionEmitter) g_ExplosionEmitter->Render(1);

    #if 0
    // Disable alpha testing and alpha blending
    g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    #endif

    // Draw the tree and people particles
    g_PeopleEmitter.Render(1);
    g_TreeEmitter.Render(1);

	//STX_DrawTexture(dirt);

    STX_Service::GetAudioInstance()->Update();
    // End the scene
    IRenderer::GetRendererInstance()->EndScene();
  }

  // Present the scene to the user
  IRenderer::GetRendererInstance()->Present( );
	//stx_exit(0);
}
