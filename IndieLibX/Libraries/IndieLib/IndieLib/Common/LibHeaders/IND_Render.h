/*****************************************************************************************
 * File: IND_Render.h
 * Desc: Render class using Direct3D
 *
 * Divided in:
 * - IND_Render.cpp
 * - RenderTransform2d.cpp
 * - RenderTransform3d.cpp
 * - RenderTransformCommon.cpp
 * - RenderObject2d.cpp
 * - RenderObject3d.cpp
 * - RenderPrimitive2d.cpp
 * - RenderText2d.cpp
 * - RenderCollision2d.cpp
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com)

This library is free software; you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/


#ifndef _IND_RENDER_H_
#define _IND_RENDER_H_

// ----- Includes -----
#include <Defines.h>
#include <IND_Window.h>
#include <IND_FontManager.h>
#include <IND_SurfaceManager.h>
#include <IND_3dMeshManager.h>
#include <IND_AnimationManager.h>
#include <IND_FontManager.h>
#include <IND_Timer.h>
#include <IND_Font.h>
#include <IND_3dMesh.h>
#include <IND_Animation.h>
#include <IND_Camera2d.h>
#include <IND_Camera3d.h>

// ----- Libs -----

//include <Framework3/IRenderer.h>

struct aiScene;
// ----- Defines -----

#define MAX_PIXELS 2048


// --------------------------------------------------------------------------------
//									  IND_Render
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Render IND_Render
\ingroup Main
Render class for setting up the render area, click in ::IND_Render to see all the methods of this class.
*/
/*@{*/
int Getbbb();
int Getmmm();

/*!
The methods of this class are used for preparing the render area, viewport, etc.

Important: You can use the methods IND_Render::GetDirect3d() and  IND_Render::GetDevice()
in order to get the Direct3d pointers. This can be useful if you want to extend the features of the
engine using Direct3d directly.
*/
struct INDIELIBAPI structPixel
{
	// Position
	float mX, mY, mZ;

	// Color
	D3DXFROMWINECOLOR mColor;
};
typedef struct structPixel PIXEL;

class INDIELIBAPI CIndieLib;
class INDIELIBAPI IND_Render
{
public:

	// ----- Init/End -----

	IND_Render		(): mOk(false)	{ }
	~IND_Render		()				{ End(); }

	bool	Init				(int pWidth, int pHeight, int argc, char *argv[]);
	bool	Init				(IND_Window *pWindow, tRenderer rtb=eGLGLSL_1_1, int argc=0, char *argv[]=0);
	bool	Init				(
		//LPDIRECT3D9 pDirect3d, IDirect3DDevice9 *pD3dDevice
		//RendererHLSLCg*
		IRenderer *r
		);
	bool	Reset				(const char *pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);
	bool	ToggleFullScreen	();
	void	End					();
	bool	IsOk				() { return mOk; }

	// ----- Public methods -----

	INLINE void BeginScene				();
	INLINE void EndScene				();
	//INLINE void ShowFpsInWindowTitle	();
#ifndef SWIG
	void ShowFpsInWindowTitle (const char* str);
#endif
	//INLINE void ShowStrInWindowTitle (const char* str);

	// ----- Viewports and cameras -----

	/** @name Viewport clearing (both for 2d and 3d viewports)
	*
	*/
	//@{
	INLINE void	ClearViewPort			(unsigned char pR,
										 unsigned char pG,
										 unsigned char pB);
	//@}

	/** @name Viewport and camera 2d
	*
	*/
	//@{
	INLINE bool SetViewPort2d			(int pX,
										 int pY,
										 int pWidth,
										 int pHeight);

	INLINE void SetCamera2d				(IND_Camera2d *pCamera2d);
	//@}
#if defined( BUILD_MESHRENDERER) 

	/** @name Viewport and camera 3d
	*
	*/
	//@{
	INLINE bool SetViewPort3d			(int pX,
										int pY,
										int pWidth,
										int pHeight);

	INLINE void SetCamera3d				(IND_Camera3d *pCamera3d);
	//@}
#endif
	// ----- Render Primitive 2d -----

