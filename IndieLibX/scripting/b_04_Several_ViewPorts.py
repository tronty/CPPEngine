#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mZoom = 1.0
		self.mCameraAngle = 0.0
		self.mBugAngle = 0.0
		self.mText = ""
		self.mSpeedRotation = 50.0
		self.mDelta = 0.0

		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceBeetle=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceOctopus=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceBug=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.mTerrain=IndieLib.IND_Factory.GetEntity2d()
		self.mBeetle=IndieLib.IND_Factory.GetEntity2d()
		self.mOctopus=IndieLib.IND_Factory.GetEntity2d()
		self.mBug=IndieLib.IND_Factory.GetEntity2d()
		self.mCamera1=IndieLib.IND_Factory.GetCamera2d()
		self.mCamera2=IndieLib.IND_Factory.GetCamera2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("b_04_Several_ViewPorts")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/twist.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBeetle, "/beetleship.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceOctopus, "/octopus.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBug, "/Enemy_Bug.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add		(self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont	(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition		(5, 5, 1)
		self.mTextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mTerrain)
		self.mTerrain.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBeetle)
		self.mBeetle.SetSurface (self.mSurfaceBeetle)
		self.mBeetle.SetHotSpot (0.5, 0.5)
		self.mBeetle.SetPosition (150, 150, 2)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mOctopus)
		self.mOctopus.SetSurface (self.mSurfaceOctopus)
		self.mOctopus.SetHotSpot (0.5, 0.5)
		self.mOctopus.SetPosition (450, 150, 2)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBug)
		self.mBug.SetSurface (self.mSurfaceBug)
		self.mBug.SetHotSpot (0.5, 0.5)
		self.mBug.SetPosition (700, 150, 2)

		self.mCamera1=IndieLib.IND_Factory.GetCamera2d(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 4)

		self.mCamera2=IndieLib.IND_Factory.GetCamera2d(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 4)

		self.mText = ""
		return 0
	
	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mText  = "Use mouse wheel for zooming in/out\n"
		self.mText += "Use mouse buttons for rotating the camera"
		self.mTextSmallWhite.SetText	 (self.mText)

		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if IndieLib.CIndieLib.Instance().Input.OnMouseButtonPress	(IndieLib.STX_MBUTTON_WHEELUP):
			self.mZoom += 0.1
		if IndieLib.CIndieLib.Instance().Input.OnMouseButtonPress	(IndieLib.STX_MBUTTON_WHEELDOWN):
			self.mZoom -= 0.1

		if IndieLib.CIndieLib.Instance().Input.IsMouseButtonPressed (IndieLib.STX_MBUTTON_LEFT):
			self.mCameraAngle += self.mSpeedRotation * self.mDelta
		if IndieLib.CIndieLib.Instance().Input.IsMouseButtonPressed (IndieLib.STX_MBUTTON_RIGHT):
			self.mCameraAngle -= self.mSpeedRotation * self.mDelta

		self.mBugAngle += self.mSpeedRotation * self.mDelta
		self.mBeetle.SetAngleXYZ (0, 0, self.mBugAngle)
		self.mBeetle.SetAngleXYZ (0, 0, self.mBugAngle)
		self.mBug.SetAngleXYZ (0, 0, -self.mBugAngle)
		self.mBug.SetAngleXYZ (0, 0, -self.mBugAngle)

		if self.mZoom < 0.1:
			self.mZoom =  0.1
		self.mCamera2.SetAngle (self.mCameraAngle)
		self.mCamera2.SetZoom (self.mZoom)

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)
		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCamera1)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ()

		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)
		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCamera2)
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

