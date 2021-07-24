#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class FloatingText:
	def __init__(self):
		self.m_szText=""
		self.m_pFont=IndieLib.IND_Factory.GetFont()
		self.m_Entity2d=IndieLib.IND_Factory.GetEntity2d()
		self.m_vecPosition=IndieLib.D3DXFROMWINEVECTOR3()
		self.m_vecVelocity=IndieLib.D3DXFROMWINEVECTOR3()
		self.m_fFadeTimer=0.0
		self.m_fFadeDelay=0.0
		self.m_fLifetime=0.0
		self.m_fAlpha=0.0
		self.m_bIsDead=False	

	def IsDead(self):
		return self.m_bIsDead

	def Init(self, Text, layer, FadeDelay, vecPosition, vecVelocity):
		self.m_fFadeDelay	= FadeDelay
		self.m_vecPosition	= vecPosition
		self.m_vecVelocity	= vecVelocity

		self.m_pFont=IndieLib.IND_Factory.GetFont()
		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.m_pFont, "Army_05.png", "Army_05.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return False

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (layer, self.m_Entity2d)
		self.m_Entity2d.SetFont (self.m_pFont)

		self.m_Entity2d.SetShow(True)

		self.m_Entity2d.SetLineSpacing	(0)
		self.m_Entity2d.SetCharSpacing	(0)
		self.m_Entity2d.SetAlign			(IndieLib.IND_CENTER)
		self.m_Entity2d.SetTransparency(255)
		self.m_Entity2d.SetHotSpot(0.5, 0.5)

		self.m_szText = Text
		self.m_Entity2d.SetText			(self.m_szText)

		return True

	def Init(self, Text, pFont, layer, FadeDelay, vecPosition, vecVelocity):

		self.m_fFadeDelay	= FadeDelay
		self.m_vecPosition	= vecPosition
		self.m_vecVelocity	= vecVelocity

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (layer, self.m_Entity2d)
		self.m_Entity2d.SetFont (pFont)
		self.m_Entity2d.SetTransparency(255)
		self.m_Entity2d.SetShow(True)

		self.m_Entity2d.SetLineSpacing	(0)
		self.m_Entity2d.SetCharSpacing	(0)
		self.m_Entity2d.SetAlign			(IndieLib.IND_CENTER)

		self.m_szText = Text
		self.m_Entity2d.SetText			(self.m_szText)

		return True

	def Update(self, fElapsedTime):

		if not self.m_bIsDead:
	
			self.m_fFadeTimer += fElapsedTime

			self.m_fAlpha =  (255 * ((self.m_fFadeDelay - self.m_fFadeTimer) / self.m_fFadeDelay))
			self.m_Entity2d.SetTransparency(self.m_fAlpha)
		
		self.m_vecPosition += self.m_vecVelocity * fElapsedTime
		self.m_Entity2d.SetPosition(self.m_vecPosition.x, self.m_vecPosition.y, self.m_vecPosition.z)

		if self.m_fAlpha <= 0.0:
			self.m_bIsDead = True	

class AppPy:
	def __init__(self):
		self.FADE_DELAY	= 3.0
		self.fElapsedTime = 0.0
		self.szPoints=""
		self.timer=IndieLib.IND_Factory.GetTimer()
		self.Font=IndieLib.IND_Factory.GetFont()
		self.guiText=IndieLib.IND_Factory.GetEntity2d()
		self.pFloatingText = 0

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-FloatingText")
		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.Font, "/IndieLib-FloatingText/Army_05.png", "/IndieLib-FloatingText/Army_05.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return False

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.guiText)
		self.guiText.SetFont(self.Font)
		self.guiText.SetPosition(0,0,0)
		self.guiText.SetCharSpacing(0)
		self.guiText.SetAlign(IndieLib.IND_LEFT)

		self.timer.Start()

		IndieLib.CIndieLib.Instance().Window.Cursor(True)
		return 0

	
	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.guiText.SetText("Left mouse creates floating text")

		self.fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if self.pFloatingText == 0 and IndieLib.CIndieLib.Instance().Input.OnMouseButtonPress(IndieLib.STX_MBUTTON_LEFT):
			self.szPoints="%d" % IndieLib.CIndieLib.Instance().Input.GetMouseX()
			
			self.pFloatingText = FloatingText()
			
			self.pFloatingText.Init(
				self.szPoints,
				self.Font,
				0,
				2.0,
				IndieLib.D3DXFROMWINEVECTOR3(IndieLib.CIndieLib.Instance().Input.GetMouseX(), IndieLib.CIndieLib.Instance().Input.GetMouseY(),0),
				IndieLib.D3DXFROMWINEVECTOR3(0.0, -60.0, 0))

		
		
		if self.pFloatingText:
		

			self.pFloatingText.Update(self.fElapsedTime)

			if self.pFloatingText.IsDead():

				self.pFloatingText = 0
			
		

		IndieLib.CIndieLib.Instance().Render.BeginScene ()

		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( )

		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

