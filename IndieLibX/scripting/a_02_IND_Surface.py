#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceWarrior=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceStar=IndieLib.IND_Factory.GetSurface()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mWarrior=IndieLib.IND_Factory.GetEntity2d()
		self.mStar1=IndieLib.IND_Factory.GetEntity2d()
		self.mStar2=IndieLib.IND_Factory.GetEntity2d()
		self.mStar3=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_02_IND_Surface")

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/blue_background.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return  0
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceWarrior, "/derekyu_sprite.png", IndieLib.IND_ALPHA, IndieLib.IND_32, 255, 0, 255): 
			return  0
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceStar, "/star.png", IndieLib.IND_ALPHA, IndieLib.IND_32): 
			return  0
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mWarrior)
		self.mWarrior.SetSurface (self.mSurfaceWarrior)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mStar1)
		self.mStar1.SetSurface (self.mSurfaceStar)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mStar2)
		self.mStar2.SetSurface (self.mSurfaceStar)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mStar3)
		self.mStar3.SetSurface (self.mSurfaceStar)
		self.mWarrior.SetPosition (400, 170, 0)
		self.mStar1.SetPosition (100, 270, 0)
		self.mStar2.SetPosition (280, 200, 0)
		self.mStar2.SetScale	(2, 2)
		self.mStar2.SetTint	(240, 160, 230)
		self.mStar2.SetTransparency (200)
		self.mStar2.SetAngleXYZ	(0, 0, 45)
		self.mStar3.SetHotSpot (0.5, 0.5)
		self.mStar3.SetPosition (400, 470, 0)
		self.mStar3.SetScale	(1.5, 1.5)
		self.mStar3.SetFade	(230, 0, 230, 128)
		self.mStar3.SetRegion (20, 20, 50, 50)
		return 0
	
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

