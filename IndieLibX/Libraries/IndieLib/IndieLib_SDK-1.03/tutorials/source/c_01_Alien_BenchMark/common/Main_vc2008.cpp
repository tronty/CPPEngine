/*****************************************************************************************
/* Desc: Test c) 01 Alien BenchMark
/*****************************************************************************************/

#include <CIndieLib.h>
#include "CTerrain.h"
#include "CUfo.h"   

#define X_TILES 8																// Horizontal number of tiles of the terrain
#define Y_TILES 6																// Vertical number of tiles of the terrain
#define NUM_HOLES 130															// Number of craters for the terrain
#define CAMERA_SPEED 300														// Camera speed

#define CAMERA_DISPLACE (CAMERA_SPEED * mI->Render->GetFrameTime() / 1000.0f)	// Camera displacement
#define MOVE_TERRAIN_BY_MOUSE_AREA 20											// We move the terrain if the mouse is on this area

	float	mZoom = 1.0f;
	float	mAngle = 0;
	char	mFpsString [128]={0}; 
	int	mSpeedRotation = 50;
	float	mDelta;
	CUfo *mUfos = 0;
	int cont = 0;
	CIndieLib *mI = 0;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceCursor;
		IND_Surface mSurfaceTerrain;
		IND_Surface mSurfaceHole1;
		IND_Surface mSurfaceHole2;
		IND_Font mFontSmall;
		IND_Animation mAnimationUfo;
		IND_Entity2d mCursor;
		IND_Entity2d mTextSmallWhite;
		IND_Camera2d mCameraTerrain;
		IND_Camera2d mCameraGui;
		CTerrain* mTerrain;
		virtual int init(const char* aTitle)
		{
	// ----- IndieLib intialization -----

#if 0
CIndieLib::Instance();
	if (!mI->Init (aTitle)) return 0;			
#else
	//g_pIndieLib = 
	mI = CIndieLib::Instance();
	if (!mI->
		Init (aTitle
		
		)) return -1;
#endif

	// ----- Surface loading -----

	// Loading cursor
	if (!mI->SurfaceManager->Add (&mSurfaceCursor, "/cursor.png", IND_ALPHA, IND_32)) return 0;


	// Loading tile for the terrain
	if (!mI->SurfaceManager->Add (&mSurfaceTerrain, "/tiled_terrain.png", IND_OPAQUE, IND_32)) return 0;


	// Loading alien hole 1
	if (!mI->SurfaceManager->Add (&mSurfaceHole1, "/hole1.png", IND_ALPHA, IND_32)) return 0;

	
	// Loading alien hole 2
	if (!mI->SurfaceManager->Add (&mSurfaceHole2, "/hole2.png", IND_ALPHA, IND_32)) return 0;


	// Font
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Animation loading -----

	if (!mI->AnimationManager->AddToSurface (&mAnimationUfo, "/animations/ufo.xml", IND_ALPHA, IND_32)) return 0;


	// ----- Cursor creation -----

	mI->Entity2dManager->Add (1, &mCursor);			// Entity adding in layer 1 (gui)
	
	mCursor.SetSurface (&mSurfaceCursor);


	// ----- Terrain creation -----

	mTerrain=new CTerrain(X_TILES, Y_TILES, NUM_HOLES, &mSurfaceTerrain, &mSurfaceHole1, &mSurfaceHole2);


	// ----- Font creation -----
				
	mI->Entity2dManager->Add (1, &mTextSmallWhite);			// Entity adding in layer 1 (gui)

	mTextSmallWhite.SetFont (&mFontSmall);					// Set the font into the entity


	mTextSmallWhite.SetLineSpacing	(18);

	mTextSmallWhite.SetCharSpacing	(-8);

	mTextSmallWhite.SetPosition		(15, 15, 4);

	mTextSmallWhite.SetAlign		(IND_LEFT);


	// ----- Ufo creation -----

	CUfo *mUfos = new CUfo [X_TILES * Y_TILES * 1024];


	int cont = 0;

	for (int i = 0; i < X_TILES * 512; i += 50)
		for (int j = 0; j < Y_TILES * 512; j += 50)
			mUfos [cont++].Init ((float) i, (float) j, &mAnimationUfo);

		
	// ----- Camera ------

	// Camera used for moving along the terrain
	mCameraTerrain=IND_Camera2d(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);


	// Camera used for showing the menu options, fps, etc (this camera dosen't change)
	mCameraGui=IND_Camera2d(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);



	return 0;

	}