	INLINE bool SetAntialiasing			(bool pSwitch);

	/** @name Bliting primitives (you can also use IND_Entity2d instead this methods)
	*
	*/
	//@{
	INLINE void BlitPixel				(/*int ab,*/int pX,
										int pY,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA);

	INLINE void BlitLine				(/*int ab,*/int pX1,
										int pY1,
										int pX2,
										int pY2,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA);

	INLINE void BlitRectangle			(/*int ab,*/int pX1,
										int pY1,
										int pX2,
										int pY2,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA);

	INLINE void BlitFillRectangle		(/*int ab,*/int pX1,
										int pY1,
										int pX2,
										int pY2,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA);

	INLINE void BlitColoredTriangle		(/*int ab,*/int pX1,
										int pY1,
										int pX2,
										int pY2,
										int pX3,
										int pY3,
										unsigned char pR1, unsigned char pG1, unsigned char pB1,
										unsigned char pR2, unsigned char pG2, unsigned char pB2,
										unsigned char pR3, unsigned char pG3, unsigned char pB3,
										unsigned char pA);

	INLINE bool BlitPoly2d				(/*int ab,*/IND_Point *pPixel,
										int pNumLines,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA);

	INLINE bool BlitRegularPoly			(/*int ab,*/int pX,
										int pY,
										int pRadius,
										int pN,
										float pAngle,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA);

	//@}


	// ----- Advanced transformations -----

	/** @name Advanced transformations methods (it's preferible to use IND_Entity2d and IND_Entity3d instead)
	*
	*/
	//@{
	INLINE void SetTransform2d			(int pX,
										int  pY,
										float pAngleX,
										float pAngleY,
										float pAngleZ,
										float pScaleX,
										float pScaleY,
										int pAxisCalX,
										int pAxisCalY,
										bool pMirrorX,
										bool pMirrorY,
										int pWidth,
										int pHeight,
										IND_Matrix *pMatrix);
	void SetTransform2d			(float pX,
										float  pY,
										float pAngleX,
										float pAngleY,
										float pAngleZ,
										float pScaleX,
										float pScaleY,
										float pAxisCalX,
										float pAxisCalY,
										bool pMirrorX,
										bool pMirrorY,
										float pWidth,
										float pHeight,
										IND_Matrix *pMatrix){SetTransform2d			(int (pX),
										int  (pY),
										pAngleX,
										pAngleY,
										pAngleZ,
										pScaleX,
										pScaleY,
										int (pAxisCalX),
										int (pAxisCalY),
										pMirrorX,
										pMirrorY,
										int (pWidth),
										int (pHeight),
										pMatrix);}

	INLINE void SetTransform3d			(float pX,
										float  pY,
										float  pZ,
										float pAngleX,
										float pAngleY,
										float pAngleZ,
										float pScaleX,
										float pScaleY,
										float pScaleZ,
										IND_Matrix *pMatrix);

	INLINE int SetRainbow2d			(IND_Type pType,
										bool pCull,
										bool pMirrorX,
										bool pMirrorY,
										IND_Filter pFilter,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA,
										unsigned char pFadeR,
										unsigned char pFadeG,
										unsigned char pFadeB,
										unsigned char pFadeA,
										IND_BlendingType pSo,
										IND_BlendingType pDs);

	INLINE void SetRainbow3d			(bool pCull,
										bool pFlipNormals,
										IND_Filter pFilter,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA,
										unsigned char pFadeR,
										unsigned char pFadeG,
										unsigned char pFadeB,
										unsigned char pFadeA,
										IND_BlendingType pSo,
										IND_BlendingType pDs);

	INLINE void LookAt					(float pEyeX, float pEyeY, float pEyeZ,
										float pLookAtX, float pLookAtY, float pLookAtZ,
										float pUpX, float pUpY, float pUpZ);

	INLINE void PerspectiveFov			(float pFov, float pAspect, float pNearClippingPlane, float pFarClippingPlane);

	INLINE void PerspectiveOrtho		(float pWidth, float pHeight, float pNearClippingPlane, float pFarClippingPlane);
	//@}


