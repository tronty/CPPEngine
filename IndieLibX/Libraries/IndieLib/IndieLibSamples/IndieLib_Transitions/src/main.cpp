#include <CIndieLib.h>
#include "Transition.h"

void Update(float fElapsedTime);

//CIndieLib * mI = 0;
int				g_Counter			= 0;
TransitionType	g_TransitionType;
bool			g_Swap				= false;
std::string		g_szText;
std::string		g_szTitle;
Transition *	g_pMainTransition	= 0;
IND_Entity2d *	g_pSourceEntity2d	= 0;
IND_Entity2d *	g_pDestEntity2d		= 0;
float			g_fDuration			= 1.0f;		// seconds
float fElapsedTime = 0.0f;
	int iiiAlpha = 255;
	
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
		IND_Surface SurfaceSnake;
		IND_Surface SurfacePool;
		IND_Font FontSmall;
		IND_Entity2d TextSmallWhite;
		IND_Timer timer;
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();
	// ----- IndieLib intialization -----

#if 0
	//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->Init ()) return 0;
#else
	
	mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->
		Init (aTitle
		)) return -1;
#endif

	// Loading layer 1
	//////////IND_Timer timer;
	if (!mI->SurfaceManager->Add (&SurfaceSnake, "/IndieLib-Transitions/snake_512.jpg", IND_ALPHA, IND_32)) return 0;

	// Loading layer 1
	//////////IND_Timer timer;
	if (!mI->SurfaceManager->Add (&SurfacePool, "/IndieLib-Transitions/pool_512.jpg", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----
	// Font (layer 3)
	//////////IND_Timer timer;
	if (!mI->FontManager->Add (&FontSmall, "/IndieLib-Transitions/font_small.png", "/IndieLib-Transitions/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// Create the Font
	//////////IND_Timer timer;
	mI->Entity2dManager->Add		(1, &TextSmallWhite);		// Entity adding (Layer 3)
	TextSmallWhite.SetFont			(&FontSmall);				// Set the font into the entity
	TextSmallWhite.SetLineSpacing	(18);
	TextSmallWhite.SetCharSpacing	(-8);
	TextSmallWhite.SetPosition		(5, 5, 1);
	TextSmallWhite.SetAlign		(IND_LEFT);

	// Creating 2d entity for the pool
	g_pSourceEntity2d = new IND_Entity2d();
	mI->Entity2dManager->Add (0, g_pSourceEntity2d);			// Entity adding !!! Note the first parameter is layer #
	g_pSourceEntity2d->SetSurface (&SurfacePool);				// Set the surface into the entity

	// Creating 2d entity for the snake
	g_pDestEntity2d	= new IND_Entity2d();
	mI->Entity2dManager->Add (0, g_pDestEntity2d);			// Entity adding !!! Note the first parameter is layer #
	g_pDestEntity2d->SetSurface (&SurfaceSnake);			// Set the surface into the entity
	g_pDestEntity2d->SetPosition(0, 0, 0);


	//////////IND_Timer timer;

	g_szTitle = "ALPHA2";
	
	// ----- Main Loop -----

	timer.Start();

	// Create the first transition
	g_pMainTransition = new Transition(g_pSourceEntity2d, g_pDestEntity2d, ALPHA2, g_pSourceEntity2d->GetSurface()->GetWidth(), g_pSourceEntity2d->GetSurface()->GetHeight());
	STX_CATCH;
	return 0;


	}
virtual void render()
{

		STX_TRY;
		mI->Input->Update ();

		fElapsedTime = mI->Render->GetFrameTime() / 1000.0f;

		// ----- Text -----
		g_szText = g_szTitle;
		g_szText += "\n";
		g_szText += "Hit SPACE for next transition\n";
		g_szText += "ESC to quit";
		TextSmallWhite.SetText	 ((char*)g_szText.c_str());



		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		Update(fElapsedTime);


		for (int i = 0; i < 2; i++)
			mI->Entity2dManager->RenderEntities2d ( i );

		//???mI->Render->ShowFpsInWindowTitle();
		mI->Render->EndScene ();
		STX_CATCH;
	}

	};
GameLogic gl;
IGameLogic* glp=&gl;
int init(const char* aTitle){return glp->init(aTitle);}
void render(){return glp->render();}
int IndieLib()
{
	glp->init("IndieLib-Transitions");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();

	// ----- Free -----
	if (g_pMainTransition)
		delete g_pMainTransition;


	//mI->Entity2dManager->Delete(g_pSourceEntity2d);

	//mI->Entity2dManager->Delete(g_pDestEntity2d);

	return 0;
}

// Updates the transitions
void Update(float fElapsedTime)
{
	if (mI->Input->OnKeyPress (STX_KEY_SPACE))
	{
		if (g_pMainTransition->IsComplete())
		{
			if (g_Counter > 27)
				g_Counter = 0;

			switch (g_Counter)
			{
				case 0:
					g_TransitionType = ALPHA2;
					g_szTitle =  "ALPHA2";
					break;
				case 1:
					g_TransitionType = WAVE_ALPHA;
					g_szTitle =  "WAVE_ALPHA";
					break;
				case 2:
					g_TransitionType = WIPE_LEFT;
					g_szTitle =  "WIPE_LEFT";
					break;
				case 3:
					g_TransitionType = WIPE_RIGHT;
 					g_szTitle =  "WIPE_RIGHT";
					break;
				case 4:
					g_TransitionType = WIPE_UP;
					g_szTitle =  "WIPE_UP";
					break;
				case 5:
					g_TransitionType = WIPE_DOWN;
					g_szTitle =  "WIPE_DOWN";
					break;
				case 6:
					g_TransitionType = SCROLL_LEFT;
					g_szTitle =  "SCROLL_LEFT";
					break;
				case 7:
					g_TransitionType = SCROLL_RIGHT;
					g_szTitle =  "SCROLL_RIGHT";
					break;
				case 8:
					g_TransitionType = SCROLL_UP;
					g_szTitle =  "SCROLL_UP";
					break;
				case 9:
					g_TransitionType = SCROLL_DOWN;
					g_szTitle =  "SCROLL_DOWN";
					break;
				case 10:
					g_TransitionType = SQUEEZE_LEFT;
					g_szTitle =  "SQUEEZE_LEFT";
					break;
				case 11:
					g_TransitionType = SQUEEZE_RIGHT;
					g_szTitle =  "SQUEEZE_RIGHT";
					break;
				case 12:
					g_TransitionType = SQUEEZE_UP;
					g_szTitle =  "SQUEEZE_UP";
					break;
				case 13:
					g_TransitionType = SQUEEZE_DOWN;
					g_szTitle =  "SQUEEZE_DOWN";
					break;
				case 14:
					g_TransitionType = SHRINK;
					g_szTitle =  "SHRINK";
					break;
				case 15:
					g_TransitionType = SHRINK_SPIN_CLOCKWISE;
					g_szTitle =  "SHRINK_SPIN_CLOCKWISE";
					break;
				case 16:
					g_TransitionType = SHRINK_SPIN_COUNTERCLOCKWISE;
					g_szTitle =  "SHRINK_SPIN_COUNTERCLOCKWISE";
					break;
				case 17:
					g_TransitionType = SLIDE_LEFT;
					g_szTitle =  "SLIDE_LEFT";
					break;
				case 18:
					g_TransitionType = SLIDE_RIGHT;
					g_szTitle =  "SLIDE_RIGHT";
					break;
				case 19:
					g_TransitionType = SLIDE_UP;
					g_szTitle =  "SLIDE_UP";
					break;
				case 20:
					g_TransitionType = SLIDE_DOWN;
					g_szTitle =  "SLIDE_DOWN";
					break;
				case 21:
					g_TransitionType = SLIDE_OUT_LEFT;
					g_szTitle =  "SLIDE_OUT_LEFT";
					break;
				case 22:
					g_TransitionType = SLIDE_OUT_RIGHT;
					g_szTitle =  "SLIDE_OUT_RIGHT";
					break;
				case 23:
					g_TransitionType = SLIDE_OUT_UP;
					g_szTitle =  "SLIDE_OUT_UP";
					break;
				case 24:
					g_TransitionType = SLIDE_OUT_DOWN;
					g_szTitle =  "SLIDE_OUT_DOWN";
					break;
				case 25:
					g_TransitionType = EXPAND;
					g_szTitle =  "EXPAND";
					break;
				case 26:
					g_TransitionType = EXPAND_SPIN_CLOCKWISE;
					g_szTitle =  "EXPAND_SPIN_CLOCKWISE";
					break;
				case 27:
					g_TransitionType = EXPAND_SPIN_COUNTERCLOCKWISE;
					g_szTitle =  "EXPAND_SPIN_COUNTERCLOCKWISE";
					break;
			}

			delete g_pMainTransition;

			if (g_Swap)
				g_pMainTransition = new Transition(g_pSourceEntity2d, g_pDestEntity2d, g_TransitionType, g_pSourceEntity2d->GetSurface()->GetWidth(), g_pSourceEntity2d->GetSurface()->GetHeight(), g_fDuration);
			else
				g_pMainTransition = new Transition(g_pDestEntity2d, g_pSourceEntity2d, g_TransitionType, g_pSourceEntity2d->GetSurface()->GetWidth(), g_pSourceEntity2d->GetSurface()->GetHeight(), g_fDuration);


			g_Swap = !g_Swap;
			g_Counter++;

		}
	}

	if (!g_pMainTransition->IsComplete())
		g_pMainTransition->Tick( fElapsedTime );
}

