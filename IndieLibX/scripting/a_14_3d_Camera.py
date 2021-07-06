#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mAngle = 0
		self.mYaw = 0.0
		self.mText = ""
		self.mSpeedRotation = 50.0
		self.mDelta = 0.0

		self.mMeshDino=IndieLib.IND_Factory.Get3dMesh()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.mDino=IndieLib.IND_Factory.GetEntity3d()
		self.mLight=IndieLib.IND_Factory.GetLight()
		self.mCamera2d=IndieLib.IND_Factory.GetCamera2d()
		self.mCamera3d=IndieLib.IND_Factory.GetCamera3d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_14_3d_Camera")
		self.mText = ""
		#if not IndieLib.CIndieLib.Instance().MeshManager.Add (self.mMeshDino, "/skeleton/skeleton.x", "/skeleton"):
		#	return 0
		if not IndieLib.CIndieLib.Instance().MeshManager.Add (self.mMeshDino, "/trex_dx/dino_videogame.x", "/trex_dx"):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont	(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition	(5, 5, 1)
		self.mTextSmallWhite.SetAlign	(IndieLib.IND_LEFT)

		IndieLib.CIndieLib.Instance().Entity3dManager.Add (self.mDino)
		self.mDino.Set3dMesh (self.mMeshDino)
		self.mCamera2d=IndieLib.IND_Factory.GetCamera2d(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight () / 2)

		self.mCamera3d=IndieLib.IND_Factory.GetCamera3d(0.0, 0.0, -2.0)
		self.mCamera3d.SetAspect (IndieLib.CIndieLib.Instance().Window.GetWidth () / IndieLib.CIndieLib.Instance().Window.GetHeight())
		IndieLib.CIndieLib.Instance().LightManager.Add (self.mLight, IndieLib.IND_DIRECTIONAL_LIGHT)
		self.mLight.SetPosition (0, 0, 0)
		self.mLight.SetColor (1.0, 1.0, 1.0, 1.0)
		self.mLight.SetDirection (0.0, -0.3, 0.5)
		self.mLight.SetRange (1000.0)

		return 0

	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mText = "Use arrow keys to move\n"
		self.mText += "Use Z / X keys to strafe\n"
		self.mTextSmallWhite.SetText	 (self.mText)

		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_LEFT):
			self.mYaw -= self.mSpeedRotation * self.mDelta
		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_RIGHT):
			self.mYaw += self.mSpeedRotation * self.mDelta

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_UP):
			self.mCamera3d.MoveWalk (0.01)
		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_DOWN):
			self.mCamera3d.MoveWalk (-0.01)
		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_z):
			self.mCamera3d.MoveStrafe (-0.01)
		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_x):
			self.mCamera3d.MoveStrafe (0.01)

		self.mCamera3d.SetYaw (self.mYaw)

		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.BeginScene ()

		IndieLib.CIndieLib.Instance().Render.SetViewPort3d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())
		IndieLib.CIndieLib.Instance().Render.SetCamera3d (self.mCamera3d)

		IndieLib.CIndieLib.Instance().LightManager.Update ()

		IndieLib.CIndieLib.Instance().Entity3dManager.RenderEntities3d ()

		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())
		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCamera2d)
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

