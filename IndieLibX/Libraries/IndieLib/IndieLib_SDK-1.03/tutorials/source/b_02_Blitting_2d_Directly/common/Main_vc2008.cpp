#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
int mWidth, mHeight;
	float mAngle = 0.0f;
	float mHotSpotX, mHotSpotY;
	int mAxisCalX, mAxisCalY;
	//////////IND_Matrix mMatrix;
	float	mSpeed = 50;
	float	mDelta;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceDraco;
		IND_Surface mSurfaceGem;
		IND_Surface mSurfaceBug;
		IND_Animation mAnimationUfo;
		IND_Matrix mMatrix;
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

	// Loading draco
	//////////IND_Matrix mMatrix;
	if (!mI->SurfaceManager->Add (&mSurfaceDraco, "/draco.png", IND_ALPHA, IND_32)) return 0;

	// Loading gem (this image has a blue rectangle surronding it)
	//////////IND_Matrix mMatrix;
	if (!mI->SurfaceManager->Add (&mSurfaceGem, "/gem_squared.png", IND_ALPHA, IND_32)) return 0;

	// Loading bug
	//////////IND_Matrix mMatrix;
	if (!mI->SurfaceManager->Add (&mSurfaceBug, "/Enemy_Bug.png", IND_ALPHA, IND_32)) return 0;

	// ----- Animation loading -----

	//////////IND_Matrix mMatrix;
	if (!mI->AnimationManager->AddToSurface (&mAnimationUfo, "/animations/ufo.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Main Loop -----
	STX_CATCH;
	return 0;

	

	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Delta -----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;

		// ----- Render  -----

		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);


		// --------------------------------------------------------------------------------
		//					Blitting Draco directly (IND_Surface object)
		// --------------------------------------------------------------------------------

		// 1) We apply the world space transformation (translation, rotation, scaling).
		// If you want to recieve the transformation in a single matrix you can pass
		// and IND_Matrix object by reference.

		mWidth = mSurfaceDraco.GetWidth();
		mHeight = mSurfaceDraco.GetHeight();
		mI->Render->SetTransform2d	(50,				// x pos
									70,					// y pos
									0,					// Angle x
									0,					// Angle y
									0,					// Angle z
									1,					// Scale x
									1,					// Scale y
									0,					// Axis cal x
									0,					// Axis cal y
									0,					// Mirror x
									0,					// Mirror y
									mWidth,				// Width
									mHeight,			// Height
									&mMatrix);			// Matrix in wich the transformation will be applied (optional)

		// 2) We apply the color, blending and culling transformations.
		mI->Render->SetRainbow2d	(IND_ALPHA,			// IND_Type
									1,					// Back face culling 0/1 => off / on
									0,					// Mirror x
									0,					// Mirror y
									IND_FILTER_LINEAR,	// IND_Filter
									255,				// R Component	for tinting
									255,				// G Component	for tinting
									255,				// B Component	for tinting
									255,				// A Component	for tinting
									0,					// R Component	for fading to a color
									0,					// G Component	for fading to a color
									0,					// B Component	for fading to a color
									255,				// Amount of fading
									IND_SRCALPHA,		// IND_BlendingType (source)
									IND_INVSRCALPHA);	// IND_BlendingType (destination)


		// 3) Blit the IND_Surface
		mI->Render->BlitSurface	(&mSurfaceDraco);


		// --------------------------------------------------------------------------------
		//		Blitting Bug directly (IND_Surface object with a HotSpot and rotating)
		// --------------------------------------------------------------------------------

		// 1) We apply the world space transformation (translation, rotation, scaling).
		// If you want to recieve the transformation in a single matrix you can pass
		// and IND_Matrix object by reference.

		mAngle += mSpeed *mDelta;
		mHotSpotX = 0.5f;
		mHotSpotY = 0.5f;
		mWidth = mSurfaceBug.GetWidth();
		mHeight = mSurfaceBug.GetHeight();
		mAxisCalX = (int) (mHotSpotX * mWidth * -1);
		mAxisCalY = (int) (mHotSpotY * mHeight * -1);

		mI->Render->SetTransform2d	(500,				// x pos
									100,				// y pos
									0,					// Angle x
									0,					// Angle y
									mAngle,				// Angle z
									1,					// Scale x
									1,					// Scale y
									mAxisCalX,			// Axis cal x
									mAxisCalY,			// Axis cal y
									0,					// Mirror x
									0,					// Mirror y
									mWidth,				// Width
									mHeight,			// Height
									&mMatrix);			// Matrix in wich the transformation will be applied (optional)

		// 2) We apply the color, blending and culling transformations.
		mI->Render->SetRainbow2d	(IND_ALPHA,			// IND_Type
									1,					// Back face culling 0/1 => off / on
									0,					// Mirror x
									0,					// Mirror y
									IND_FILTER_LINEAR,	// IND_Filter
									255,				// R Component	for tinting
									255,				// G Component	for tinting
									255,				// B Component	for tinting
									255,				// A Component	for tinting
									0,					// R Component	for fading to a color
									0,					// G Component	for fading to a color
									0,					// B Component	for fading to a color
									255,				// Amount of fading
									IND_SRCALPHA,		// IND_BlendingType (source)
									IND_INVSRCALPHA);	// IND_BlendingType (destination)


		// 3) Blit the IND_Surface
		mI->Render->BlitSurface	(&mSurfaceBug);


		// --------------------------------------------------------------------------------
		//				Blitting a region of Gem directly (IND_Surface region)
		// --------------------------------------------------------------------------------

		// 1) We apply the world space transformation (translation, rotation, scaling).
		// If you want to recieve the transformation in a single matrix you can pass
		// and IND_Matrix object by reference.

		mWidth = mSurfaceGem.GetWidth();
		mHeight = mSurfaceGem.GetHeight();
		mI->Render->SetTransform2d	(600,				// x pos
									200,				// y pos
									0,					// Angle x
									0,					// Angle y
									0,					// Angle z
									1,					// Scale x
									1,					// Scale y
									0,					// Axis cal x
									0,					// Axis cal y
									0,					// Mirror x
									0,					// Mirror y
									mWidth,				// Width
									mHeight,			// Height
									&mMatrix);			// Matrix in wich the transformation will be applied (optional)

		// 2) We apply the color, blending and culling transformations.
		mI->Render->SetRainbow2d	(IND_ALPHA,			// IND_Type
									1,					// Back face culling 0/1 => off / on
									0,					// Mirror x
									0,					// Mirror y
									IND_FILTER_LINEAR,	// IND_Filter
									255,				// R Component	for tinting
									255,				// G Component	for tinting
									255,				// B Component	for tinting
									255,				// A Component	for tinting
									0,					// R Component	for fading to a color
									0,					// G Component	for fading to a color
									0,					// B Component	for fading to a color
									255,				// Amount of fading
									IND_SRCALPHA,		// IND_BlendingType (source)
									IND_INVSRCALPHA);	// IND_BlendingType (destination)


		// 3) Blit the IND_Surface
		mI->Render->BlitRegionSurface (&mSurfaceGem, 10, 10, 70, 70);


		// --------------------------------------------------------------------------------
		//				Blitting a tiled region of Gem directly (IND_Surface wrap)
		// --------------------------------------------------------------------------------

		// 1) We apply the world space transformation (translation, rotation, scaling).
		// If you want to recieve the transformation in a single matrix you can pass
		// and IND_Matrix object by reference.

		mWidth = mSurfaceGem.GetWidth();
		mHeight = mSurfaceGem.GetHeight();
		mI->Render->SetTransform2d	(500,				// x pos
									350,				// y pos
									0,					// Angle x
									0,					// Angle y
									0,					// Angle z
									1,					// Scale x
									1,					// Scale y
									0,					// Axis cal x
									0,					// Axis cal y
									0,					// Mirror x
									0,					// Mirror y
									mWidth,				// Width
									mHeight,			// Height
									&mMatrix);			// Matrix in wich the transformation will be applied (optional)

		// 2) We apply the color, blending and culling transformations.
		mI->Render->SetRainbow2d	(IND_ALPHA,			// IND_Type
									1,					// Back face culling 0/1 => off / on
									0,					// Mirror x
									0,					// Mirror y
									IND_FILTER_LINEAR,	// IND_Filter
									255,				// R Component	for tinting
									255,				// G Component	for tinting
									255,				// B Component	for tinting
									255,				// A Component	for tinting
									0,					// R Component	for fading to a color
									0,					// G Component	for fading to a color
									0,					// B Component	for fading to a color
									255,				// Amount of fading
									IND_SRCALPHA,		// IND_BlendingType (source)
									IND_INVSRCALPHA);	// IND_BlendingType (destination)


		// 3) Blit the IND_Surface
		mI->Render->BlitWrapSurface (&mSurfaceGem, 200, 200, 0, 0);


		// --------------------------------------------------------------------------------
		//				Blitting a ufo animation directly (IND_Animation object)
		// --------------------------------------------------------------------------------

		// 1) We apply the world space transformation (translation, rotation, scaling).
		// If you want to recieve the transformation in a single matrix you can pass
		// and IND_Matrix object by reference.

		mWidth = mAnimationUfo.GetActualSurface (0)->GetWidth();
		mHeight= mAnimationUfo.GetActualSurface (0)->GetHeight();
		mI->Render->SetTransform2d	(650,				// x pos
									70,					// y pos
									0,					// Angle x
									0,					// Angle y
									0,					// Angle z
									1,					// Scale x
									1,					// Scale y
									0,					// Axis cal x
									0,					// Axis cal y
									0,					// Mirror x
									0,					// Mirror y
									mWidth,				// Width
									mHeight,			// Height
									&mMatrix);			// Matrix in wich the transformation will be applied (optional)

		// 2) We apply the color, blending and culling transformations.
		mI->Render->SetRainbow2d	(IND_ALPHA,			// IND_Type
									1,					// Back face culling 0/1 => off / on
									0,					// Mirror x
									0,					// Mirror y
									IND_FILTER_LINEAR,	// IND_Filter
									255,				// R Component	for tinting
									255,				// G Component	for tinting
									255,				// B Component	for tinting
									255,				// A Component	for tinting
									0,					// R Component	for fading to a color
									0,					// G Component	for fading to a color
									0,					// B Component	for fading to a color
									255,				// Amount of fading
									IND_SRCALPHA,		// IND_BlendingType (source)
									IND_INVSRCALPHA);	// IND_BlendingType (destination)


		// 3) Blit the IND_Animation looping
		if (mI->Render->BlitAnimation	(&mAnimationUfo, 0, 0, 0, 0, 0, 0, 0, 0) == -1)
			mAnimationUfo.SetActualFramePos (0, 0);

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
	glp->init("b_02_Blitting_2d_directly");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