	// ----- Render Object 2d -----

	/** @name Advanced 2d bliting methods (it's preferible to use IND_Entity2d together with IND_Entity2dManager instead)
	*
	*/
	//@{
	void DrawTexture(TextureID id);

	INLINE void BlitSurface				(/*int ab,*/IND_Surface *pSu);

	INLINE void BlitGrid				(/*int ab,*/IND_Surface *pSu, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);

	INLINE bool BlitRegionSurface		(/*int ab,*/IND_Surface *pSu,
										int pX,
										int pY,
										int pWidth,
										int pHeight);

	INLINE bool BlitWrapSurface			(/*int ab,*/IND_Surface *pSu,
										int pWidth,
										int pHeight,
										float pUDisplace,
										float pVDisplace);

	INLINE int BlitAnimation			(/*int ab,*/IND_Animation *pAn,
										int pSequence,
										int pX, int pY,
										int pWidth, int pHeight,
										bool pToggleWrap,
										float pUDisplace,
										float pVDisplace);

	// ------ Render Text 2d -----

	INLINE void BlitText				(/*int ab,*/IND_Font *pFo,
										char *pText,
										int pX,
										int pY,
										int pOffset,
										int pLineSpacing,
										unsigned char pR,
										unsigned char pG,
										unsigned char pB,
										unsigned char pA,
										unsigned char pFadeR,
										unsigned char pFadeG,
										unsigned char pFadeB,
										unsigned char pFadeA,
										IND_Filter pFilter,
										IND_BlendingType pSo,
										IND_BlendingType pDs,
										IND_Align pAlign);

	//@}


	// ----- Render Object 3d -----

	/** @name Advanced 3d bliting methods (it's preferible to use IND_Entity3d together with  IND_Entity3dManager instead)
	*
	*/
	//@{
#if defined( BUILD_MESHRENDERER)
	INLINE void Blit3dMesh				(/*int ab,*/IND_3dMesh *p3dMesh);
	INLINE void Set3dMeshSequence		(/*int ab,*/IND_3dMesh *p3dMesh, unsigned int pIndex);
#endif
	//@}

	// ----- Atributos -----

	/** @name Gets
	*
	*/
	//@{
	//! This function returns the x position of the actual viewport
	int GetViewPortX			()		{ return mInfo.mViewPortX;			}
	//! This function returns the y position of the actual viewport
	int GetViewPortY			()		{ return mInfo.mViewPortY;			}
	//! This function returns the width position of the actual viewport
	int GetViewPortWidth		()		{ return mInfo.mViewPortWidth;		}
	//! This function returns the width position of the actual viewport
	int GetViewPortHeight		()		{ return mInfo.mViewPortHeight;		}
	//! This function returns the actual version of Direct3d that is using IndieLib.
    const char *GetVersion			()		{ return(const char *) mInfo.mVersion.c_str();			}
	//! This function returns the name of the graphic card vendor.
    const char *GetVendor				()		{ return(const char *) mInfo.mVendor.c_str();				}
	//!	This function returns the name of the graphic card.
    const char *GetRenderer			()		{ return(const char *) mInfo.mRenderer.c_str();			}
	//! This function returns the maximum texture size allowed by the graphic card.
	int  GetMaxTextureSize		()		{ return mInfo.mMaxTextureSize;		}
	//! This function returns the pointer to Direct3d.
//	LPDIRECT3D9 GetDirect3d		()		{ return mInfo.mDirect3d;			}
	//! This function returns the pointer to the Direct3d device.
	IRenderer *GetDevice	()		{ return mInfo.mDevice;				}
	//! This function returns the actual fps (frames per second) in a string of chars.
	//! @param _String IN-OUT buffer capable to hold string representation of integer FPS value
	void *GetFpsString			(char* _String,int n);
	//! This function returns the actual fps (frames per second) in an int
	int GetFpsInt				()		{ return mLastFps;					}
	//! This function returns a pointer to the IND_Window object where the render has been created
	IND_Window *GetWindow		()		{ return mWindow;					}
	//! This function returns in miliseconds the time that took the previous frame.
	/*!
	It is very useful to indicate transformations along the timeline.

	For example, making Position_X += Speed * Render.GetFrameTime() / 1000.0f we would get
	one x position that would go moving along the time with a given speed.
	*/
	float GetFrameTime			()		{ return mLast;						}
	//@}

