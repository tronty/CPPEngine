#define __MESHRENDERER__ 1
#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	float	mAngle = 0;
	char	mText [2048]={0}; 
	int	mSpeed = 25;
	float	mDelta;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Light mLight0;
		IND_Light mLight1;
		IND_Light mLight2;
		IND_Light mLight3;
		IND_Camera2d mCamera2d; 
		#ifdef BUILD_MESHRENDERER		
		IND_3dMesh mMeshDino;
		IND_Entity3d mDino;
		IND_Camera3d mCamera3d;
		#endif
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();
	// ----- IndieLib intialization -----

#if 0
	//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->Init (aTitle)) return 0;
#else
	
//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->
		Init (aTitle
		
		)) return -1;
#endif
	mText [0] = 0;
	// ----- Surface loading -----
#ifdef BUILD_MESHRENDERER
	// 3d Dino loading
	////////////////IND_Light mLight3;
	if (!mI->MeshManager->Add (&mMeshDino
//, "/skeleton/skeleton.x", "/skeleton"
, "/trex_dx/dino_videogame.x", "/trex dx"
)) return 0;
#endif
	// Font
	////////////////IND_Light mLight3;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	////////////////IND_Light mLight3;
	mI->Entity2dManager->Add		(&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Set the mesh into 3d entity -----
#ifdef BUILD_MESHRENDERER
	// Creating 3d entity
	////////////////IND_Light mLight3;
	mI->Entity3dManager->Add (&mDino);						// Entity adding
	mDino.Set3dMesh (&mMeshDino);							// Set the 3d mesh into the entity
#endif
	// ----- Cameras ------

	mCamera2d=IND_Camera2d(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);
#ifdef BUILD_MESHRENDERER
	mCamera3d=IND_Camera3d(0.0f, 0.0f, -2.0f);
	mCamera3d.SetAspect ((float) mI->Window->GetWidth () / mI->Window->GetHeight());
	// ----- Light -----

	////////////////IND_Light mLight3;
	mI->LightManager->Add (&mLight0, IND_AMBIENT_LIGHT);
	mLight0.SetColor (1.0f, 1.0f, 1.0f, 1.0f);

	// Light 1 (Direction light)
	////////////////IND_Light mLight3;
	mI->LightManager->Add (&mLight1, IND_DIRECTIONAL_LIGHT);
	mLight1.SetColor (1.0f, 1.0f, 1.0f, 1.0f);
	mLight1.SetDirection (0.0f, -0.3f, 0.5f);
	mLight1.SetRange (1000.0f);

	// Light 2 (Point light)
	////////////////IND_Light mLight3;
	mI->LightManager->Add (&mLight2, IND_POINT_LIGHT);
	mLight2.SetPosition (3, 3, 3);
	mLight2.SetColor (0.4f, 1.0f, 0.4f, 1.0f);
	mLight2.SetRange (200);
	mLight2.SetAttenuation (0.5f);

	// Light 3 (Spot light)
	////////////////IND_Light mLight3;
	mI->LightManager->Add (&mLight3, IND_SPOT_LIGHT);
	mLight3.SetPosition (5, 5, 5);
	mLight3.SetColor (1.0f, 1.0f, 1.0f, 1.0f);
	mLight3.SetDirection (0.0f, -0.3f, 0.5f);
	mLight3.SetRange (1000);
	mLight3.SetAttenuation (0.2f);
	mLight3.SetFalloff (1.0f);
	mLight3.SetPhi (8.0f);
	mLight3.SetTheta (7);

	// ----- Main Loop -----

	mI->LightManager->Disable(&mLight2);
	mI->LightManager->Disable(&mLight3);
#endif
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Text -----

		stx_strlcpy (mText, "Press 1, 2 or 3 in order to toggle between different lights\n", 2048);
		mTextSmallWhite.SetText	 (mText);
#ifdef BUILD_MESHRENDERER
		// ----- Input -----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;

		// Activate only light 1
		if (mI->Input->IsKeyPressed (STX_KEY_1))
		{
			mI->LightManager->Disable(&mLight2);
			mI->LightManager->Disable(&mLight3);
			mI->LightManager->Enable(&mLight1);
		}

		// Activate only light 2
		if (mI->Input->IsKeyPressed (STX_KEY_2))
		{
			mI->LightManager->Disable(&mLight1);
			mI->LightManager->Disable(&mLight3);
			mI->LightManager->Enable(&mLight2);
		}

		// Activate only light 3
		if (mI->Input->IsKeyPressed (STX_KEY_3))
		{
			mI->LightManager->Disable(&mLight1);
			mI->LightManager->Disable(&mLight2);
			mI->LightManager->Enable(&mLight3);
		}

		// ----- Updating entities attributes  -----

		mAngle += mDelta * mSpeed;
		mDino.SetAngleXYZ (0, mAngle, 0);
#endif
		// ----- Render  -----

		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->BeginScene ();
#ifdef BUILD_MESHRENDERER
		mI->Render->SetViewPort3d (0, 0, mI->Window->GetWidth (), mI->Window->GetHeight());
		mI->Render->SetCamera3d (&mCamera3d);

		mI->LightManager->Update ();

		mI->Entity3dManager->RenderEntities3d ();
#endif
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth (), mI->Window->GetHeight ());
		mI->Render->SetCamera2d (&mCamera2d);
		mI->Entity2dManager->RenderEntities2d ();

		mI->Render->EndScene ();
		STX_CATCH;
	}

	// ----- Free -----

	};
GameLogic gl;
IGameLogic* glp=&gl;
int init(const char* aTitle){return glp->init(aTitle);}
void render(){return glp->render();}
int IndieLib()
{
	glp->init("a_10_IND_Light");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

