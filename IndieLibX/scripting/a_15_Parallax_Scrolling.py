#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mMiddleScreenX=0# = IndieLib.CIndieLib.Instance().Window.GetWidth () / 2
		self.mMiddleScreenY=0# = IndieLib.CIndieLib.Instance().Window.GetHeight() / 2

		self.mPosXCamera0=0# = (float) mMiddleScreenX
		self.mPosXCamera1=0# = (float) mMiddleScreenX
		self.mPosXCamera2=0# = (float) mMiddleScreenX

		self.mSpeedLayer0		= 50
		self.mSpeedLayer1		= 162
		self.mSpeedLayer2		= 250

		self.mText = "" 
		self.mDelta=0.0

		self.mSurfaceCave=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceCaveFirstPlane=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceSky=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.mSky=IndieLib.IND_Factory.GetEntity2d()
		self.mCave=IndieLib.IND_Factory.GetEntity2d()
		self.mCaveFirstPlane=IndieLib.IND_Factory.GetEntity2d()
		self.mCamera0=IndieLib.IND_Factory.GetCamera2d()
		self.mCamera1=IndieLib.IND_Factory.GetCamera2d()
		self.mCamera2=IndieLib.IND_Factory.GetCamera2d()
		self.mCameraText=IndieLib.IND_Factory.GetCamera2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_15_Parallax_Scrolling")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceCave, "/cave.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceCaveFirstPlane, "/cave_near.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceSky, "/sky.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(3, self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont		(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition	(5, 5, 1)
		self.mTextSmallWhite.SetAlign	(IndieLib.IND_LEFT)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.mSky)
		self.mSky.SetSurface (self.mSurfaceSky)
		self.mSky.SetPosition (600, 0, 0)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (1, self.mCave)
		self.mCave.SetSurface (self.mSurfaceCave)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (2, self.mCaveFirstPlane)
		self.mCaveFirstPlane.SetSurface (self.mSurfaceCaveFirstPlane)

		self.mCamera0=IndieLib.IND_Factory.GetCamera2d	(self.mPosXCamera0, self.mMiddleScreenY)
		self.mCamera1=IndieLib.IND_Factory.GetCamera2d	(self.mPosXCamera1, self.mMiddleScreenY)
		self.mCamera2=IndieLib.IND_Factory.GetCamera2d	(self.mPosXCamera2, self.mMiddleScreenY)

		self.mCameraText=IndieLib.IND_Factory.GetCamera2d (IndieLib.CIndieLib.Instance().Window.GetWidth() / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

		self.mText = ""
		self.mMiddleScreenX = IndieLib.CIndieLib.Instance().Window.GetWidth () / 2
		self.mMiddleScreenY = IndieLib.CIndieLib.Instance().Window.GetHeight() / 2

		self.mPosXCamera0 = self.mMiddleScreenX
		self.mPosXCamera1 = self.mMiddleScreenX
		self.mPosXCamera2 = self.mMiddleScreenX
	
		return 0

	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mText = "Use right and left arrow keys for moving the camera\n"
		self.mTextSmallWhite.SetText	 (self.mText)

		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_RIGHT):
			if self.mPosXCamera2 < self.mSurfaceCaveFirstPlane.GetWidth() - self.mMiddleScreenX:
				self.mPosXCamera0 += self.mSpeedLayer0 * self.mDelta
				self.mPosXCamera1 += self.mSpeedLayer1 * self.mDelta
				self.mPosXCamera2 += self.mSpeedLayer2 * self.mDelta
			
		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_LEFT):
			if self.mPosXCamera2 > self.mMiddleScreenX:
				self.mPosXCamera0 -= self.mSpeedLayer0 * self.mDelta
				self.mPosXCamera1 -= self.mSpeedLayer1 * self.mDelta
				self.mPosXCamera2 -= self.mSpeedLayer2 * self.mDelta
			
		

		self.mCamera0.SetPosition (self.mPosXCamera0, self.mMiddleScreenY)
		self.mCamera1.SetPosition (self.mPosXCamera1, self.mMiddleScreenY)
		self.mCamera2.SetPosition (self.mPosXCamera2, self.mMiddleScreenY)

		IndieLib.CIndieLib.Instance().Render.ClearViewPort(60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.BeginScene ()

		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCamera0)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (0)

		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCamera1)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (1)

		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCamera2)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (2)

		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCameraText)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (3)

		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