virtual void render()
{

	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{

		// ----- Input update ----

		mI->Input->Update ();


		// ----- Texts -----

		char mFpsString2[128]={0};

		stx_strlcpy (mFpsString, "Fps: ", 128);

		//strcat (mFpsString, mI->Render->GetFpsString());
		mI->Render->GetFpsString(mFpsString2, 128);

		stx_strlcat (mFpsString, mFpsString2, 128);

		stx_strlcat (mFpsString, "\n", 128);

		stx_strlcat (mFpsString, "Rendered: ", 128);

		stx_strlcat (mFpsString, mI->Render->GetNumRenderedObjectsString(), 128);

		stx_strlcat (mFpsString, "\n", 128);

		stx_strlcat (mFpsString, "Discarded: ", 128);

		stx_strlcat (mFpsString, mI->Render->GetNumDiscardedObjectsString(), 128);

		mTextSmallWhite.SetText	 (mFpsString);


		// ----- Input ----

		// Delta
		mDelta = mI->Render->GetFrameTime() / 1000.0f;


		// Cursor position
		mCursor.SetPosition ((float) mI->Input->GetMouseX(), (float) mI->Input->GetMouseY(), 3);


		// Change resolution to 640, 480 when pressing 1
		if (mI->Input->OnKeyPress (STX_KEY_1))
		{

			if (!mI->Render->Reset ("Alien BenchMark", 640, 480, 32, 0, 1)) return;

			mCameraGui.SetPosition (mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);

			mCameraTerrain.SetPosition	(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);

		}


		// Change resolution to 800, 600 when pressing 2
		if (mI->Input->OnKeyPress (STX_KEY_2))
		{

			if (!mI->Render->Reset ("Alien BenchMark", 800, 600, 32, 0, 1)) return;

			mCameraGui.SetPosition (mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);

			mCameraTerrain.SetPosition	(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);

		}


		// Change resolution to 1024, 768 when pressing 3
		if (mI->Input->OnKeyPress (STX_KEY_3))
		{

			if (!mI->Render->Reset ("Alien BenchMark", 1024, 768, 32, 0, 1)) return;

			mCameraGui.SetPosition (mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);

			mCameraTerrain.SetPosition	(mI->Window->GetWidth () / 2, mI->Window->GetHeight() / 2);

		}


		// Toogle full screen when pressing "space"
		if (mI->Input->OnKeyPress (STX_KEY_SPACE)) mI->Render->ToggleFullScreen();


		// Camera Zoom in / out
		if (mI->Input->OnMouseButtonPress	(STX_MBUTTON_WHEELUP))	mZoom += 0.1f;

		if (mI->Input->OnMouseButtonPress	(STX_MBUTTON_WHEELDOWN))	mZoom -= 0.1f;
		

		// Camera angle
		if (mI->Input->IsMouseButtonPressed (STX_MBUTTON_LEFT))		mAngle += mSpeedRotation * mDelta;

		if (mI->Input->IsMouseButtonPressed (STX_MBUTTON_RIGHT))		mAngle -= mSpeedRotation * mDelta;
		

		// ----- Updating entities attributes  -----

		// Zooming and rotating the camera
		if (mZoom < 0.1f) mZoom =  0.1f;

		mCameraTerrain.SetAngle (mAngle);

		mCameraTerrain.SetZoom (mZoom);


		// Camera translation
		if (mI->Input->GetMouseX() > mI->Window->GetWidth () - MOVE_TERRAIN_BY_MOUSE_AREA)		mCameraTerrain.SetPosition (mCameraTerrain.GetPosX() + (int) CAMERA_DISPLACE, mCameraTerrain.GetPosY());

		if (mI->Input->GetMouseX() < MOVE_TERRAIN_BY_MOUSE_AREA)								mCameraTerrain.SetPosition (mCameraTerrain.GetPosX() - (int) CAMERA_DISPLACE, mCameraTerrain.GetPosY());

		if (mI->Input->GetMouseY() > mI->Window->GetHeight ()- MOVE_TERRAIN_BY_MOUSE_AREA)		mCameraTerrain.SetPosition (mCameraTerrain.GetPosX(), mCameraTerrain.GetPosY() + (int) CAMERA_DISPLACE);

		if (mI->Input->GetMouseY() < MOVE_TERRAIN_BY_MOUSE_AREA)								mCameraTerrain.SetPosition (mCameraTerrain.GetPosX(), mCameraTerrain.GetPosY() - (int) CAMERA_DISPLACE);


		// ----- Render  -----

		mI->Render->BeginScene ();

		mI->Render->ClearViewPort (0, 0, 0);

		
		// --- Layer 0 (terrain) ---

		mI->Render->ResetNumDiscardedObjects();

		mI->Render->ResetNumRenderedObject();

		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		mI->Render->SetCamera2d (&mCameraTerrain);

		mI->Entity2dManager->RenderEntities2d (0);


		// --- Layer 1 (gui) ---

		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		mI->Render->SetCamera2d (&mCameraGui);

		mI->Render->BlitFillRectangle (10, 10, 240, 80, 128, 128, 128, 128);

		mI->Render->BlitRectangle (10, 10, 240, 80, 255, 255, 255, 128);

		mI->Entity2dManager->RenderEntities2d (1);


		mI->Render->EndScene ();
	
	}


	// ----- Free -----

	//mI->End ();
	//delete [] mUfos;
}
};
GameLogic gl;
IGameLogic* glp=&gl;
int init(const char* aTitle){return glp->init(aTitle);}
void render(){return glp->render();}
int IndieLib()
{
	glp->init("c_01_Alien_BenchMark");
    mI->Input->Update ();
    while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();

	delete [] mUfos;

	return 0;
}

