#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceBeetleship=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceOctopus=IndieLib.IND_Factory.GetSurface()
		self.mSurfacePlanet=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mBeetleship=IndieLib.IND_Factory.GetEntity2d()
		self.mOctopus=IndieLib.IND_Factory.GetEntity2d()
		self.mPlanet=IndieLib.IND_Factory.GetEntity2d()

		self.mAngle = 0.0
		self.mPos = 400.0
		self.mSpeed = 200
		self.mDelta = 0.0
		self.mText = ""

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_07_IND_Input")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/twist.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBeetleship, "/beetleship.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceOctopus, "/octopus.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfacePlanet, "/planet.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.mTextSmallWhite)		
		self.mTextSmallWhite.SetFont	(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition(5, 5, 1)
		self.mTextSmallWhite.SetAlign(IndieLib.IND_LEFT)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBeetleship)
		self.mBeetleship.SetSurface (self.mSurfaceBeetleship)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mOctopus)
		self.mOctopus.SetSurface (self.mSurfaceOctopus)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mPlanet)
		self.mPlanet.SetSurface (self.mSurfacePlanet)

		self.mBack.SetHotSpot (0.5, 0.5)
		self.mBack.SetPosition (400, 300, 0)
		self.mBack.SetScale (1.7, 1.7)

		self.mBeetleship.SetHotSpot (0.5, 0.5)

		self.mOctopus.SetMirrorX (True)
		self.mOctopus.SetHotSpot (0.5, 0.5)

		self.mPlanet.SetHotSpot (0.5, 0.5)

		self.mText = ""
		return 0

	
	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mText = "Use the mouse for moving the planet\n"
		self.mText+= "Use left and right arrow keys for moving the ships\n"
		self.mText+= "Press CTRL + X or ESC key to quit"
		self.mTextSmallWhite.SetText	 (self.mText)

		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_RIGHT):
			self.mPos = self.mPos + self.mSpeed * self.mDelta

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_LEFT):
			self.mPos = self.mPos - self.mSpeed * self.mDelta

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_LCTRL) and IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_x):
		
			IndieLib.CIndieLib.Instance().Render.EndScene ()
		#IndieLib.CIndieLib.Instance().End0???()
		

		self.mAngle += (self.mSpeed / 4) * self.mDelta

		self.mBack.SetAngleXYZ (0, 0, self.mAngle)

		self.mBeetleship.SetPosition ( self.mPos, 140, 1)
		self.mBeetleship.SetAngleXYZ (0, 0,  self.mPos)

		self.mPlanet.SetPosition ( IndieLib.CIndieLib.Instance().Input.GetMouseX(), 300, 2)

		self.mOctopus.SetPosition (800 -  self.mPos, 480, 3)

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

