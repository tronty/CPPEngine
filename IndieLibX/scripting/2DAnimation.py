#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.name=["mario_stand", "mario_walk", "Mario_jump"]

	def actionPerformed(self):
		IndieLib.UpdateNSGUISWIG()
		loop=not IndieLib.isGUIEventsEmpty()
		while loop:
			evt=IndieLib.PollGUIEvent()
			loop=not IndieLib.isGUIEventsEmpty()
			if evt:
				callbackString=evt.getCallbackCString()
				if callbackString=="Unknown":
					continue
				if IndieLib.isClicked(evt):
					for i in range(0,len(self.name)):
      						if callbackString == self.name[i]:
							#print i
							self.anim.SetSequence(self.name[i])
							break

	def init(self, width, height):
		self.anim=IndieLib.FW3.GetInstance().GetAnimation("/2DAnimation/mario.xml")

		IndieLib.InitNSGUISWIG("/2DAnimation/GUILayout.xml")

		self.anim.SetSequence("mario_stand")
		#self.anim.SetSequence("mario_walk")
		#self.anim.SetSequence("Mario_jump")
		self.anim.SetPosition(IndieLib.IRenderer.GetRendererInstance().GetWidth()/2-25.0, IndieLib.IRenderer.GetRendererInstance().GetHeight()/2-25.0)
		self.anim.SetSize(50.0,50.0)
	def render(self):
		IndieLib.IRenderer.GetRendererInstance().Clear( True, True, IndieLib.D3DXFROMWINEVECTOR4(0.35, 0.53, 0.7, 1.0))
		IndieLib.IRenderer.GetRendererInstance().BeginScene()
		self.actionPerformed()

		self.anim.render()

		IndieLib.IRenderer.GetRendererInstance().EndScene()
		IndieLib.IRenderer.GetRendererInstance().Present( )

if __name__=="__main__":
	app=AppPy()
	IndieLib.IRenderer.GetRendererInstance("2DAnimation")
	app.init(640, 512)
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()

