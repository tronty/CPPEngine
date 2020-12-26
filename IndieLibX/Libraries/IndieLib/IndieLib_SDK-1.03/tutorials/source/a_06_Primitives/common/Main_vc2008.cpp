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
		IND_Entity2d mPixel;
		IND_Entity2d mRegularPoly;
		IND_Entity2d mLine;
		IND_Entity2d mRectangle;
		IND_Entity2d mFillRectangle;
		IND_Entity2d mPoly2d;
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
	// ----- Set the primitives into 2d entities -----
#if 1
	// Pixel
	////////////IND_Entity2d mPoly2d;
	mI->Entity2dManager->Add (&mPixel);
	mPixel.SetPrimitive2d (IND_PIXEL);
	mPixel.SetPosition (200, 75, 0);
	mPixel.SetTint (255, 255, 255);

	// Regular poly
	////////////IND_Entity2d mPoly2d;
	mI->Entity2dManager->Add (&mRegularPoly);
	mRegularPoly.SetPrimitive2d (IND_REGULAR_POLY);
	mRegularPoly.SetPosition (200, 75, 0);
	mRegularPoly.SetNumSides (5);
	mRegularPoly.SetRadius (50);
	mRegularPoly.SetTint (255, 0, 0);

	// Line
	////////////IND_Entity2d mPoly2d;
	mI->Entity2dManager->Add (&mLine);
	mLine.SetPrimitive2d (IND_LINE);
	mLine.SetLine (100, 100, 50, 50);
	mLine.SetTint (0, 255, 0);

	// Rectangle
	////////////IND_Entity2d mPoly2d;
	mI->Entity2dManager->Add (&mRectangle);
	mRectangle.SetPrimitive2d (IND_RECTANGLE);
	mRectangle.SetRectangle (350, 50, 400, 100);
	mRectangle.SetTint (0, 0, 255);

	// Filled rectangle
	////////////IND_Entity2d mPoly2d;
	mI->Entity2dManager->Add (&mFillRectangle);
	mFillRectangle.SetPrimitive2d (IND_FILL_RECTANGLE);
	mFillRectangle.SetRectangle (550, 40, 650, 110);
	mFillRectangle.SetTint (255, 0, 0);
	mFillRectangle.SetTransparency (50);

	// 2d Poly
	////////////IND_Entity2d mPoly2d;
	mI->Entity2dManager->Add (&mPoly2d);
	mPoly2d.SetPrimitive2d (IND_POLY2D);
	IND_Point mVertPoly2 [] = { {440, 200},  {480, 100},  {450, 10},  {470, 220} };		// Poly points
	mPoly2d.SetPolyPoints (mVertPoly2);
	mPoly2d.SetNumLines	(3);	 														// Number of edges - 1
	mPoly2d.SetTint (255, 128, 255);
#endif
	// ----- Main Loop -----
	STX_CATCH;
	return 0;
	}
virtual void render()
{
	STX_TRY;
		// ----- Input update -----

		mI->Input->Update ();

		// ----- Render -----

		mI->Render->ClearViewPort (0, 0, 0);
		mI->Render->BeginScene ();

		// Direct bliting of primitives
		for (int i = 0; i < 400; i += 5)
		{
			mI->Render->BlitLine		(70, 150, i * 2, 500, i, 255 -i, 255, 255);
			mI->Render->BlitRegularPoly	(600, 600, i, 70, 0, 255 - i, i, i*4, 255);
		}

		mI->Entity2dManager->RenderEntities2d();
		mI->Render->EndScene ();
		STX_CATCH;
	}

	// ----- Free -----

	};
GameLogic gl;
IGameLogic* glp=&gl;
#include <STX/STXCPP.h>
int init(const char* aTitle){return glp->init(aTitle);}
void render(){return glp->render();}
int IndieLib()
{
	glp->init("a_06_Primitives");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

