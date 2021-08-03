#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class CRabbit:
	def __init__(self):
		self.mZ=0
		self.mRabbit=IndieLib.IND_Factory.GetEntity2d()
		#self.mScreenWidth=0
		#self.mScreenHeight=0
		self.mSpeed=0.0
		self.mDir=0
		self.mSpeedRotation=0.0
		self.mScreenWidth = IndieLib.CIndieLib.Instance().Render.GetViewPortWidth()
		self.mScreenHeight = IndieLib.CIndieLib.Instance().Render.GetViewPortHeight()
		self.RIGHT_DOWN	= 0
		self.RIGHT_UP	= 1
		self.LEFT_DOWN	= 2
		self.LEFT_UP	= 3

	def Init (self, pSurfaceRabbit, pZ):
		self.mZ = pZ

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mRabbit)	
		self.mRabbit.SetSurface (pSurfaceRabbit)

		self.mRabbit.SetPosition ( IndieLib.CIndieLib.Instance().Math.Rand (0, self.mScreenWidth),  				IndieLib.CIndieLib.Instance().Math.Rand (0, self.mScreenHeight), self.mZ)

		if IndieLib.CIndieLib.Instance().Math.Rand (0, 5) != 5:
			self.mRabbit.SetTint ( IndieLib.CIndieLib.Instance().Math.Rand (128, 255), IndieLib.CIndieLib.Instance().Math.Rand (128, 255), IndieLib.CIndieLib.Instance().Math.Rand (128, 255))

		self.mRabbit.SetHotSpot (0.5, 0.5)

		self.mScale = IndieLib.CIndieLib.Instance().Math.Rand (50, 150)
		self.mRabbit.SetScale ( self.mScale / 100.0,  self.mScale / 100.0)

		self.mDir = IndieLib.CIndieLib.Instance().Math.Rand (0, 3)

		self.mSpeedRotation =  IndieLib.CIndieLib.Instance().Math.Rand (-200, 200)

		self.mSpeed =  IndieLib.CIndieLib.Instance().Math.Rand (100, 200)

	def Update():

		self.mDirX=0.0
		self.mDirY=0.0

		self.mDelta =   IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000
		
		if self.mDir==RIGHT_DOWN:
		
			self.mDirX = self.mDelta * self.mSpeed
			self.mDirY = self.mDelta * self.mSpeed

			if self.mRabbit.GetPosX() > self.mScreenWidth:
						mDir = LEFT_DOWN

			if self.mRabbit.GetPosY() > self.mScreenHeight:
						mDir = RIGHT_UP

			
		

		elif mDir==LEFT_DOWN:
		
			self.mDirX = -self.mDelta * self.mSpeed
			self.mDirY = self.mDelta * self.mSpeed

			if self.mRabbit.GetPosX() < 0:
				self.mDir = RIGHT_DOWN

			if self.mRabbit.GetPosY() > self.mScreenHeight:
				self.mDir = LEFT_UP

			
		


		elif self.mDir==RIGHT_UP:
		
			self.mDirX = self.mDelta * self.mSpeed
			self.mDirY = -self.mDelta * self.mSpeed

			if self.mRabbit.GetPosX() > self.mScreenWidth:
				self.mDir = LEFT_UP

			if self.mRabbit.GetPosY() < 0:
				self.mDir = RIGHT_DOWN

			
		


		elif self.mDir==LEFT_UP:
		
			self.mDirX = -self.mDelta * self.mSpeed
			self.mDirY = -self.mDelta * self.mSpeed

			if self.mRabbit.GetPosX() < 0:
				self.mDir = RIGHT_UP

			if self.mRabbit.GetPosY() < 0:
				self.mDir = LEFT_DOWN

			
		
	

		self.mRabbit.SetPosition (self.mRabbit.GetPosX() + self.mDirX,  self.mRabbit.GetPosY() + self.mDirY, self.mZ)
		self.mRabbit.SetAngleXYZ (0, 0, self.mRabbit.GetAngleZ() + self.mSpeedRotation * self.mDelta)


class AppPy:
	def __init__(self):
		self.MAX_OBJECTS = 1000

		self.mRabbits = 0
		self.mSurfaceRabbit=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("c_02_Rabbits_BenchMark")

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceRabbit, "/rabbit.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		self.mRabbits = []
		for i in range(0,self.MAX_OBJECTS):
			self.mRabbits.append(CRabbit())

		i = 0 
		while i < self.MAX_OBJECTS:
			self.mRabbits[i].Init (self.mSurfaceRabbit, i)		
		i=i+1

		IndieLib.CIndieLib.Instance().Entity2dManager.Add	(self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont		(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition	(5, 5, i)
		self.mTextSmallWhite.SetAlign	(IndieLib.IND_LEFT)

		self.mFpsString = ""
		return 0

	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		#mFpsString, "Fps: ", 128)
		#IndieLib.CIndieLib.Instance().Render.GetFpsString(mFpsString2,128)
		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_SPACE):
			IndieLib.CIndieLib.Instance().Render.ToggleFullScreen()

		i = 0
		while i < MAX_OBJECTS:
			self.mRabbits[i].Update()
			i=i+1

		

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
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

