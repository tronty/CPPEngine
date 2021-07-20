#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class CTerrain:
	def __init__(self):  
		self.mHoles=IndieLib.IND_Factory.GetEntity2d()
		self.mTerrain=IndieLib.IND_Factory.GetEntity2d()
		self.mNumBlocksX=0
		self.mNumBlocksY=0
 
	def __init__(self, pNumBlocksX, pNumBlocksY, pNumHoles, pTerrainTile, pHole1, pHole2):
		self.mHoles=IndieLib.IND_Factory.GetEntity2d()
		self.mTerrain=IndieLib.IND_Factory.GetEntity2d()
		self.mNumBlocksX = pNumBlocksX
		self.mNumBlocksY = pNumBlocksY

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.mTerrain)	
		self.mTerrain.SetSurface(pTerrainTile)
		self.mTerrain.ToggleWrap (True)
		self.mTerrain.SetRegion (0, 0, pNumBlocksX *  pTerrainTile.GetWidth(), pNumBlocksY *  pTerrainTile.GetHeight())

		self.mHoles = []#new IND_Entity2d [pNumHoles]

		for i in range(0,pNumHoles):
			self.mHoles.append(IndieLib.IND_Factory.GetEntity2d())
	
			IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.mHoles[i])			

			if IndieLib.CIndieLib.Instance().Math.Rand (0, 1):
		
				self.mHoles[i].SetSurface (pHole1)	
		
			else:
		
				self.mHoles[i].SetSurface (pHole2)

			self.mHoles[i].SetBoundingCircle ("area_crater", 250, 250, 270)
			self.mHoles[i].SetBoundingCircle ("area_crater_ufo", 250, 250, 200)

			self.mHoles[i].SetPosition	( IndieLib.CIndieLib.Instance().Math.Rand (0, pNumBlocksX * pTerrainTile.GetWidth()), IndieLib.CIndieLib.Instance().Math.Rand (0, pNumBlocksY * pTerrainTile.GetHeight()), 1)

			self.mScale = IndieLib.CIndieLib.Instance().Math.Rand (50, 100)
			self.mHoles[i].SetScale ( self.mScale / 100.0,  self.mScale / 100.0)

			for j in range(0,i):
				if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision (self.mHoles[i], "area_crater", self.mHoles[j], "area_crater"):
					IndieLib.CIndieLib.Instance().Entity2dManager.Delete (self.mHoles[i])

class CUfo:
	def __init__(self):
		self.mUfo=IndieLib.IND_Factory.GetEntity2d()

	def Init (self, pX, pY, pUfoAnimation):
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.mUfo)
		self.mUfo.SetAnimation (pUfoAnimation)
		self.mUfo.SetPosition (pX, pY, 2)