	char *GetNumRenderedObjectsString		();
	char *GetNumDiscardedObjectsString		();

	int GetNumRenderedObjectsInt			()		{ return mNumRenderedObjects;		};
	int GetNumDiscardedObjectsInt			()		{ return mNumDiscardedObjects;		};

	void ResetNumRenderedObject				()		{ mNumRenderedObjects = 0; }
	void ResetNumDiscardedObjects			()		{ mNumDiscardedObjects = 0; }

private:

	// ----- Private methods -----

	bool mOk;

	// ----- Objects -----

	IND_Window *mWindow;

	// ----- Vars -----

	int mNumRenderedObjects;
	int mNumDiscardedObjects;

	struct StrucutFrustumPlane
	{
		D3DXFROMWINEVECTOR3 mNormal;
		float mDistance;

		INLINE float DistanceToPoint (D3DXFROMWINEVECTOR3 &pPnt)
		{
			return D3DXFROMWINEVec3Dot (&mNormal, &pPnt) + mDistance;
		}

		INLINE void Normalise()
		{
			float denom = 1 / sqrt ((mNormal.x * mNormal.x) + (mNormal.y * mNormal.y) +  (mNormal.z * mNormal.z));
			mNormal.x = mNormal.x * denom;
			mNormal.y = mNormal.y * denom;
			mNormal.z = mNormal.z * denom;
			mDistance = mDistance * denom;
		}
	};

	StrucutFrustumPlane mFrustumPlanes[6];

	// Timer
	float mLastTime;
	float mLast;


	// Fps
	int mFpsCounter;
	float mCurrentTimeFps;
	float mLastTimeFps;
	int mLastFps;

	// ----- Primitives vertices -----

	// Temporal buffer of pixels for drawing primitives
	PIXEL mPixels [MAX_PIXELS];

	// ----- Vertex array -----

	// Temporal buffer of vertices for drawing regions of an IND_Surface
	CUSTOMVERTEX2D mVertices2d [MAX_PIXELS];

	// ----- Info -----
#ifndef SWIG
	struct infoStruct
	{
		infoStruct()
	{
		mFBWidth=0;
		mFBHeight=0;
		mViewPortX=0;
		mViewPortY=0;
		mViewPortWidth=0;
		mViewPortHeight=0;
		mAntialiasing=false;
		mVersion="";// [0]='\0';
		mVendor="";// [0]='\0';
		mRenderer="";// [0]='\0';
		mMaxTextureSize=0;
		mVertexShaderVersion=0;
		mPixelShaderVersion=0;
		mSoftwareVertexProcessing=false;
		mDevice=0;
	}
		int mFBWidth;
		int mFBHeight;
		int mViewPortX;
		int mViewPortY;
		int mViewPortWidth;
		int mViewPortHeight;
		bool mAntialiasing;
		std::string mVersion;// [1024] ;
		std::string mVendor;// [1024];
		std::string mRenderer;// [1024];
		int mMaxTextureSize;
		__DWORD__ mVertexShaderVersion;
		__DWORD__ mPixelShaderVersion;
		bool mSoftwareVertexProcessing;
#if 0
		LPDIRECT3D9	mDirect3d;
		IDirect3DDevice9 *mDevice;
#else
		//RendererHLSLD3D9*
			IRenderer *mDevice;
#endif
	};
public:
	struct infoStruct mInfo;
#endif
private:
	// ----- Private methods -----

	bool Direct3DInit				(int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen, tRenderer rtb=eGLGLSL_1_1, const char* path=".", int argc=0, char *argv[]=0);
	bool Direct3dReset							(int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);

	bool CreateRender							(IND_Window *pWindow, tRenderer rtb=eGLGLSL_1_1
							   ,const char* path="."
							  , int argc=0, char *argv[]=0);

