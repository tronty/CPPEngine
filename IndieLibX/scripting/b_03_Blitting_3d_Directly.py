#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mSequence = 0
		self.mMeshBones=IndieLib.IND_Factory.Get3dMesh()
		self.mLight=IndieLib.IND_Factory.GetLight()
		self.mMatrix=IndieLib.IND_Factory.GetMatrix()
		self.mCamera3d=IndieLib.IND_Factory.GetCamera3d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("b_03_Blitting_3d_directly")

		if not IndieLib.CIndieLib.Instance().MeshManager.Add (self.mMeshBones, "/skeleton/skeleton.x", "/skeleton"):
			return 0

		self.mCamera3d=IndieLib.IND_Factory.GetCamera3d(0.0, 0.9, -3.0)
		self.mCamera3d.SetAspect (IndieLib.CIndieLib.Instance().Window.GetWidth () / IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().LightManager.Add (self.mLight, IndieLib.IND_DIRECTIONAL_LIGHT)
		self.mLight.SetPosition (0, 0, 0)
		self.mLight.SetColor (1.0, 1.0, 1.0, 1.0)
		self.mLight.SetDirection (0.0, -0.3, 0.5)
		self.mLight.SetRange (1000.0)

		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_UP):
		
			self.mSequence = self.mSequence+1
			if self.mSequence == self.mMeshBones.GetNumSequences():
					self.mSequence = 0
			IndieLib.CIndieLib.Instance().Render.Set3dMeshSequence (self.mMeshBones, self.mSequence)
		

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_DOWN):
		
			self.mSequence = self.mSequence-1
			if self.mSequence < 0:
					self.mSequence = self.mMeshBones.GetNumSequences() - 1
			IndieLib.CIndieLib.Instance().Render.Set3dMeshSequence (self.mMeshBones, self.mSequence)
		

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)

		IndieLib.CIndieLib.Instance().Render.SetViewPort3d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())
		IndieLib.CIndieLib.Instance().Render.SetCamera3d (self.mCamera3d)

		IndieLib.CIndieLib.Instance().LightManager.Update ()

		IndieLib.CIndieLib.Instance().Render.SetTransform3d(0, 0, 0, 0, 0, 0, 1, 1, 1, self.mMatrix)

		#IndieLib.CIndieLib.Instance().Render.SetRainbow3d(1, False, IndieLib.IND_FILTER_LINEAR, 255, 255, 255, 255, 0, 0, 0, 255, IndieLib.IND_SRCALPHA, IndieLib.IND_INVSRCALPHA)

		IndieLib.CIndieLib.Instance().Render.Blit3dMesh (self.mMeshBones)

		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

