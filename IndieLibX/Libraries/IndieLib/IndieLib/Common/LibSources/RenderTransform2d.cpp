/*****************************************************************************************
 * File: RenderTransform2d.cpp
 * Desc: Transformations applied before blitting a 2d object usind D3D
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com)

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

// ----- Includes -----

#include <CIndieLib.h>


#include <IND_Render.h>

#define LOG_FNLN8
// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------


/*!
\b Parameters:

\arg \b pX              Upper left X coordinate of the Viewport
\arg \b pY              Upper left Y coordinate of the Viewport
\arg \b pWidth          Viewport width
\arg \b pHeight         Viewport height

\b Operation:

This function returns 1 (true) if a 2d ViewPort is created in the specified area.

The ViewPort is the drawing region. By default, IndieLib is initialized with a Viewport
that has the same area as the window. Using this method you can define different viewports before
calling to IND_Entity2dManager::RenderEntities2d().

After using this method, all the bliting methods will render the graphical objects inside the Viewport.
Objects or blocks drawn outside the viewport will be discarded.

This method returns 0 (false) if the user tries to create a Viewport outside the window area.
*/
#include <IND_Viewport.h>
IND_Viewport g_Viewport2d;
INLINE bool IND_Render::SetViewPort2d (int pX,
									 int pY,
									 int pWidth,
									 int pHeight)
{
	LOG_FNLN8;
	g_Viewport2d=IND_Viewport(pX,pY,pWidth,pHeight);
	// ----- If the region is outside the framebuffer return error -----

	LOG_FNLN8;
	if (pX +  pWidth > mInfo.mFBWidth)	return 0;
	LOG_FNLN8;
	if (pX < 0)							return 0;
	LOG_FNLN8;
	if (pY + pHeight > mInfo.mFBHeight)	return 0;
	LOG_FNLN8;
	if (pY < 0)							return 0;

	// ----- Viewport characteristics -----

	LOG_FNLN8;
	mInfo.mViewPortX	  = pX;
	LOG_FNLN8;
	mInfo.mViewPortY	  = pY;
	LOG_FNLN8;
	mInfo.mViewPortWidth  = pWidth;
	LOG_FNLN8;
	mInfo.mViewPortHeight = pHeight;

	// ----- World matrix -----

	LOG_FNLN8;
	D3DXFROMWINEMATRIX mMatWorld;
	D3DXFROMWINEMatrixIdentity(&mMatWorld);
	LOG_FNLN8;
	IRenderer::GetRendererInstance()->SetTransform (TS_WORLD, mMatWorld);

	// ----- View region  -----

	//D3DVIEWPORT9 mViewData = { pX, pY, pWidth, pHeight, -2048.0f, 2048.0f };
	//IRenderer::GetRendererInstance()->SetViewport (&mViewData);
	LOG_FNLN8;
	IRenderer::GetRendererInstance()->setViewport(pX, pY, pWidth, pHeight, -2048.0f, 2048.0f);

	// ----- 2d Sets -----
#if 0
	IRenderer::GetRendererInstance()->SetRenderState (D3DRS_NORMALIZENORMALS, false);
	IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ZENABLE, D3DZB_FALSE);
	IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ZWRITEENABLE, false);
	IRenderer::GetRendererInstance()->SetRenderState (D3DRS_LIGHTING, false);

	// ----- TextureOTL clamp on -----

	IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else
//???DBG_HALT;//noDepthTest
#endif
	LOG_FNLN8;
	return 1;
}


