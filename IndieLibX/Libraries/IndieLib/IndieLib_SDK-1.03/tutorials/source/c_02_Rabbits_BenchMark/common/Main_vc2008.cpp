#include <CIndieLib.h>
#include "CRabbit.h"

#define MAX_OBJECTS 1000

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
char mFpsString [128];
CRabbit *mRabbits = 0;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceRabbit;
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();
	// ----- IndieLib intialization -----

#if 0
	//CIndieLib * mI = CIndieLib::Instance();
	if (!mI->Init (aTitle)) return 0;
#else
	
//CIndieLib * mI = CIndieLib::Instance();
	if (!mI->
		Init (aTitle
		
		)) return -1;
#endif
	// ----- Surface loading -----

	// Loading rabbit
	//IND_Entity2d mTextSmallWhite
	if (!mI->SurfaceManager->Add (&mSurfaceRabbit, "/rabbit.png", IND_ALPHA, IND_32)) return 0;

	// Font
	//IND_Entity2d mTextSmallWhite
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Create rabbits -----

	mRabbits = new CRabbit [MAX_OBJECTS];

	int i;
	for (i = 0; i < MAX_OBJECTS; i++) mRabbits[i].Init (&mSurfaceRabbit, i);

	// ----- Font creation -----

	//IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add	(&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont		(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition	(5, 5, i);
	mTextSmallWhite.SetAlign	(IND_LEFT);

	// ----- Main Loop -----

	mFpsString [0] = 0;
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Fps -----
#if 1
		stx_strlcpy (mFpsString, "Fps: ", 128);
		char mFpsString2[128];
		mI->Render->GetFpsString(mFpsString2,128);
		stx_strlcat (mFpsString,mFpsString2, 128 );
		stx_strlcat (mFpsString, "\nPress space to toggle full screen", 128);
		mTextSmallWhite.SetText	 (mFpsString);
#else
		mTextSmallWhite.SetText	 ("Fps:");
#endif
		// ----- Game logic ----

		// Toogle full screen when pressing "space"
		if (mI->Input->OnKeyPress (STX_KEY_SPACE)) mI->Render->ToggleFullScreen();

		// Update rabbits position
		for (int i = 0; i < MAX_OBJECTS; i++) mRabbits[i].Update();

		// ----- Render -----

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
	glp->init("c_02_Rabbits_BenchMark");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();

	delete [] mRabbits;

	return 0;
}

