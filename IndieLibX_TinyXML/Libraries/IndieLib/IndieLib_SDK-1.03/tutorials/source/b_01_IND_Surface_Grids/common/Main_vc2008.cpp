#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
int mNumBlocksX;
	int mNumBlocksY;
	int mWidthBlock;
	int mHeightBlock;
	bool mShowGrid = 0;
	float mAngle = 0;
	//////////////IND_Timer mTimer; mTimer.Start();
	float mT;
	char	mText [2048]; 
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceBack;
		IND_Surface mSurfaceDraco;
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Entity2d mBack;
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

	// Loading Background
	//////////////IND_Timer mTimer;
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/twist.jpg", IND_OPAQUE, IND_32)) return 0;

	// Loading draco
	//////////////IND_Timer mTimer;
	if (!mI->SurfaceManager->Add (&mSurfaceDraco, "/draco.png", IND_ALPHA, IND_32)) return 0;

	// Font
	//////////////IND_Timer mTimer;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	//////////////IND_Timer mTimer;
	mI->Entity2dManager->Add		(&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Create a grid for Draco IND_Surface -----

	mSurfaceDraco.SetGrid (8, 8);

	// ----- Set the surfaces into 2d entities -----

	// Creating 2d entity for the background
	//////////////IND_Timer mTimer;
	mI->Entity2dManager->Add (&mBack);						// Entity adding
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// Creating 2d entity for the draco
	//////////////IND_Timer mTimer;
	mI->Entity2dManager->Add (&mDraco);						// Entity adding
	mDraco.SetSurface (&mSurfaceDraco);						// Set the surface into the entity

	// ----- Changing the attributes of the 2d entities -----

	// Background
	mBack.SetHotSpot (0.5f, 0.5f);
	mBack.SetPosition (400, 300, 0);
	mBack.SetScale (1.7f, 1.7f);

	// Draco
	mDraco.SetPosition (150, 50, 1);

	// ----- Main Loop -----
	mText [0] = 0;
	
	mNumBlocksX = mSurfaceDraco.GetBlocksX();
	mNumBlocksY = mSurfaceDraco.GetBlocksY();
	mWidthBlock = mSurfaceDraco.GetWidthBlock();
	mHeightBlock = mSurfaceDraco.GetHeightBlock();
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Text -----

		stx_strlcpy (mText, "Press space to see the grid in action. This is really cool, isn't it?", 2048);
		mTextSmallWhite.SetText	 (mText);

		// ----- Input ----

		// Show / Hide the grid pressing "space"
		if (mI->Input->OnKeyPress (STX_KEY_SPACE))
		{
			if (mShowGrid) mShowGrid = 0; else mShowGrid = 1;
		}

		// ----- Updating entities attributes  -----

		mAngle += 0.1f;
		mBack.SetAngleXYZ (0, 0, mAngle);

		// Update grid vertices for making a "wave" effect
		mT = mTimer.GetTicks() / 1000.0f;

		for (int i = 1; i < mNumBlocksX; i++)
			for (int j = 1; j < mNumBlocksY; j++)
				mSurfaceDraco.SetVertexPos (j,
											i,
											(int) ((j * mHeightBlock + cosf (mT * 10 + (i + j) / 2) * 5)),
											(int) ((i * mWidthBlock	+ sinf (mT * 10 + (i + j) / 2) * 5)));

		// ----- Render  -----

		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
		mI->Entity2dManager->RenderEntities2d ();
		if (mShowGrid) 	mI->Entity2dManager->RenderGridAreas (0, 0, 0, 255);
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
	glp->init("b_01_IND_Surface_grids");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