/*!
\b Parameters:

\arg \b pCamera2d				::IND_Camera2d object that defines a camera.

\b Operation:

This function sets a 2d camera. See the methods of ::IND_Camera2d for information on how you can manipulate the camera.
*/
#if BUILD_REPLAY
IND_Camera2d g_Camera2d;
#endif
INLINE void IND_Render::SetCamera2d (IND_Camera2d *pCamera2d)
{
#if BUILD_REPLAY
	g_Camera2d=*pCamera2d;
#endif
	D3DXFROMWINEMATRIX mTrans, mMatView, mMatProjection;
	D3DXFROMWINEMatrixIdentity (&mMatView);
	D3DXFROMWINEMatrixIdentity (&mMatProjection);

	// ----- Move camera to origin -----

	int pStrafe = pCamera2d->mX;
	int pFly = -pCamera2d->mY;
	pCamera2d->Strafe (pStrafe);
	pCamera2d->Fly (pFly);

	// ----- View matrix -----

	pCamera2d->mLook	= D3DXFROMWINEVECTOR3 (0.0f, 0.0f, 1.0f);
	pCamera2d->mUp		= D3DXFROMWINEVECTOR3 (0.0f, 1.0f, 0.0f);
	pCamera2d->mRight	= D3DXFROMWINEVECTOR3 (-1.0f, 0.0f, 0.0f);

	// ----- Rotation -----

	// Roll is rotation around the z axis (m_look)
	// Create a matrix that can carry out this rotation
	D3DXFROMWINEMATRIX rollMatrix;
	D3DXFROMWINEMatrixRotationAxis (&rollMatrix, &pCamera2d->mLook, D3DXFROMWINEToRadian(pCamera2d->mAngle));
	// To apply roll we rotate up and right about the look vector (using our roll matrix)
	D3DXFROMWINEVec3TransformCoord (&pCamera2d->mRight, &pCamera2d->mRight, &rollMatrix);
	D3DXFROMWINEVec3TransformCoord (&pCamera2d->mUp, &pCamera2d->mUp, &rollMatrix);

	// Build the view matrix from the transformed camera axis
	mMatView.m[0][0] = pCamera2d->mRight.x; mMatView.m[0][1] = pCamera2d->mUp.x; mMatView.m[0][2] = pCamera2d->mLook.x;
	mMatView.m[1][0] = pCamera2d->mRight.y; mMatView.m[1][1] = pCamera2d->mUp.y; mMatView.m[1][2] = pCamera2d->mLook.y;
	mMatView.m[2][0] = pCamera2d->mRight.z; mMatView.m[2][1] = pCamera2d->mUp.z; mMatView.m[2][2] = pCamera2d->mLook.z;

	mMatView.m[3][0] = -D3DXFROMWINEVec3Dot (&pCamera2d->mPos, &pCamera2d->mRight);
	mMatView.m[3][1] = -D3DXFROMWINEVec3Dot (&pCamera2d->mPos, &pCamera2d->mUp);
	mMatView.m[3][2] = -D3DXFROMWINEVec3Dot (&pCamera2d->mPos, &pCamera2d->mLook);

	// ---- 2d set ----

	D3DXFROMWINEMatrixRotationZ		(&mTrans, (float) PI);
	D3DXFROMWINEMatrixMultiply		(&mMatView, &mTrans, &mMatView);
	D3DXFROMWINEMatrixTranslation	(&mTrans, -0.5f, -0.5f, 0);
	D3DXFROMWINEMatrixMultiply		(&mMatView, &mTrans, &mMatView);

	// ---- Zoom ----

	if (pCamera2d->mZoom != 1.0f)
	{
		D3DXFROMWINEMatrixScaling (&mTrans, pCamera2d->mZoom, pCamera2d->mZoom, pCamera2d->mZoom);
		D3DXFROMWINEMatrixMultiply (&mMatView, &mMatView, &mTrans);
	}
	// ----- Set transformation -----
#if 0
	D3DXFROMWINEMatrixIdentity (&mMatView);
	D3DXFROMWINEMATRIX mMatScale;
	D3DXFROMWINEMatrixScaling (&mMatScale,1.0f/mInfo.mFBWidth,1.0f/mInfo.mFBHeight,1.0f);
/*
	D3DXFROMWINEMATRIX mMatTrans;
	D3DXFROMWINEMatrixTranslation (&mMatTrans,
		-mInfo.mFBWidth*mInfo.mFBWidth/2.0f,
		-mInfo.mFBHeight*mInfo.mFBHeight/2.0f,0.0f);
/#
	//mMatView=mMatTrans*mMatScale;
		D3DXFROMWINEMatrixMultiply	(&mMatView, &mMatView, &mMatTrans);
		D3DXFROMWINEMatrixMultiply (&mMatView, &mMatView, &mMatScale);
#/

	D3DXFROMWINEMatrixTranslation	(&mMatTrans, -0.5f, -0.5f, 0);
	D3DXFROMWINEMatrixMultiply		(&mMatView, &mMatTrans, &mMatView);
*/
	D3DXFROMWINEMatrixMultiply		(&mMatView, &mMatScale, &mMatView);

#endif
	IRenderer::GetRendererInstance()->SetTransform (TS_VIEW, mMatView);
	D3DXFROMWINEMatrixIdentity (&pCamera2d->w);
	pCamera2d->v=mMatView;

	// ----- Projection matrix -----

#if 1
	D3DXFROMWINEMatrixOrthoRH (
		&mMatProjection,
		(float) mInfo.mViewPortWidth,
		(float) mInfo.mViewPortHeight,
		-2048.0f,
		2048.0f);
#elif 0
    D3DXFROMWINEMatrixPerspectiveFovLH( &mMatProjection, D3DXFROMWINEToRadian( 45.0f ),
                                (float) mInfo.mViewPortWidth
								/(float) mInfo.mViewPortHeight,
								 0.001f, 20000.0f
								);
#else
	D3DXFROMWINEMatrixIdentity (&mMatProjection);
#endif
	IRenderer::GetRendererInstance()->SetTransform (TS_PROJECTION, mMatProjection);
	pCamera2d->p=mMatProjection;

	// ----- Move camera to the correct position -----

	pCamera2d->Strafe (-pStrafe);
	pCamera2d->Fly (-pFly);
}


