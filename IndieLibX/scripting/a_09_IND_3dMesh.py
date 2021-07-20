#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mAngle = 0.0
		self.mSequence = 0
		self.mText = ""
		self.mSpeed = 25
		self.mDelta = 0.0

		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.mMeshBones=IndieLib.IND_Factory.Get3dMesh()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mBones=IndieLib.IND_Factory.GetEntity3d()
		self.mLight=IndieLib.IND_Factory.GetLight()
		self.mCamera2d=IndieLib.IND_Factory.GetCamera2d() 		
		self.mCamera3d=IndieLib.IND_Factory.GetCamera3d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_09_3dMesh")
		self.mText = ""
	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/twist.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add		(self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont			(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition		(5, 5, 1)
		self.mTextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		if not IndieLib.CIndieLib.Instance().MeshManager.Add (self.mMeshBones, "/skeleton/skeleton.x", "/skeleton"):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity3dManager.Add (self.mBones)
		self.mBones.Set3dMesh (self.mMeshBones)
	
		self.mBack.SetHotSpot (0.5, 0.5)
		self.mBack.SetPosition (400, 300, 0)
		self.mBack.SetScale (1.7, 1.7)

		self.mBones.SetPosition (0, 0, 0)
		self.mBones.SetScale (1.5, 1.5, 1.5)

		self.mCamera2d=IndieLib.IND_Factory.GetCamera2d(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

		self.mCamera3d=IndieLib.IND_Factory.GetCamera3d(0.0, 1.4, -4.0)
		self.mCamera3d.SetAspect (IndieLib.CIndieLib.Instance().Window.GetWidth () / 		   IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().LightManager.Add (self.mLight, IndieLib.IND_DIRECTIONAL_LIGHT)
		self.mLight.SetPosition (0, 0, 0)
		self.mLight.SetColor (1.0, 1.0, 1.0, 1.0)
		self.mLight.SetDirection (0.0, -0.3, 0.5)
		self.mLight.SetRange (1000.0)
		return 0

	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mText = "Use up and down arrow keys to toggle the animation\n"
		self.mTextSmallWhite.SetText	 (self.mText)

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_UP):
		
			self.mSequence=self.mSequence+1
			if self.mSequence == self.mMeshBones.GetNumSequences():
				self.mSequence = 0
				self.mBones.SetSequence (self.mSequence)
		

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_DOWN):
		
			self.mSequence=self.mSequence-1
			if self.mSequence < 0:
				self.mSequence = self.mMeshBones.GetNumSequences() - 1
				self.mBones.SetSequence (self.mSequence)
		
		self.mAngle += self.mDelta * self.mSpeed

		self.mBack.SetAngleXYZ (0, 0, self.mAngle)
		self.mBones.SetAngleXYZ (0, self.mAngle, 0)

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, 800, 600)
		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCamera2d)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ()
		IndieLib.CIndieLib.Instance().Render.SetViewPort3d (0, 0, 800, 600)
		IndieLib.CIndieLib.Instance().Render.SetCamera3d (self.mCamera3d)
		IndieLib.CIndieLib.Instance().LightManager.Update ()
		IndieLib.CIndieLib.Instance().Entity3dManager.RenderEntities3d ()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

