#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	int mMiddleScreenX;// = mI->Window->GetWidth () / 2;
	int mMiddleScreenY;// = mI->Window->GetHeight() / 2;

	float mPosXCamera0;// = (float) mMiddleScreenX;
	float mPosXCamera1;// = (float) mMiddleScreenX;
	float mPosXCamera2;// = (float) mMiddleScreenX;

	int		mSpeedLayer0		= 50;
	int		mSpeedLayer1		= 162;
	int		mSpeedLayer2		= 250;

	// --- Some variables ---

	char	mText [2048]; 
	float	mDelta;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceCave;
		IND_Surface mSurfaceCaveFirstPlane;
		IND_Surface mSurfaceSky;
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Entity2d mSky;
		IND_Entity2d mCave;
		IND_Entity2d mCaveFirstPlane;
		IND_Camera2d mCamera0;
	IND_Camera2d mCamera1;
	IND_Camera2d mCamera2;
	IND_Camera2d mCameraText;
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

	// Loading cave
	////////////////IND_Entity2d mCaveFirstPlane;
	if (!mI->SurfaceManager->Add (&mSurfaceCave, "/cave.png", IND_ALPHA, IND_32)) return 0;

	// Loading cave (first plane)
	////////////////IND_Entity2d mCaveFirstPlane;
	if (!mI->SurfaceManager->Add (&mSurfaceCaveFirstPlane, "/cave_near.png", IND_ALPHA, IND_32)) return 0;

	// Loading sky
	////////////////IND_Entity2d mCaveFirstPlane;
	if (!mI->SurfaceManager->Add (&mSurfaceSky, "/sky.jpg", IND_OPAQUE, IND_32)) return 0;

	// Font
	////////////////IND_Entity2d mCaveFirstPlane;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	////////////////IND_Entity2d mCaveFirstPlane;
	mI->Entity2dManager->Add		(3, &mTextSmallWhite);		// Entity adding (Layer 3)
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Entities -----

	// Creating 2d entity for the sky
	////////////////IND_Entity2d mCaveFirstPlane;
	mI->Entity2dManager->Add (0, &mSky);							// Entity adding (Layer 0)
	mSky.SetSurface (&mSurfaceSky);
	mSky.SetPosition (600, 0, 0);

	// Creating 2d entity for the cave
	////////////////IND_Entity2d mCaveFirstPlane;
	mI->Entity2dManager->Add (1, &mCave);							// Entity adding (Layer 1)
	mCave.SetSurface (&mSurfaceCave);

	// Creating 2d entity for the cave (first plane)
	////////////////IND_Entity2d mCaveFirstPlane;
	mI->Entity2dManager->Add (2, &mCaveFirstPlane);					// Entity adding (Layer 2)
	mCaveFirstPlane.SetSurface (&mSurfaceCaveFirstPlane);

	// ----- Cameras -----

	// --- Cameras for the parallax layers ---
	mCamera0=IND_Camera2d	((int) mPosXCamera0, mMiddleScreenY);
	mCamera1=IND_Camera2d	((int) mPosXCamera1, mMiddleScreenY);
	mCamera2=IND_Camera2d	((int) mPosXCamera2, mMiddleScreenY);

// --- Camera for showing the text that explain the input controls ---

	mCameraText=IND_Camera2d ((int) mI->Window->GetWidth() / 2, mI->Window->GetHeight() / 2);

	mText [0] = 0;
	mMiddleScreenX = mI->Window->GetWidth () / 2;
	mMiddleScreenY = mI->Window->GetHeight() / 2;

	mPosXCamera0 = (float) mMiddleScreenX;
	mPosXCamera1 = (float) mMiddleScreenX;
	mPosXCamera2 = (float) mMiddleScreenX;
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Text -----

		stx_strlcpy (mText, "Use right and left arrow keys for moving the camera\n", 2048);
		mTextSmallWhite.SetText	 (mText);

		// ----- Input ----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;

		// Move cameras when pressing right key
		if (mI->Input->IsKeyPressed (STX_KEY_RIGHT))
		{
			// Check limits
			if (mPosXCamera2 < mSurfaceCaveFirstPlane.GetWidth() - mMiddleScreenX)
			{
				mPosXCamera0 += mSpeedLayer0 * mDelta;
				mPosXCamera1 += mSpeedLayer1 * mDelta;
				mPosXCamera2 += mSpeedLayer2 * mDelta;
			}
		}

		// Move cameras when pressing left key
		if (mI->Input->IsKeyPressed (STX_KEY_LEFT))
		{
			// Check limits
			if (mPosXCamera2 > mMiddleScreenX)
			{
				mPosXCamera0 -= mSpeedLayer0 * mDelta;
				mPosXCamera1 -= mSpeedLayer1 * mDelta;
				mPosXCamera2 -= mSpeedLayer2 * mDelta;
			}
		}

		// ----- Updating cameras-----

		mCamera0.SetPosition ((int) mPosXCamera0, mMiddleScreenY);
		mCamera1.SetPosition ((int) mPosXCamera1, mMiddleScreenY);
		mCamera2.SetPosition ((int) mPosXCamera2, mMiddleScreenY);

		// ----- Render  -----

		mI->Render->ClearViewPort(60, 60, 60);
		mI->Render->BeginScene ();

		// Render sky (Layer 0)
		mI->Render->SetCamera2d (&mCamera0);
		mI->Entity2dManager->RenderEntities2d (0);

		// Render cave (Layer 1)
		mI->Render->SetCamera2d (&mCamera1);
		mI->Entity2dManager->RenderEntities2d (1);

		// Render the first plane cave (Layer 2)
		mI->Render->SetCamera2d (&mCamera2);
		mI->Entity2dManager->RenderEntities2d (2);

		// Render the text explaining the controls plane cave (Layer 3)
		mI->Render->SetCamera2d (&mCameraText);
		mI->Entity2dManager->RenderEntities2d (3);

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
	glp->init("a_15_Parallax_Scrolling");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