	void GetInfo								();
	void WriteInfo								();

	INLINE void	FillVertex2d					(CUSTOMVERTEX2D *pVertex2d,
												int pX,
												int pY,
												float pU,
												float pV);

	void ResetTimer								();
#if defined( BUILD_MESHRENDERER)
	// ----- Render Object 3d -----
	void FrameMove								(IND_3dMesh *p3dMesh, float elapsedTime, const D3DXFROMWINEMATRIX *matWorld);
#if 0
	void UpdateFrameMatrices					(const D3DXFROMWINEFRAME *frameBase, const D3DXFROMWINEMATRIX *parentMatrix);
	void DrawFrame								(LPD3DXFROMWINEFRAME frame);
	void DrawMeshContainer						(LPD3DXFROMWINEMESHCONTAINER meshContainerBase, LPD3DXFROMWINEFRAME frameBase);
#else
//	void UpdateFrameMatrices					(const aiScene*frameBase, const D3DXFROMWINEMATRIX *parentMatrix);
	void DrawFrame								(IND_3dMesh *p3dMesh);
	void DrawMeshContainer						(IND_3dMesh *p3dMesh);
#endif
#endif
	// ----- Primitives -----

	INLINE bool	IsBlockRightOfViewPort			(D3DXFROMWINEVECTOR4 pP1, D3DXFROMWINEVECTOR4 pP2, D3DXFROMWINEVECTOR4 pP3, D3DXFROMWINEVECTOR4 pP4);
	INLINE bool	IsBlockLeftOfViewPort			(D3DXFROMWINEVECTOR4 pP1, D3DXFROMWINEVECTOR4 pP2, D3DXFROMWINEVECTOR4 pP3, D3DXFROMWINEVECTOR4 pP4);
	INLINE bool	IsBlockDownOfViewPort			(D3DXFROMWINEVECTOR4 pP1, D3DXFROMWINEVECTOR4 pP2, D3DXFROMWINEVECTOR4 pP3, D3DXFROMWINEVECTOR4 pP4);
	INLINE bool	IsBlockUpOfViewPort				(D3DXFROMWINEVECTOR4 pP1, D3DXFROMWINEVECTOR4 pP2, D3DXFROMWINEVECTOR4 pP3, D3DXFROMWINEVECTOR4 pP4);

	INLINE void FillPixel						(PIXEL *pPixel,
												int pX,
												int pY,
												unsigned char pR,
												unsigned char pG,
												unsigned char pB);

	INLINE void SetForPrimitive					(unsigned char pA);

	// ------ Fonts -----

	INLINE int GetLongInPixels					(IND_Font *pFo, char *pText, int pPos, int pOffset);

	INLINE void SetTranslation					(int pX, int pY, D3DXFROMWINEMATRIX *pMatWorld, D3DXFROMWINEMATRIX *pMatTraslation);

	// ----- Grid -----

	INLINE void BlitGridLine					(/*int ab,*/int pPosX1, int pPosY1, int pPosX2, int pPosY2,  unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA, D3DXFROMWINEMATRIX pWorldMatrix);
	INLINE void BlitGridQuad					(/*int ab,*/int pAx, int pAy,
												int pBx, int pBy,
												int pCx, int pCy,
												int pDx, int pDy,
												unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA,
												D3DXFROMWINEMATRIX pWorldMatrix);

	// ----- Collisions  -----

	INLINE void BlitCollisionCircle				(/*int ab,*/int pPosX, int pPosY, int pRadius, float pScale, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA, IND_Matrix pWorldMatrix);
	INLINE void BlitCollisionLine				(/*int ab,*/int pPosX1, int pPosY1, int pPosX2, int pPosY2,  unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA, IND_Matrix pIndWorldMatrix);

