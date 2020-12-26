#include <CIndieLib.h>
#include "FloatingText.h"

const float FADE_DELAY	= 3.0f; // seconds


/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
float fElapsedTime = 0.0f;
	FloatingText*	pFloatingText = 0;
	char szPoints[256];	

class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Timer timer;
		IND_Font Font;
		IND_Entity2d guiText;
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
	
//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->
		Init (aTitle
		)) return -1;
#endif
	//////IND_Entity2d guiText;



	//////IND_Entity2d guiText;
	if (!mI->FontManager->Add (&Font, "/IndieLib-FloatingText/Army_05.png", "/IndieLib-FloatingText/Army_05.xml", IND_ALPHA, IND_32))
		return false;

	//////IND_Entity2d guiText;
	mI->Entity2dManager->Add(&guiText);
	guiText.SetFont(&Font);
	guiText.SetPosition(0,0,0);
	guiText.SetCharSpacing(0);
	guiText.SetAlign(IND_LEFT);

	// ----- Main Loop -----

	timer.Start();

	mI->Window->Cursor(true);
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;

		mI->Input->Update ();

		guiText.SetText("Left mouse creates floating text");

		// ----- Delta Time -----
		fElapsedTime = mI->Render->GetFrameTime() / 1000.0f;

		if (pFloatingText == 0 && mI->Input->OnMouseButtonPress(STX_MBUTTON_LEFT))
		{

			stx_itoa(mI->Input->GetMouseX(), szPoints, 10);
			//LOG_PRINT("new pFloatingText;(1)\n");

			pFloatingText = new FloatingText;
			//LOG_PRINT("%x\n", pFloatingText);
			//LOG_PRINT("new pFloatingText;(2)\n");

			pFloatingText->Init(
				szPoints,
				&Font,
				0,
				2.0f,
				D3DXFROMWINEVECTOR3((float)mI->Input->GetMouseX(), (float)mI->Input->GetMouseY(),0),
				D3DXFROMWINEVECTOR3(0.0f, -60.0f, 0));

		}
		
		if (pFloatingText)
		{
			//LOG_PRINT("%f\n",fElapsedTime);

			pFloatingText->Update(fElapsedTime);

			if (pFloatingText->IsDead())
			{
				//LOG_PRINT("delete pFloatingText;(1)\n");

				delete pFloatingText;

				//LOG_PRINT("delete pFloatingText;(2)\n");
				pFloatingText = 0;
			}
		}

		mI->Render->BeginScene ();

		mI->Render->ClearViewPort (60, 60, 60);

		mI->Entity2dManager->RenderEntities2d ( );

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
	glp->init("IndieLib-FloatingText");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();

	// ----- Free -----
	//???mI->FontManager->Delete(&Font);

	return 0;
}

