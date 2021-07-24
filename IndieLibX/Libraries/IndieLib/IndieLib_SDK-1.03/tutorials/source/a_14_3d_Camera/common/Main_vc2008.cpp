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
	float	mYaw = 0;
	char	mText [2048]={0}; 
	float	mSpeedRotation = 50;
	float	mDelta;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Light mLight;
		IND_Camera2d mCamera2d; 
#ifdef BUILD_MESHRENDERER		
		IND_Camera3d mCamera3d;
		IND_3dMesh mMeshDino;
		IND_Entity3d mDino;
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
	// Bones loading
	//////////IND_Light mLight;
	if (!mI->MeshManager->Add (&mMeshDino
//, "/skeleton/skeleton.x", "/skeleton"
, "/trex_dx/dino_videogame.x", "/trex dx"
)) return 0;
#endif
	// Font
	//////////IND_Light mLight;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	//////////IND_Light mLight;
	mI->Entity2dManager->Add		(&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Set the mesh into 3d entity -----
#ifdef BUILD_MESHRENDERER
	// Creating 3d entity
	//////////IND_Light mLight;
	mI->Entity3dManager->Add (&mDino);						// Entity adding
	mDino.Set3dMesh (&mMeshDino);							// Set the 3d mesh into the entity
#endif
	// ----- Cameras ------

	IND_Camera2d mCamera2d (mI->Window->GetWidth () / 2, mI->Window->GetHeight () / 2);
#ifdef BUILD_MESHRENDERER
	IND_Camera3d mCamera3d (0.0f, 0.0f, -2.0f);
	mCamera3d.SetAspect ((float) mI->Window->GetWidth () / mI->Window->GetHeight());

	// ----- Light -----

	//////////IND_Light mLight;
	mI->LightManager->Add (&mLight, IND_DIRECTIONAL_LIGHT);
	mLight.SetPosition (0, 0, 0);
	mLight.SetColor (1.0f, 1.0f, 1.0f, 1.0f);
	mLight.SetDirection (0.0f, -0.3f, 0.5f);
	mLight.SetRange (1000.0f);
#endif
	// ----- Main Loop -----
	STX_CATCH;
	return 0;



	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Text -----

		stx_strlcpy (mText, "Use arrow keys to move\n", 2048);
		stx_strlcat (mText, "Use Z / X keys to strafe\n", 2048);
		mTextSmallWhite.SetText	 (mText);

		// ----- Input -----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;
#ifdef BUILD_MESHRENDERER
		if (mI->Input->IsKeyPressed (STX_KEY_LEFT))	mYaw -= mSpeedRotation * mDelta;
		if (mI->Input->IsKeyPressed (STX_KEY_RIGHT))	mYaw += mSpeedRotation * mDelta;

		if (mI->Input->IsKeyPressed (STX_KEY_UP))	mCamera3d.MoveWalk (0.01f);
		if (mI->Input->IsKeyPressed (STX_KEY_DOWN))	mCamera3d.MoveWalk (-0.01f);
		if (mI->Input->IsKeyPressed (STX_KEY_z))		mCamera3d.MoveStrafe (-0.01f);
		if (mI->Input->IsKeyPressed (STX_KEY_x))		mCamera3d.MoveStrafe (0.01f);

		// ----- Updating entities attributes  -----

		mCamera3d.SetYaw (mYaw);
#endif
		// ----- Render  -----

		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->BeginScene ();
#ifdef BUILD_MESHRENDERER
		mI->Render->SetViewPort3d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());
		mI->Render->SetCamera3d (&mCamera3d);

		mI->LightManager->Update ();

		mI->Entity3dManager->RenderEntities3d ();
#endif
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());
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
	glp->init("a_14_3d_Camera");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