	INLINE bool	IsTriangleToTriangleCollision	(BOUNDING_COLLISION *pB1, IND_Matrix pMat1, BOUNDING_COLLISION *pB2, IND_Matrix pMat2);
	INLINE bool	IsCircleToCircleCollision		(BOUNDING_COLLISION *pB1, IND_Matrix pMat1, float pScale1, BOUNDING_COLLISION *pB2, IND_Matrix pMat2, float pScale2);
	INLINE bool	IsCircleToTriangleCollision		(BOUNDING_COLLISION *pB1, IND_Matrix pMat1, float pScale1, BOUNDING_COLLISION *pB2, IND_Matrix pMat2);


	INLINE bool IsTriangleToTriangleCollision	(D3DXFROMWINEVECTOR2 pA1,
												D3DXFROMWINEVECTOR2 pB1,
												D3DXFROMWINEVECTOR2 pC1,
												D3DXFROMWINEVECTOR2 pA2,
												D3DXFROMWINEVECTOR2 pB2,
												D3DXFROMWINEVECTOR2 pC2);

	INLINE bool IsCircleToCircleCollision		(D3DXFROMWINEVECTOR2 pP1, int pRadius1,
												D3DXFROMWINEVECTOR2 pP2, int pRadius2);

	INLINE bool IsCircleToTriangleCollision		(D3DXFROMWINEVECTOR2 pPCenter, int pRadius1,
												D3DXFROMWINEVECTOR2 a2,
												D3DXFROMWINEVECTOR2 b2,
												D3DXFROMWINEVECTOR2 c2);

	INLINE double PointToLineDistance			(D3DXFROMWINEVECTOR2 pA, D3DXFROMWINEVECTOR2 pB, D3DXFROMWINEVECTOR2 pC, bool pIsSegment);
	INLINE double Distance						(D3DXFROMWINEVECTOR2 pA, D3DXFROMWINEVECTOR2 pB);
	INLINE int	Cross3							(D3DXFROMWINEVECTOR2 pA, D3DXFROMWINEVECTOR2 pB, D3DXFROMWINEVECTOR2 pC);
	INLINE int	Dot3							(D3DXFROMWINEVECTOR2 pA, D3DXFROMWINEVECTOR2 pB, D3DXFROMWINEVECTOR2 pC);

	INLINE bool IsPointInsideTriangle			(D3DXFROMWINEVECTOR2 p,
												D3DXFROMWINEVECTOR2 a,
												D3DXFROMWINEVECTOR2 b,
												D3DXFROMWINEVECTOR2 c);

	INLINE bool IsSegmentIntersection			(D3DXFROMWINEVECTOR2 a,
												D3DXFROMWINEVECTOR2 b,
												D3DXFROMWINEVECTOR2 c,
												D3DXFROMWINEVECTOR2 d);

	INLINE void GetD3DMatrix					(IND_Matrix pMatrix, D3DXFROMWINEMATRIX *pD3DMatrix);

	// ----- Culling -----

	INLINE void	CalculeFrustumPlanes			();
	INLINE __WORD__ CullFrustumBox					(const D3DXFROMWINEVECTOR3 &pAABBMin, const D3DXFROMWINEVECTOR3 &pAABBMax);
	INLINE void CalculateBoundingRectangle		(D3DXFROMWINEVECTOR3 *mP1, D3DXFROMWINEVECTOR3 *mP2, D3DXFROMWINEVECTOR3 *mP3, D3DXFROMWINEVECTOR3 *mP4);
	INLINE void MinAndMax4						(int p1,
												int p2,
												int p3,
												int p4,
												int *pMax,
												int *pMin);
	INLINE void Transform4Vertices				(float pX1, float pY1,
												 float pX2, float pY2,
												 float pX3, float pY3,
												 float pX4, float pY4,
												 D3DXFROMWINEVECTOR4 *mP1Res,
												 D3DXFROMWINEVECTOR4 *mP2Res,
												 D3DXFROMWINEVECTOR4 *mP3Res,
												 D3DXFROMWINEVECTOR4 *mP4Res);
	// ----- Init / End -----

	void DestroyD3DWindow						();
	void InitAttribRender						();
	void InitVars								();
	void FreeVars								();

	// ----- Friends -----

	friend class IND_Entity2dManager;
	friend class IND_Input;
};
/*@}*/

#endif // _IND_RENDER_H_
