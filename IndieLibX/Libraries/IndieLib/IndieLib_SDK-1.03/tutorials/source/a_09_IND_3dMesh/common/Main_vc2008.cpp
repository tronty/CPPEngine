//define __MESHRENDERER__ 1
//define BUILD_MESHRENDERER 1

#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	float	mAngle = 0;
	int	mSequence = 0;
	char	mText [2048]={0}; 
	int	mSpeed = 25;
	float	mDelta;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceBack;
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Entity2d mBack;
		IND_Light mLight;
		IND_Camera2d mCamera2d; 
#ifdef BUILD_MESHRENDERER		
		IND_Entity3d mBones;
		IND_Camera3d mCamera3d; 
		IND_3dMesh mMeshBones;
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

	// Loading Background
	//////////////IND_Light mLight;

	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/twist.jpg", IND_OPAQUE, IND_32)) return 0;

	// Font
	//////////////IND_Light mLight;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	//////////////IND_Light mLight;
	mI->Entity2dManager->Add		(&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- 3d Mesh loading -----
#ifdef BUILD_MESHRENDERER
	// Bones loading
	//////////////IND_Light mLight;
	if (!mI->MeshManager->Add (&mMeshBones, "/skeleton/skeleton.x", "/skeleton")) return 0;
#endif
	// ----- Set the surfaces into 2d entities -----

	// Creating 2d entity for the background
	//////////////IND_Light mLight;
	mI->Entity2dManager->Add (&mBack);						// Entity adding
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// ----- Set the mesh into 3d entities -----
#ifdef BUILD_MESHRENDERER
	// Creating 3d entity
	//////////////IND_Light mLight;
	mI->Entity3dManager->Add (&mBones);						// Entity adding
	mBones.Set3dMesh (&mMeshBones);							// Set the 3d mesh into the entity
#endif
	// ----- Changing the attributes of the 2d entities -----

	mBack.SetHotSpot (0.5f, 0.5f);
	mBack.SetPosition (400, 300, 0);
	mBack.SetScale (1.7f, 1.7f);

	// ----- Changing the attributes of the 3d entities -----
#ifdef BUILD_MESHRENDERER
	mBones.SetPosition (0, 0, 0);
	mBones.SetScale (1.5f, 1.5f, 1.5f);
#endif
	// ----- Cameras ------

	mCamera2d=IND_Camera2d (mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);
#ifdef BUILD_MESHRENDERER
	mCamera3d=IND_Camera3d (0.0f, 1.4f, -4.0f);
	mCamera3d.SetAspect ((float) mI->Window->GetWidth () / mI->Window->GetHeight());

	// ----- Light -----

	//////////////IND_Light mLight;
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

		stx_strlcpy (mText, "Use up and down arrow keys to toggle the animation\n", 2048);
		mTextSmallWhite.SetText	 (mText);

		// ----- Input -----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;
#ifdef BUILD_MESHRENDERER
		// Change animation sequence
		if (mI->Input->OnKeyPress (STX_KEY_UP))
		{
			mSequence++;
			if (mSequence == mMeshBones.GetNumSequences()) mSequence = 0;
			mBones.SetSequence (mSequence);
		}

		// Change animation sequence
		if (mI->Input->OnKeyPress (STX_KEY_DOWN))
		{
			mSequence--;
			if (mSequence < 0) mSequence = mMeshBones.GetNumSequences() - 1;
			mBones.SetSequence (mSequence);
		}
#endif
		mAngle += mDelta * mSpeed;

		// ----- Updating entities attributes  -----

		// Back
		mBack.SetAngleXYZ (0, 0, mAngle);
#ifdef BUILD_MESHRENDERER
		// Skeleton
		mBones.SetAngleXYZ (0, mAngle, 0);
#endif
		// ----- Render  -----

		mI->Render->BeginScene ();
		mI->Render->SetViewPort2d (0, 0, 800, 600);
		mI->Render->SetCamera2d (&mCamera2d);
		mI->Entity2dManager->RenderEntities2d ();
#ifdef BUILD_MESHRENDERER
		mI->Render->SetViewPort3d (0, 0, 800, 600);
		mI->Render->SetCamera3d (&mCamera3d);
		mI->LightManager->Update ();
		mI->Entity3dManager->RenderEntities3d ();
#endif
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
	glp->init("a_09_3dMesh");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

