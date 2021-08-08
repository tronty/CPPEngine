#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.fElapsedTime = 0.0
		self.bShowBounds = False
		self.bToggleRes = False
		self.speed = 1500.0
		self.btnMissionAlpha = 0
		self.btnSurvivalAlpha = 0
		self.buttonBrightness = 180
		self.fScreenFlashTimer = 0.0
		self.fMAX_FLASH = 0.15
		self.szText=""

		self.timer=IndieLib.IND_Factory.GetTimer()
		self.surfaceRed=IndieLib.IND_Factory.GetSurface()
		self.surfaceBG=IndieLib.IND_Factory.GetSurface()
		self.surfaceHightlight=IndieLib.IND_Factory.GetSurface()
		self.surfaceCursor=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.TextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.entityBG=IndieLib.IND_Factory.GetEntity2d()
		self.entityRed=IndieLib.IND_Factory.GetEntity2d()
		self.btnTargetMission=IndieLib.IND_Factory.GetEntity2d()
		self.btnTargetSurvival=IndieLib.IND_Factory.GetEntity2d()
		self.entityCursor=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-Blend")

		IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceRed, "/IndieLib-Blend/red.png", IndieLib.IND_ALPHA, IndieLib.IND_32)

		IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBG, "/IndieLib-Blend/mainmenu.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32)

		IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceHightlight, "/IndieLib-Blend/mainbigbtn.png", IndieLib.IND_ALPHA, IndieLib.IND_32)

		IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceCursor, "/IndieLib-Blend/cursor_arrow.png", IndieLib.IND_ALPHA, IndieLib.IND_32)

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/IndieLib-Blend/font_small.png", "/IndieLib-Blend/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (2, self.TextSmallWhite)
		self.TextSmallWhite.SetFont (self.mFontSmall)
		self.TextSmallWhite.SetLineSpacing	(18)
		self.TextSmallWhite.SetCharSpacing	(-8)
		self.TextSmallWhite.SetPosition		(15, 15, 4)
		self.TextSmallWhite.SetAlign			(IndieLib.IND_LEFT)


		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.entityBG)
		self.entityBG.SetSurface(self.surfaceBG)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(2, self.entityRed)
		self.entityRed.SetShow(False)
		self.entityRed.SetTransparency(127)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.btnTargetMission)
		self.btnTargetMission.SetSurface(self.surfaceHightlight)
		self.btnTargetMission.SetTransparency(0)
		self.btnTargetMission.SetPosition(100, 250, 0)
		self.btnTargetMission.SetHotSpot(0.5, 0.5)
		self.btnTargetMission.SetBoundingCircle("btnMission", 64, 64, 50)
		self.btnTargetMission.SetBoundingRectangle("no collision", 0, 0, 128, 128)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.btnTargetSurvival)
		self.btnTargetSurvival.SetSurface(self.surfaceHightlight)
		self.btnTargetSurvival.SetTransparency(0)
		self.btnTargetSurvival.SetPosition(230, 380, 0)
		self.btnTargetSurvival.SetHotSpot(0.5, 0.5)
		self.btnTargetSurvival.SetBoundingCircle("btnSurvival", 64, 64, 50)
		self.btnTargetSurvival.SetBoundingRectangle("no collision", 0, 0, 128, 128)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.entityCursor)
		self.entityCursor.SetSurface(self.surfaceCursor)
		self.entityCursor.SetBoundingCircle("", 27, 27, 3)
		self.entityCursor.SetHotSpot(0.5, 0.5)
		self.timer.Start()
		return 0

	def render(self):
		szText ="Press 'b' to toggle display bounds\n"
		szText+="Press 'f' to red flash the screen\n"
		szText+="Press ESC' to quit"
		self.TextSmallWhite.SetText	 (szText)

		IndieLib.CIndieLib.Instance().Input.Update ()

		fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		self.entityCursor.SetPosition(IndieLib.CIndieLib.Instance().Input.GetMouseX(), IndieLib.CIndieLib.Instance().Input.GetMouseY(), 1)

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_f):
			fScreenFlashTimer = fMAX_FLASH
			self.entityRed.SetShow(True)

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_b):
			bShowBounds = not bShowBounds

		if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision(self.entityCursor, "", self.btnTargetMission, "btnMission"):
			self.btnTargetMission.SetShow(True)

			self.entityBG.SetBlendSource(IndieLib.IND_SRCALPHA)
			self.entityBG.SetBlendDest(IndieLib.IND_INVSRCALPHA)
			self.btnTargetMission.SetBlendSource(IndieLib.IND_SRCALPHA)
			self.btnTargetMission.SetBlendDest(IndieLib.IND_ONE)

			self.btnMissionAlpha += self.fElapsedTime * self.speed
			if self.btnMissionAlpha > self.buttonBrightness:
				self.btnMissionAlpha = self.buttonBrightness

				self.btnTargetMission.SetTransparency(self.btnMissionAlpha)
			else:
				self.btnTargetMission.SetShow(False)
				self.btnMissionAlpha = 0

		if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision(self.entityCursor, "", self.btnTargetSurvival, "btnSurvival"):
			self.btnTargetSurvival.SetShow(True)

			self.entityBG.SetBlendSource(IndieLib.IND_SRCALPHA)
			self.entityBG.SetBlendDest(IndieLib.IND_INVSRCALPHA)
			self.btnTargetSurvival.SetBlendSource(IndieLib.IND_SRCALPHA)
			self.btnTargetSurvival.SetBlendDest(IndieLib.IND_ONE)

			self.btnSurvivalAlpha += self.fElapsedTime * self.speed
			if self.btnSurvivalAlpha > self.buttonBrightness:
				self.btnSurvivalAlpha = self.buttonBrightness

			self.btnTargetSurvival.SetTransparency(self.btnSurvivalAlpha)
		else:
			self.btnTargetSurvival.SetShow(False)
			self.btnSurvivalAlpha = 0

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( )

		if self.bShowBounds:
			IndieLib.CIndieLib.Instance().Entity2dManager.RenderCollisionAreas(255, 0, 0, 255)

		self.fScreenFlashTimer -= self.fElapsedTime
		if self.fScreenFlashTimer > 0.0:
			alpha = (127 - (127 * (fMAX_FLASH - self.fScreenFlashTimer)))
			if alpha < 0:
				alpha = 0
			self.entityRed.SetTransparency(alpha)
		else:
			self.entityRed.SetShow(False)
			self.fScreenFlashTimer = 0.0

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (1)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (2)
		#???IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle()
		IndieLib.CIndieLib.Instance().Render.EndScene ()		
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