class AppPy:
	def __init__(self):
		self.X_TILES=8
		self.Y_TILES=6
		self.NUM_HOLES=130
		self.CAMERA_SPEED=300
		self.MOVE_TERRAIN_BY_MOUSE_AREA=20

		self.mZoom = 1.0
		self.mAngle = 0.0
		self.mFpsString = ""
		self.mSpeedRotation = 50
		self.mDelta = 0.0
		#self.CUfo *mUfos = 0
		self.cont = 0
		self.mSurfaceCursor=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceTerrain=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceHole1=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceHole2=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mAnimationUfo=IndieLib.IND_Factory.GetAnimation()
		self.mCursor=IndieLib.IND_Factory.GetEntity2d()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.mCameraTerrain=IndieLib.IND_Factory.GetEntity2d()
		self.mCameraGui=IndieLib.IND_Factory.GetEntity2d()
		self.mTerrain=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("c_01_Alien_BenchMark")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceCursor, "/cursor.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceTerrain, "/tiled_terrain.png", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceHole1, "/hole1.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceHole2, "/hole2.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.mAnimationUfo, "/animations/ufo.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0


		IndieLib.CIndieLib.Instance().Entity2dManager.Add (1, self.mCursor)
	
		self.mCursor.SetSurface (self.mSurfaceCursor)


		self.mTerrain=CTerrain(self.X_TILES, self.Y_TILES, self.NUM_HOLES, self.mSurfaceTerrain, self.mSurfaceHole1, self.mSurfaceHole2)


		IndieLib.CIndieLib.Instance().Entity2dManager.Add (1, self.mTextSmallWhite)

		self.mTextSmallWhite.SetFont (self.mFontSmall)


		self.mTextSmallWhite.SetLineSpacing	(18)

		self.mTextSmallWhite.SetCharSpacing	(-8)

		self.mTextSmallWhite.SetPosition		(15, 15, 4)

		self.mTextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		self.mUfos=[]
		for i in range(0,self.X_TILES * self.Y_TILES * 1024):
			self.mUfos.append(CUfo())


		cont = 0
		for i in range(0, self.X_TILES * 512, 50):
			for j in range(0,self.Y_TILES * 512, 50):
				self.mUfos [cont]=CUfo()
				self.mUfos [cont].Init ( i,  j, self.mAnimationUfo)
				cont+=1

		
		self.mCameraTerrain=IndieLib.IND_Factory.GetCamera2d(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)


		self.mCameraGui=IndieLib.IND_Factory.GetCamera2d(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)
		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()


		self.mFpsString2=""

		self.mFpsString= "Fps:"

		IndieLib.CIndieLib.Instance().Render.GetFpsString(self.mFpsString2, 128)

		self.mFpsString+= self.mFpsString2

		self.mFpsString += "\n"

		self.mFpsString += "Rendered: "

		self.mFpsString += IndieLib.CIndieLib.Instance().Render.GetNumRenderedObjectsString()

		self.mFpsString += "\n"

		self.mFpsString += "Discarded: "

		self.mFpsString += IndieLib.CIndieLib.Instance().Render.GetNumDiscardedObjectsString()

		self.mTextSmallWhite.SetText	 (self.mFpsString)


		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0


		self.mCursor.SetPosition ( IndieLib.CIndieLib.Instance().Input.GetMouseX(),  IndieLib.CIndieLib.Instance().Input.GetMouseY(), 3)


		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_1):
		

			if not IndieLib.CIndieLib.Instance().Render.Reset ("Alien BenchMark", 640, 480, 32, 0, 1):
				return 0

			self.mCameraGui.SetPosition (IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

			self.mCameraTerrain.SetPosition	(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

		


		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_2):
		

			if not IndieLib.CIndieLib.Instance().Render.Reset ("Alien BenchMark", 800, 600, 32, 0, 1):
				return 0

			self.mCameraGui.SetPosition (IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

			self.mCameraTerrain.SetPosition	(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

		


		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_3):
		

			if not IndieLib.CIndieLib.Instance().Render.Reset ("Alien BenchMark", 1024, 768, 32, 0, 1):
				return 0

			self.mCameraGui.SetPosition (IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

			self.mCameraTerrain.SetPosition	(IndieLib.CIndieLib.Instance().Window.GetWidth () / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

		


		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_SPACE):
			IndieLib.CIndieLib.Instance().Render.ToggleFullScreen()


		if IndieLib.CIndieLib.Instance().Input.OnMouseButtonPress	(IndieLib.STX_MBUTTON_WHEELUP):
			self.mZoom += 0.1

		if IndieLib.CIndieLib.Instance().Input.OnMouseButtonPress	(IndieLib.STX_MBUTTON_WHEELDOWN):
			self.mZoom -= 0.1
		

		if IndieLib.CIndieLib.Instance().Input.IsMouseButtonPressed (IndieLib.STX_MBUTTON_LEFT):
			self.mAngle += self.mSpeedRotation * self.mDelta

		if IndieLib.CIndieLib.Instance().Input.IsMouseButtonPressed (IndieLib.STX_MBUTTON_RIGHT):

			self.mAngle -= self.mSpeedRotation * self.mDelta
		

		if self.mZoom < 0.1:
			self.mZoom =  0.1

		self.mCameraTerrain.SetAngle (self.mAngle)

		self.mCameraTerrain.SetZoom (self.mZoom)


		if IndieLib.CIndieLib.Instance().Input.GetMouseX() > IndieLib.CIndieLib.Instance().Window.GetWidth () - self.MOVE_TERRAIN_BY_MOUSE_AREA:
			self.mCameraTerrain.SetPosition (self.mCameraTerrain.GetPosX() +  (self.CAMERA_SPEED * IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0), self.mCameraTerrain.GetPosY())

		if IndieLib.CIndieLib.Instance().Input.GetMouseX() < self.MOVE_TERRAIN_BY_MOUSE_AREA:
			self.mCameraTerrain.SetPosition (self.mCameraTerrain.GetPosX() -  (self.CAMERA_SPEED * IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0), self.mCameraTerrain.GetPosY())

		if IndieLib.CIndieLib.Instance().Input.GetMouseY() > IndieLib.CIndieLib.Instance().Window.GetHeight ()- self.MOVE_TERRAIN_BY_MOUSE_AREA:
			self.mCameraTerrain.SetPosition (self.mCameraTerrain.GetPosX(), self.mCameraTerrain.GetPosY() +  (self.CAMERA_SPEED * IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0))

		if IndieLib.CIndieLib.Instance().Input.GetMouseY() < self.MOVE_TERRAIN_BY_MOUSE_AREA:
			self.mCameraTerrain.SetPosition (self.mCameraTerrain.GetPosX(), self.mCameraTerrain.GetPosY() -  (self.CAMERA_SPEED * IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0))


		IndieLib.CIndieLib.Instance().Render.BeginScene ()

		IndieLib.CIndieLib.Instance().Render.ClearViewPort (0, 0, 0)

		
		IndieLib.CIndieLib.Instance().Render.ResetNumDiscardedObjects()

		IndieLib.CIndieLib.Instance().Render.ResetNumRenderedObject()

		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCameraTerrain)

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (0)


		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.mCameraGui)

		IndieLib.CIndieLib.Instance().Render.BlitFillRectangle (10, 10, 240, 80, 128, 128, 128, 128)

		IndieLib.CIndieLib.Instance().Render.BlitRectangle (10, 10, 240, 80, 255, 255, 255, 128)

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (1)

		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

