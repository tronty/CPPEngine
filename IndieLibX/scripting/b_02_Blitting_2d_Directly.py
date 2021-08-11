#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mWidth = 0
		self.mHeight = 0
		self.mAngle = 0.0
		self.mHotSpotX = 0
		self.mHotSpotY = 0
		self.mAxisCalX = 0
		self.mAxisCalY = 0
		self.mSpeed = 50.0
		self.mDelta = 0.0

		self.mSurfaceDraco=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceGem=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceBug=IndieLib.IND_Factory.GetSurface()
		self.mAnimationUfo=IndieLib.IND_Factory.GetAnimation()
		self.mMatrix=IndieLib.IND_Factory.GetMatrix()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("b_02_Blitting_2d_directly")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceDraco, "/draco.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceGem, "/gem_squared.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBug, "/Enemy_Bug.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.mAnimationUfo, "/animations/ufo.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0	
		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)

		self.mWidth = self.mSurfaceDraco.GetWidth()
		self.mHeight = self.mSurfaceDraco.GetHeight()
		IndieLib.CIndieLib.Instance().Render.SetTransform2d	(50, 70, 0, 0, 0, 1, 1, 0, 0, 0, 0, self.mWidth, self.mHeight, self.mMatrix)

		IndieLib.CIndieLib.Instance().Render.SetRainbow2d	(IndieLib.IND_ALPHA, 1, 0, 0, IndieLib.IND_FILTER_LINEAR, 255, 255, 255, 255, 0, 0, 0, 255, IndieLib.IND_SRCALPHA, IndieLib.IND_INVSRCALPHA)


		IndieLib.CIndieLib.Instance().Render.BlitSurface	(self.mSurfaceDraco)


		self.mAngle += self.mSpeed *self.mDelta
		self.mHotSpotX = 0.5
		self.mHotSpotY = 0.5
		self.mWidth = self.mSurfaceBug.GetWidth()
		self.mHeight = self.mSurfaceBug.GetHeight()
		self.mAxisCalX = (self.mHotSpotX * self.mWidth * -1)
		self.mAxisCalY = (self.mHotSpotY * self.mHeight * -1)

		IndieLib.CIndieLib.Instance().Render.SetTransform2d	(500, 100, 0, 0, self.mAngle, 1, 1, self.mAxisCalX, self.mAxisCalY, 0, 0, self.mWidth, self.mHeight, self.mMatrix)
		IndieLib.CIndieLib.Instance().Render.SetRainbow2d	(IndieLib.IND_ALPHA, 1, 0, 0, IndieLib.IND_FILTER_LINEAR, 255, 255, 255, 255, 0, 0, 0, 255, IndieLib.IND_SRCALPHA, IndieLib.IND_INVSRCALPHA)

		IndieLib.CIndieLib.Instance().Render.BlitSurface	(self.mSurfaceBug)


		self.mWidth = self.mSurfaceGem.GetWidth()
		self.mHeight = self.mSurfaceGem.GetHeight()
		IndieLib.CIndieLib.Instance().Render.SetTransform2d	(600, 200, 0, 0, 0, 1, 1, 0, 0, 0, 0, self.mWidth, self.mHeight, self.mMatrix)
		IndieLib.CIndieLib.Instance().Render.SetRainbow2d	(IndieLib.IND_ALPHA, 1, 0, 0, IndieLib.IND_FILTER_LINEAR, 255, 255, 255, 255, 0, 0, 0, 255, IndieLib.IND_SRCALPHA, IndieLib.IND_INVSRCALPHA)
		IndieLib.CIndieLib.Instance().Render.BlitRegionSurface (self.mSurfaceGem, 10, 10, 70, 70)


		self.mWidth = self.mSurfaceGem.GetWidth()
		self.mHeight = self.mSurfaceGem.GetHeight()
		IndieLib.CIndieLib.Instance().Render.SetTransform2d	(500, 350, 0, 0, 0, 1, 1, 0, 0, 0, 0, self.mWidth, self.mHeight, self.mMatrix)
		IndieLib.CIndieLib.Instance().Render.SetRainbow2d	(IndieLib.IND_ALPHA, 1, 0, 0, IndieLib.IND_FILTER_LINEAR,	 255, 255, 255, 255, 0, 0, 0, 255, IndieLib.IND_SRCALPHA, IndieLib.IND_INVSRCALPHA)

		IndieLib.CIndieLib.Instance().Render.BlitWrapSurface (self.mSurfaceGem, 200, 200, 0, 0)
		self.mWidth = self.mAnimationUfo.GetActualSurface (0).GetWidth()
		self.mHeight= self.mAnimationUfo.GetActualSurface (0).GetHeight()
		IndieLib.CIndieLib.Instance().Render.SetTransform2d	(650, 70, 0, 0, 0, 1, 1, 0, 0, 0, 0, self.mWidth, self.mHeight, self.mMatrix)
		IndieLib.CIndieLib.Instance().Render.SetRainbow2d	(IndieLib.IND_ALPHA, 1, 0, 0, IndieLib.IND_FILTER_LINEAR, 255, 255, 255, 255, 0, 0, 0, 255, IndieLib.IND_SRCALPHA, IndieLib.IND_INVSRCALPHA)

		if IndieLib.CIndieLib.Instance().Render.BlitAnimation	(self.mAnimationUfo, 0, 0, 0, 0, 0, 0, 0, 0) == -1:
			self.mAnimationUfo.SetActualFramePos (0, 0)

		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

