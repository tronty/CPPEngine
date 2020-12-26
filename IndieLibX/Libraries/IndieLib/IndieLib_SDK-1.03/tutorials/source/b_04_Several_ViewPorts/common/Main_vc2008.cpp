#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
float	mZoom = 1.0f;
	float	mCameraAngle = 0;
	float	mBugAngle = 0;
	char	mText [2048]; 
	float	mSpeedRotation = 50;
	float	mDelta;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceBack;
		IND_Surface mSurfaceBeetle;
		IND_Surface mSurfaceOctopus;
		IND_Surface mSurfaceBug;
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Entity2d mTerrain;
		IND_Entity2d mBeetle;
		IND_Entity2d mOctopus;
		IND_Entity2d mBug;
		IND_Camera2d mCamera1;
		IND_Camera2d mCamera2;
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
	// ----- Surface loading -----

	// Loading tile for the terrain
	////////////////////IND_Entity2d mBug;
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/twist.jpg", IND_OPAQUE, IND_32)) return 0;

	// Loading beetle
	////////////////////IND_Entity2d mBug;
	if (!mI->SurfaceManager->Add (&mSurfaceBeetle, "/beetleship.png", IND_ALPHA, IND_32)) return 0;

	// Loading octopus
	////////////////////IND_Entity2d mBug;
	if (!mI->SurfaceManager->Add (&mSurfaceOctopus, "/octopus.png", IND_ALPHA, IND_32)) return 0;

	// Loading bug
	////////////////////IND_Entity2d mBug;
	if (!mI->SurfaceManager->Add (&mSurfaceBug, "/Enemy_Bug.png", IND_ALPHA, IND_32)) return 0;

	// Font
	////////////////////IND_Entity2d mBug;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	////////////////////IND_Entity2d mBug;
	mI->Entity2dManager->Add		(&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Entities -----

	// Terrain
	////////////////////IND_Entity2d mBug;
	mI->Entity2dManager->Add (&mTerrain);
	mTerrain.SetSurface (&mSurfaceBack);

	// Beetle
	////////////////////IND_Entity2d mBug;
	mI->Entity2dManager->Add (&mBeetle);
	mBeetle.SetSurface (&mSurfaceBeetle);
	mBeetle.SetHotSpot (0.5f, 0.5f);
	mBeetle.SetPosition (150, 150, 2);

	// Octopus
	////////////////////IND_Entity2d mBug;
	mI->Entity2dManager->Add (&mOctopus);
	mOctopus.SetSurface (&mSurfaceOctopus);
	mOctopus.SetHotSpot (0.5f, 0.5f);
	mOctopus.SetPosition (450, 150, 2);

	// But
	////////////////////IND_Entity2d mBug;
	mI->Entity2dManager->Add (&mBug);
	mBug.SetSurface (&mSurfaceBug);
	mBug.SetHotSpot (0.5f, 0.5f);
	mBug.SetPosition (700, 150, 2);

	// ----- Camera ------

	// Camera for the viewport 1
	mCamera1=IND_Camera2d		(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 4);

	// Camera for the viewport 2
	mCamera2=IND_Camera2d		(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 4);

	// ----- Main Loop -----
	mText [0] = 0;
	STX_CATCH;
	return 0;
	

	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Text -----

		stx_strlcpy (mText, "Use mouse wheel for zooming in/out\n", 2048);
		stx_strlcat (mText, "Use mouse buttons for rotating the camera", 2048);
		mTextSmallWhite.SetText	 (mText);

		// ----- Input ----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;

		// Camera Zoom in / out
		if (mI->Input->OnMouseButtonPress	(STX_MBUTTON_WHEELUP))	mZoom += 0.1f;
		if (mI->Input->OnMouseButtonPress	(STX_MBUTTON_WHEELDOWN))	mZoom -= 0.1f;

		// Camera angle
		if (mI->Input->IsMouseButtonPressed (STX_MBUTTON_LEFT))			mCameraAngle += mSpeedRotation * mDelta;
		if (mI->Input->IsMouseButtonPressed (STX_MBUTTON_RIGHT))		mCameraAngle -= mSpeedRotation * mDelta;

		// ----- Updating entities attributes  -----

		// Rotation of the beetle and bug
		mBugAngle += mSpeedRotation * mDelta;
		mBeetle.SetAngleXYZ (0, 0, mBugAngle);
		mBeetle.SetAngleXYZ (0, 0, mBugAngle);
		mBug.SetAngleXYZ (0, 0, -mBugAngle);
		mBug.SetAngleXYZ (0, 0, -mBugAngle);

		// Zooming and rotating the camera
		if (mZoom < 0.1f) mZoom =  0.1f;
		mCamera2.SetAngle (mCameraAngle);
		mCamera2.SetZoom (mZoom);

		// ----- Render  -----

		// ----- Upper viewport -----

		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight() / 2);
		mI->Render->SetCamera2d (&mCamera1);
		mI->Entity2dManager->RenderEntities2d ();

		// ----- Lower viewport -----

		mI->Render->SetViewPort2d (0, mI->Window->GetHeight() / 2, mI->Window->GetWidth(), mI->Window->GetHeight() / 2);
		mI->Render->SetCamera2d (&mCamera2);
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
	glp->init("b_04_Several_ViewPorts");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

