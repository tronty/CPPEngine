#define __MESHRENDERER__ 1


#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	int	mSequence = 0;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_3dMesh mMeshBones;
		IND_Light mLight;
		IND_Matrix mMatrix;
		IND_Camera3d mCamera3d;
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
	
//CIndieLib * mI =CIndieLib::Instance();
	if (!CIndieLib::Instance()->
		Init (aTitle
		
		)) return -1;
#endif
	// ----- 3d Mesh loading -----
#ifdef BUILD_MESHRENDERER
	// Bones loading
	//////IND_Matrix mMatrix;
	if (!mI->MeshManager->Add (&mMeshBones, "/skeleton/skeleton.x", "/skeleton")) return 0;

	// ----- Cameras ------

	mCamera3d=IND_Camera3d(0.0f, 0.9f, -3.0f);
	mCamera3d.SetAspect ((float) mI->Window->GetWidth () / mI->Window->GetHeight());
#endif
	// ----- Light -----

	//////IND_Matrix mMatrix;
	mI->LightManager->Add (&mLight, IND_DIRECTIONAL_LIGHT);
	mLight.SetPosition (0, 0, 0);
	mLight.SetColor (1.0f, 1.0f, 1.0f, 1.0f);
	mLight.SetDirection (0.0f, -0.3f, 0.5f);
	mLight.SetRange (1000.0f);

	// ----- Main Loop -----
	STX_CATCH;
	return 0;



	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Input -----
#ifdef BUILD_MESHRENDERER
		// Change sequence when pressing UP key
		if (mI->Input->OnKeyPress (STX_KEY_UP))
		{
			mSequence++;
			if (mSequence == mMeshBones.GetNumSequences()) mSequence = 0;
			mI->Render->Set3dMeshSequence (&mMeshBones, mSequence);
		}

		// Change sequence when pressing KEY up
		if (mI->Input->OnKeyPress (STX_KEY_DOWN))
		{
			mSequence--;
			if (mSequence < 0) mSequence = mMeshBones.GetNumSequences() - 1;
			mI->Render->Set3dMeshSequence (&mMeshBones, mSequence);
		}
#endif
		// ----- Render  -----

		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
#ifdef BUILD_MESHRENDERER
		mI->Render->SetViewPort3d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());
		mI->Render->SetCamera3d (&mCamera3d);
#endif
		mI->LightManager->Update ();
#ifdef BUILD_MESHRENDERER
		// 1) We apply the world space transformations (translation, rotation, scaling).
		// If you want to recieve the transformation in a single matrix you can pass
		// and IND_Matrix object by reference.
		//////IND_Matrix mMatrix;
		mI->Render->SetTransform3d	(0,					// x pos
									0,					// y pos
									0,					// z pos
									0,					// Angle x
									0,					// Angle y
									0,					// Angle z
									1,					// Scale x
									1,					// Scale y
									1,					// Scale z
									&mMatrix);			// Matrix in wich the transformation will be applied (optional)

		// 2) We apply the color, blending and culling transformations.
		mI->Render->SetRainbow3d	(1,					// Back face culling 0/1 => off / on
									0,					// Flip normals 0/1 => off / on
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

		// 3) Blit the 3d Mesh
		mI->Render->Blit3dMesh (&mMeshBones);
#endif
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
	glp->init("b_03_Blitting_3d_directly");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

