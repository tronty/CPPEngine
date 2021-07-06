#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	float	mAngle = 0;
	float	mPos = 400;
	int		mSpeed = 200;
	float	mDelta;
	char	mText [2048]; 
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceBack;
		IND_Surface mSurfaceBeetleship;
		IND_Surface mSurfaceOctopus;
		IND_Surface mSurfacePlanet;
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Entity2d mBack;
		IND_Entity2d mBeetleship;
		IND_Entity2d mOctopus;
		IND_Entity2d mPlanet;
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

	// Loading Background
	////////////////////IND_Entity2d mPlanet;
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/twist.jpg", IND_OPAQUE, IND_32)) return 0;

	// Loading Beettleship
	////////////////////IND_Entity2d mPlanet;
	if (!mI->SurfaceManager->Add (&mSurfaceBeetleship, "/beetleship.png", IND_ALPHA, IND_32)) return 0;

	// Loading Octopus
	////////////////////IND_Entity2d mPlanet;
	if (!mI->SurfaceManager->Add (&mSurfaceOctopus, "/octopus.png", IND_ALPHA, IND_32)) return 0;

	// Loading Planet
	////////////////////IND_Entity2d mPlanet;
	if (!mI->SurfaceManager->Add (&mSurfacePlanet, "/planet.png", IND_ALPHA, IND_32)) return 0;

	// Font
	////////////////////IND_Entity2d mPlanet;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	////////////////////IND_Entity2d mPlanet;
	mI->Entity2dManager->Add		(&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Set the surfaces into 2d entities -----

	// Creating 2d entity for the background
	////////////////////IND_Entity2d mPlanet;
	mI->Entity2dManager->Add (&mBack);						// Entity adding
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// Creating 2d entity for the bettleship
	////////////////////IND_Entity2d mPlanet;
	mI->Entity2dManager->Add (&mBeetleship);				// Entity adding
	mBeetleship.SetSurface (&mSurfaceBeetleship);			// Set the surface into the entity

	// Creating 2d entity for the octopus
	////////////////////IND_Entity2d mPlanet;
	mI->Entity2dManager->Add (&mOctopus);					// Entity adding
	mOctopus.SetSurface (&mSurfaceOctopus);					// Set the surface into the entity

	// Creating 2d entity for the planet
	////////////////////IND_Entity2d mPlanet;
	mI->Entity2dManager->Add (&mPlanet);					// Entity adding
	mPlanet.SetSurface (&mSurfacePlanet);					// Set the surface into the entity

	// ----- Changing the attributes of the 2d entities -----

	mBack.SetHotSpot (0.5f, 0.5f);
	mBack.SetPosition (400, 300, 0);
	mBack.SetScale (1.7f, 1.7f);

	mBeetleship.SetHotSpot (0.5f, 0.5f);

	mOctopus.SetMirrorX (true);
	mOctopus.SetHotSpot (0.5f, 0.5f);

	mPlanet.SetHotSpot (0.5f, 0.5f);

	// ----- Main Loop -----


	mText [0] = 0;
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		// ----- Input update -----

		mI->Input->Update ();

		// ----- Text -----

		stx_strlcpy (mText, "Use the mouse for moving the planet\n", 2048);
		stx_strlcat (mText, "Use left and right arrow keys for moving the ships\n", 2048);
		stx_strlcat (mText, "Press CTRL + X or ESC key to quit", 2048);
		mTextSmallWhite.SetText	 (mText);

		// ----- Input -----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;

		// Move entities when pressing right
		if (mI->Input->IsKeyPressed (STX_KEY_RIGHT))	mPos += mSpeed * mDelta;

		// Move entities when pressing left
		if (mI->Input->IsKeyPressed (STX_KEY_LEFT))	mPos -= mSpeed * mDelta;

		// If CTRL + X pressed then stx_exit
		if (mI->Input->IsKeyPressed (STX_KEY_LCTRL) && mI->Input->IsKeyPressed (STX_KEY_x))
		{
			mI->Render->EndScene ();
			//mI->End0???();
			stx_exit (0);
		}

		mAngle += (mSpeed / 4) * mDelta;

		// ----- Updating entities attributes  -----

		// Back
		mBack.SetAngleXYZ (0, 0, mAngle);

		// Beetle
		mBeetleship.SetPosition ((float) mPos, 140, 1);
		mBeetleship.SetAngleXYZ (0, 0, (float) mPos);

		// Planet
		mPlanet.SetPosition ((float) mI->Input->GetMouseX(), 300, 2);

		// Octopus
		mOctopus.SetPosition (800 - (float) mPos, 480, 3);

		// ----- Render -----

		mI->Render->BeginScene ();
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
	glp->init("a_07_IND_Input");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