/*!
\defgroup Graphical_Objects Bliting Surfaces, Animations, Fonts and setting transformations directly
\ingroup Advances
*/
/*@{*/

/*!
\b Parameters:

\arg \b pX							Translation in the X coordinate. The (0, 0) position is the
									upper-left corner of the Viewport
\arg \b pY							Translation in the Y coordinate. The (0, 0) position is the
									upper-left corner of the Viewport
\arg \b pAngleX						Rotation in the angle x in degrees
\arg \b pAngleY						Rotation in the angle y in degrees
\arg \b pAngleZ						Rotation in the angle z in degrees
\arg \b pScaleX						Scaling in the x coordinate. 1 for maintaining the original size
\arg \b pScaleY						Scaling in the y coordinate. 1 for maintaining the original size
\arg \b pAxisCalX, \b pAxisCalY		Parameters that indicates the displacement that the graphical
									object undergoes due to the HotSpot. If the HotSpot is not specified,
									the value should be 0 for both of them. All the transformation
									will be aplied from the upper-left corner of the object.
\arg \b pMirrorX					Horizontal mirroring. (true / false) = (activated / deactivated).
\arg \b pMirrorY					Vertical mirroring. (true / false) = (activated / deactivated).
\arg \b pWidth						Width of the graphical object that we are going to blit just after
									applying the transformation. You shoud use the GetWidth() method
									of the object.
\arg \b pHeight						Height of the graphical object that we are going to blit just after
									applying the transformation. You shoud use the GetHeight() method
									on the object.
\arg \b pMatrix						Pointer to a ::IND_Matrix matrix. In this parameter will be
									returned by reference the world matrix transformation that
									will be aplied to the graphical object. This matrix can be useful
									for advanced programmers that need the algebraic description
									of the transformation. It is possible to use the value 0
									if it not necessary to have this matrix information.
\b Operation:

This function sets the 2d transformation (translation, rotation, scaling, mirroring and hotspot)
of the following 2d graphical objects that will be rendered
by the engine. You should use this method before calling to any of the Bliting methods.

Remember that you can use IND_Entity2d object for applying 2d transformations to the graphical
objects without having to use this advanced method directly. This method is only useful for advanced
users with really concrete purposes.

Using this method is equivalent to using a combination of these methods:
- IND_Entity2d::SetAnimation()
- IND_Entity2d::SetSurface()
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetFont()
- IND_Entity2d::SetPosition()
- IND_Entity2d::SetAngleXYZ()
- IND_Entity2d::SetScale()
- IND_Entity2d::SetBackCull()
- IND_Entity2d::SetMirrorX()
- IND_Entity2d::SetMirrorY()
- IND_Entity2d::SetFilter()
- IND_Entity2d::SetHotSpot()
- IND_Entity2d::SetRegion()
- IND_Entity2d::ToggleWrap()
*/
INLINE void IND_Render::SetTransform2d	(int pX,
										int pY,
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
										IND_Matrix *pMatrix)
{
	// ----- World matrix initialization -----

	D3DXFROMWINEMATRIX mMatWorld, mMatZ, mMatX, mMatY, mMatTraslation, mMatScale;

	D3DXFROMWINEMatrixIdentity (&mMatWorld);
	IRenderer::GetRendererInstance()->SetTransform (TS_WORLD, mMatWorld);

	// ----- Transformation matrix creation -----

	// Mirroring (180º rotations)
	if (pMirrorX || pMirrorY)
	{
		if (pMirrorX && !pMirrorY)
		{
			D3DXFROMWINEMatrixTranslation (&mMatWorld, (float) -pWidth, 0, 0);
			D3DXFROMWINEMatrixRotationY (&mMatY, D3DXFROMWINEToRadian (180));
			D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatY);
		}

		if (!pMirrorX && pMirrorY)
		{
			D3DXFROMWINEMatrixTranslation (&mMatWorld, 0, (float) -pHeight, 0);
			D3DXFROMWINEMatrixRotationX (&mMatX, D3DXFROMWINEToRadian (180));
			D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatX);
		}

		if (pMirrorX && pMirrorY)
		{

			D3DXFROMWINEMatrixTranslation (&mMatWorld, (float) -pWidth, 0, 0);
			D3DXFROMWINEMatrixTranslation (&mMatTraslation, 0, (float) -pHeight, 0);
			D3DXFROMWINEMatrixMultiply	(&mMatWorld, &mMatWorld, &mMatTraslation);

			D3DXFROMWINEMatrixRotationY (&mMatY, D3DXFROMWINEToRadian (180));
			D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatY);
			D3DXFROMWINEMatrixRotationX (&mMatX, D3DXFROMWINEToRadian (180));
			D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatX);
		}
	}

	// Hotspot
	if (pAxisCalX != 0 || pAxisCalY != 0)
	{
		D3DXFROMWINEMatrixTranslation (&mMatTraslation, (float) pAxisCalX, (float) pAxisCalY, 0);
		D3DXFROMWINEMatrixMultiply	(&mMatWorld, &mMatWorld, &mMatTraslation);
	}

	// Scaling
	if (pScaleX != 1.0f || pScaleY != 1.0f)
	{
		D3DXFROMWINEMatrixScaling (&mMatScale, pScaleX, pScaleY, 1.0f);
		D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatScale);
	}

	// Rotations
	if (pAngleX != 0.0f)
	{
		D3DXFROMWINEMatrixRotationX (&mMatX, D3DXFROMWINEToRadian (pAngleX));
		D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatX);
	}
	if (pAngleY != 0.0f)
	{
		D3DXFROMWINEMatrixRotationY (&mMatY, D3DXFROMWINEToRadian (pAngleY));
		D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatY);
	}
	if (pAngleZ != 0.0f)
	{
		D3DXFROMWINEMatrixRotationZ (&mMatZ, D3DXFROMWINEToRadian (pAngleZ));
		D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatZ);
	}

	// Translations
	if (pX != 0 || pY != 0)
	{
		D3DXFROMWINEMatrixTranslation (&mMatTraslation, (float) pX, (float) pY, 0);
		D3DXFROMWINEMatrixMultiply (&mMatWorld, &mMatWorld, &mMatTraslation);
	}

	// ----- Return World Matrix (in IndieLib format) -----

	if (pMatrix)
	{
		pMatrix->m[0][0] = mMatWorld.m[0][0];
		pMatrix->m[0][1] = mMatWorld.m[0][1];
		pMatrix->m[0][2] = mMatWorld.m[0][2];
		pMatrix->m[0][3] = mMatWorld.m[0][3];

		pMatrix->m[1][0] = mMatWorld.m[1][0];
		pMatrix->m[1][1] = mMatWorld.m[1][1];
		pMatrix->m[1][2] = mMatWorld.m[1][2];
		pMatrix->m[1][3] = mMatWorld.m[1][3];

		pMatrix->m[2][0] = mMatWorld.m[2][0];
		pMatrix->m[2][1] = mMatWorld.m[2][1];
		pMatrix->m[2][2] = mMatWorld.m[2][2];
		pMatrix->m[2][3] = mMatWorld.m[2][3];

		pMatrix->m[3][0] = mMatWorld.m[3][0];
		pMatrix->m[3][1] = mMatWorld.m[3][1];
		pMatrix->m[3][2] = mMatWorld.m[3][2];
		pMatrix->m[3][3] = mMatWorld.m[3][3];
	}

	// ----- Applies the transformation -----

	IRenderer::GetRendererInstance()->SetTransform (TS_WORLD, mMatWorld);
}


