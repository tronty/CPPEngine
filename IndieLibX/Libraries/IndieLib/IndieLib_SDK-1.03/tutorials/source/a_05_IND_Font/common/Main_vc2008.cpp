#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceBack;
		IND_Surface mSurfaceTip;
		IND_Font mFontSmall;
		IND_Font mFontBig;
		IND_Entity2d mBack;
		IND_Entity2d mTip;
		IND_Entity2d mTextTitle;
		IND_Entity2d mTextSmallBlack;
		IND_Entity2d mTextSmallWhite;
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
	// ----- Surface and font loading -----

	// Loading Background
	//////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/blue_background.jpg", IND_OPAQUE, IND_32)) return 0;

	// Loading tip page
	//////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add (&mSurfaceTip, "/tip.png", IND_ALPHA, IND_32)) return 0;

	// Font 1
	//////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// Font 2
	//////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->FontManager->Add (&mFontBig, "/font_big.png", "/font_big.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Set the surfaces and fonts into 2d entities -----

	// Creating 2d entity for the background
	//////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mBack);						// Entity adding
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// Creating 2d entity for the tips page
	//////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mTip);						// Entity adding
	mTip.SetSurface (&mSurfaceTip);							// Set the surface into the entity

	// Title text
	//////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mTextTitle);					// Entity adding
	mTextTitle.SetFont (&mFontBig);							// Set the font into the entity

	// Text small black
	//////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mTextSmallBlack);			// Entity adding
	mTextSmallBlack.SetFont (&mFontSmall);					// Set the font into the entity

	// Text small white
	//////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont (&mFontSmall);					// Set the font into the entity

	// ----- Changing the attributes of the 2d entities -----

	mTip.SetHotSpot (0.5f, 0.5f);
	mTip.SetPosition (250, 300, 0);

	mTextTitle.SetText				("IndieLib Fonts");
	mTextTitle.SetLineSpacing		(18);
	mTextTitle.SetCharSpacing		(-8);
	mTextTitle.SetPosition			(400, 40, 1);
	mTextTitle.SetAlign				(IND_CENTER);

	mTextSmallBlack.SetText			("This paragraph is\ncentered using.\nIND_CENTER\n\nThis font has\nblack color using\nSetTint() method.\n\nRocks!");
	mTextSmallBlack.SetLineSpacing	(18);
	mTextSmallBlack.SetCharSpacing	(-8);
	mTextSmallBlack.SetPosition		(242, 230, 1);
	mTextSmallBlack.SetAlign		(IND_CENTER);
	mTextSmallBlack.SetTint			(0, 0, 0);

	mTextSmallWhite.SetText			("This is a another paragraph. This time\nusing the font without changing the color\nand with a IND_LEFT alignment.\n\nThis is a really easy way of writing\ntexts, isn't it?");
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-7);
	mTextSmallWhite.SetPosition		(400, 230, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Main Loop -----
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		mI->Input->Update ();
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
	glp->init("a_05_IND_Font");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

