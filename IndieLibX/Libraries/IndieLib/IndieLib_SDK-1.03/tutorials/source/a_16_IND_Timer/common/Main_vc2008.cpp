#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	char mTimeString [128]; 
	char mTempTime [1024];

	

	int mX = 0;
	int mSecond;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Font mFontBig;
		IND_Surface mSurfaceDraco;
		IND_Entity2d mTextTime;
		IND_Entity2d mDraco;
		IND_Timer mTimer;
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

	// Font
	//////////IND_Timer mTimer;
	if (!mI->FontManager->Add (&mFontBig, "/font_big.png", "/font_big.xml", IND_ALPHA, IND_32)) return 0;

	// Loading draco
	//////////IND_Timer mTimer;
	if (!mI->SurfaceManager->Add (&mSurfaceDraco, "/draco.png", IND_ALPHA, IND_32)) return 0;

	// ----- Entities -----

	// Title text
	//////////IND_Timer mTimer;
	mI->Entity2dManager->Add (&mTextTime);					// Entity adding
	mTextTime.SetFont (&mFontBig);							// Set the font into the entity

	// Creating 2d entity for the draco
	//////////IND_Timer mTimer;
	mI->Entity2dManager->Add (&mDraco);						// Entity adding
	mDraco.SetSurface (&mSurfaceDraco);						// Set the surface into the entity
	mDraco.SetHotSpot (0.5f, 0.5f);
	mDraco.SetPosition (400, 330, 1);

	// ----- Changing the attributes of the 2d entities -----

	// Text showing the time
	mTextTime.SetLineSpacing		(18);
	mTextTime.SetCharSpacing		(-8);
	mTextTime.SetPosition			(280, 20, 1);
	mTextTime.SetAlign				(IND_LEFT);

	// ----- Main Loop -----

	// Create and start the timer;
	//////////IND_Timer mTimer;
	mTimer.Start();
	mTimeString [0] = 0;
	STX_CATCH;
	return 0;


	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Input ----

		// Pause / Restart time when pressing space
		if (mI->Input->OnKeyPress (STX_KEY_SPACE))
		{
			if (mTimer.IsPaused()) mTimer.Unpause(); else mTimer.Pause();
		}

		// ----- Updating entities attributes  -----

		mSecond = (int) (mTimer.GetTicks() / 1000.0f);

		// Show the time passing in seconds
		stx_itoa (mSecond, (char *) &mTempTime, 10);
		stx_strlcpy (mTimeString, "Seconds: ", 2048);
		stx_strlcat (mTimeString, mTempTime, 2048);
		mTextTime.SetText	 (mTimeString);

		// Update Draco position each second
		mDraco.SetAngleXYZ (0, 0, (float) mSecond + 10);

		// ----- Render  -----

		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
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
	glp->init("a_16_IND_Timer");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

