#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mImageBugOriginal=IndieLib.IND_Factory.GetImage()
		self.mImageBugGaussian=IndieLib.IND_Factory.GetImage()
		self.mImageBugPixelize=IndieLib.IND_Factory.GetImage()
		self.mImageBugContrast=IndieLib.IND_Factory.GetImage()
		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mOriginalSurface=IndieLib.IND_Factory.GetSurface()
		self.mGaussianSurface=IndieLib.IND_Factory.GetSurface()
		self.mPixelizeSurface=IndieLib.IND_Factory.GetSurface()
		self.mContrastSurface=IndieLib.IND_Factory.GetSurface()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mOriginal=IndieLib.IND_Factory.GetEntity2d()
		self.mGaussian=IndieLib.IND_Factory.GetEntity2d()
		self.mPixelize=IndieLib.IND_Factory.GetEntity2d()
		self.mContrast=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_03_Ind_Image")

		if not IndieLib.CIndieLib.Instance().ImageManager.Add (self.mImageBugOriginal, "/Enemy_Bug.png"): 
			return 0
		if not IndieLib.CIndieLib.Instance().ImageManager.Clone (self.mImageBugGaussian, self.mImageBugOriginal): 
			return 0
		if not IndieLib.CIndieLib.Instance().ImageManager.Clone (self.mImageBugPixelize, self.mImageBugOriginal): 
			return 0
		if not IndieLib.CIndieLib.Instance().ImageManager.Clone (self.mImageBugContrast, self.mImageBugOriginal): 
			return 0
		self.mImageBugGaussian.GaussianBlur	(5)
		self.mImageBugPixelize.Pixelize		(5)
		self.mImageBugContrast.Contrast		(20)
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/blue_background.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32): 
			return 0
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mOriginalSurface, self.mImageBugOriginal, IndieLib.IND_ALPHA, IndieLib.IND_32): 
			return 0
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mGaussianSurface, self.mImageBugGaussian, IndieLib.IND_ALPHA, IndieLib.IND_32): 
			return 0
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mPixelizeSurface, self.mImageBugPixelize, IndieLib.IND_ALPHA, IndieLib.IND_32): 
			return 0
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mContrastSurface, self.mImageBugContrast, IndieLib.IND_ALPHA, IndieLib.IND_32): 
			return 0
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mOriginal)
		self.mOriginal.SetSurface (self.mOriginalSurface)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mGaussian)
		self.mGaussian.SetSurface (self.mGaussianSurface)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mPixelize)
		self.mPixelize.SetSurface (self.mPixelizeSurface)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mContrast)
		self.mContrast.SetSurface (self.mContrastSurface)
		self.mOriginal.SetPosition (290, 90, 0)
		self.mGaussian.SetPosition (50, 230, 0)
		self.mPixelize.SetPosition (290, 230, 0)
		self.mContrast.SetPosition (520, 230, 0)

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()
		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()