/*!
\b Parameters:

\arg \b pType										::IND_Type type of the following graphical object.
\arg \b pCull										Backface culling. (true / false) = (on / off). This feature can
													be deactivated when we are going to rotate the graphical object
													in the x or y coordinates. This way it is possible to draw for
													example the leaf of a tree that falls down rotating and shows
													both faces. In cases that you don't want to draw back faces
													deactivate this for faster render times.
\arg \b pMirrorX									Horizontal mirroring. (true / false) = (activated / deactivated).
\arg \b pMirrorY									Vertical mirroring. (true / false) = (activated / deactivated).
\arg \b pFilter										Type of filter ::IND_Filter. There are two types of filters that
													can be applied to the graphical object when it suffers a rotation
													or scaling:
													- ::IND_FILTER_POINT (Nearest point filter). Less quality, but
													bigger performance. It is possible to use this one without
													quality loss in graphical objects that don't rotate or are
													affected by scaling.
													- ::IND_FILTER_LINEAR (Bilinear filter). More quality, but less
													performance. Used in graphical objects that be rotated or
													affected by scaling.
\arg \b pR, pG, pB									Color bytes FadeR, FadeG, FadeB
													Used for fading the image to a certain color.
													For example, if we use RGB = (255, 0, 0),
													the image will progressively become red.
\arg \b pA											The A unsigned char indicates the level of transparency.
													If a value of 0 is used, the following graphical
													object will be completely transparent, as opposed
													to the value 255 that will cause the object
													to be drawn completely opaque. It is possible to use all the
													intermediate values for different levels of
													transparency.
\arg \b pFadeR, \b pFadeG, \b pFadeB, \b pFadeA		Bytes FadeR, FadeG, FadeB
													Used for fading the image to a certain color.
													For example, if we use RGB = (255, 0, 0),
													the image will progressively become red.
\arg \b pSo											Indicates the blending source,
													see (::IND_BlendingType).
\arg \b pDs											Indicates the blending destiny,
													see (::IND_BlendingType).

Operation:

This functions sets the color and blending attributes of a graphical object. It should
be used before any calling to the Blit methods.

Important: It is not possible to use tinting and fading at the same time, the fading will overide the tinting.

Remember that you can use IND_Entity2d objects for applying color
transformations to the graphical objects without having to use this advanced method directly.
This method is only useful for advanced users with really concrete purposes.

Using this method is equivalent to using a combination of these methods:
- IND_Entity2d::SetTransparency()
- IND_Entity2d::SetFade()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetBlendSource()
- IND_Entity2d::SetBlendDest()
*/
int ab=-1;
INLINE int IND_Render::SetRainbow2d	(IND_Type pType,
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
										IND_BlendingType pDs)
{
#if 0
	// ----- Filters -----

	IRenderer::GetRendererInstance()->SetSamplerState (0, D3DSAMP_MIPFILTER, GetD3DFilter (pFilter));
	IRenderer::GetRendererInstance()->SetSamplerState (0, D3DSAMP_MINFILTER, GetD3DFilter (pFilter));
	IRenderer::GetRendererInstance()->SetSamplerState (0, D3DSAMP_MAGFILTER, GetD3DFilter (pFilter));

	// ----- Back face culling -----

	if (pCull)
		IRenderer::GetRendererInstance()->SetRenderState (D3DRS_CULLMODE, D3DCULL_CCW);
	else
		IRenderer::GetRendererInstance()->SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);

	// Mirroring (180º rotations)
	if (pMirrorX || pMirrorY)
	{
		if (pMirrorX && !pMirrorY)
		{
			// Back face culling
			if (pCull)
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_CULLMODE, D3DCULL_CW);
			else
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);
		}

		if (!pMirrorX && pMirrorY)
		{
			// Back face culling
			if (pCull)
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_CULLMODE, D3DCULL_CW);
			else
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);
		}
	}

	// ----- Blending -----

	switch (pType)
	{
		case IND_OPAQUE:
		{
			// Alphablending and alpha test = OFF
			IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ALPHABLENDENABLE,	FALSE);
			IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ALPHATESTENABLE,	FALSE);

			// Initializes the tinting and alpha values of previous iterations
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG1,  D3DTA_TEXTURE);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG2,  D3DTA_CURRENT);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			// Tinting
			if (pR != 255 || pG != 255 || pB != 255)
			{
				// Tinting color
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));

				// Tinting
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			}

			// Alpha
			if (pA != 255)
			{
				// Alpha color
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));

				// Alpha-blending = ON
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ALPHABLENDENABLE, TRUE);

				// Set source alpha and destination alpha
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				// Alpha
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAARG1,  D3DTA_TFACTOR);
			}

			// Fade to color
			if (pFadeA != 255)
			{
				// Color fade
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pFadeR, pFadeG, pFadeB, pFadeA));

				// Fade
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			}

			if (pSo && pDs)
			{
				// Alpha-blending = ON
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ALPHABLENDENABLE, TRUE);

				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_SRCBLEND,	GetD3DBlendingType (pSo));
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_DESTBLEND, GetD3DBlendingType (pDs));
			}

			break;
		}

		case IND_ALPHA:
		{
			// Alpha-testing = OFF
			IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ALPHATESTENABLE, FALSE);

			// Initializes the tinting and alpha values of previous iterations
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG1,  D3DTA_TEXTURE);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG2,  D3DTA_CURRENT);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			// Tinting
			if (pR != 255 || pG != 255 || pB != 255)
			{
				// Tinting color
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));

				// Tinting
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			}

			// Alpha
			if (pA != 255)
			{
				// Color alpha
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));

				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			}

			// Fade to color
			if (pFadeA != 255)
			{
				// Color fade
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pFadeR, pFadeG, pFadeB, pFadeA));

				// Fade
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				IRenderer::GetRendererInstance()->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			}

			// Alpha-blending = ON
			IRenderer::GetRendererInstance()->SetRenderState (D3DRS_ALPHABLENDENABLE,	TRUE);

			// Set source alpha and destination alpha
			if (!pSo || !pDs)
			{
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
			else
			{
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_SRCBLEND,	GetD3DBlendingType (pSo));
				IRenderer::GetRendererInstance()->SetRenderState (D3DRS_DESTBLEND, GetD3DBlendingType (pDs));
			}
		}

		default:
		{
		}
	}
#else
//???DBG_HALT;
	// ----- Blending -----
	ab=IRenderer::GetRendererInstance()->GetblendSrcAlpha();
	ab=IRenderer::GetRendererInstance()->GetblendOneOne();

	switch (pType)
	{
		case IND_OPAQUE:
		{
			// Alpha
			if (pA != 255)
			{
 	ab=IRenderer::GetRendererInstance()->GetblendSrcAlpha();
			}
			if (pSo && pDs)
			{
 	//ab=IRenderer::GetRendererInstance()->GetblendSrcAlpha();
//	ab=IRenderer::GetRendererInstance()->GetblendOneOne();
			}
		}
		case IND_ALPHA:
		{
			// Alpha
			//if (pA != 255)
			{
 	ab=IRenderer::GetRendererInstance()->GetblendSrcAlpha();
			}/*
			else
			{
	ab=IRenderer::GetRendererInstance()->GetblendOneOne();
			}*/
		}
	};
#endif
		return ab;
}

