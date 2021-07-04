#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		return
	
	def init(self):
		IndieLib.InitNSGUISWIG("/3DAnimation/GUILayout.xml")
		self.v=[]
		self.v.append("/trex_dx/dino_videogame.x")
		self.v.append("/trex_dx/dino_videogame2.x")
		self.v.append("/skeleton/skeleton.x")
		self.v.append("/assimp--1.0.412-sdk/test.x")
		self.v.append("/assimp--1.0.412-sdk/dwarf.x")
		self.v.append("/AdvAniCh12/backdrop.x")
		self.v.append("/AdvAniCh12/chopper.X")
		self.v.append("/AdvAniCh12/Rotor.X")
		self.v.append("/AdvAniCh12/Shadow.X")
		# Following five models require:
		# 	the newest version of libMeshRenderer2.a static library.
		#	the newest version of IndieLib_resources.zip package.
		self.v.append("/dx9sdk/cube.x")
		self.v.append("/dx9sdk/sphere.x")
		self.v.append("/dx9sdk/teapot.x")
		self.v.append("/dx9sdk/tiger.x")
		self.v.append("/dx9sdk/tiny.x")
		self.v.append("/DXJune2010/Dwarf/Dwarf.x")
		#self.v.append("/DXJune2010/Dwarf/DwarfWithEffectInstance.x")
		#self.v.append("/PM/P-TankHistory/allied/t34_76/t34ds/T34-76.3DS")
		self.m_Mesh=[]
		self.mAngleX = 0
		self.mAngleY = 0
		self.mSpeedRotation = 5
		self.mDelta = 0
		self.i=0
		c=0
		for i in self.v:
			self.m_Mesh.append(IndieLib.MeshRenderer())
			self.m_Mesh[c].Load(self.v[c], IndieLib.eSimpleShader);
			self.m_Mesh[c].SetPlay(False)
			self.m_Mesh[c].SetAnimIndex(0)
			self.m_Mesh[c].SetPlay(True)
			c=c+1
	
	def actionPerformed_(self):
		if(IndieLib.STX_Service.GetInputInstance().OnKeyPress(IndieLib.STX_KEY_PAGEUP)):
			self.i=self.i+1
			if(self.i>(len(self.v)-1)):
				self.i=0
		elif(IndieLib.STX_Service.GetInputInstance().OnKeyPress(IndieLib.STX_KEY_PAGEDOWN)):
			if(self.i==0):
				self.i=len(self.v)-1
			else:
				self.i=self.i-1
		return
	
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
					if callbackString=="dino_videogame":
						self.i=0
					elif callbackString=="dino_videogame2":
						self.i=1
					elif callbackString=="skeleton":
						self.i=2
					elif callbackString=="test":
						self.i=3
					elif callbackString=="dwarf":
						self.i=4
					elif callbackString=="backdrop":
						self.i=5
					elif callbackString=="chopper":
						self.i=6
					elif callbackString=="Rotor":
						self.i=7
					elif callbackString=="Shadow":
						self.i=8
						
					elif callbackString=="cube":
						self.i=9
					elif callbackString=="sphere":
						self.i=10
					elif callbackString=="teapot":
						self.i=11
					elif callbackString=="tiger":
						self.i=12
					elif callbackString=="tiny":
						self.i=13
						
					elif callbackString=="Dwarf":
						self.i=14

	
	def render(self):
		IndieLib.IRenderer.GetRendererInstance().Clear(True, True, IndieLib.D3DXFROMWINEVECTOR4(0.35, 0.53, 0.7, 1.0))
		IndieLib.IRenderer.GetRendererInstance().BeginScene()

		self.mDelta = IndieLib.timeGetTime() / 1000.0
		if (IndieLib.STX_Service.GetInputInstance().IsMouseButtonPressed (IndieLib.STX_MBUTTON_LEFT)):
			self.mAngleX += self.mSpeedRotation * self.mDelta
		if (IndieLib.STX_Service.GetInputInstance().IsMouseButtonPressed (IndieLib.STX_MBUTTON_RIGHT)):
			self.mAngleY += self.mSpeedRotation * self.mDelta

		matRot=IndieLib.D3DXFROMWINEMATRIX()
		pi=3.141592654
		IndieLib.D3DXFROMWINEMatrixRotationYawPitchRoll(matRot, self.mAngleX*pi/180.0, self.mAngleY*pi/180.0, 0.0)

		#w=IndieLib.D3DXFROMWINEMATRIX()
		#IndieLib.D3DXFROMWINEMatrixIdentity(w)
		self.m_Mesh[self.i].render(matRot)

		#txt = "Use mouse buttons for rotating the model."
		#IndieLib.IRenderer.drawText(txt, 10, 10, 15, 18)
		#, IndieLib.IRenderer.GetdefaultFont(), IndieLib.IRenderer.GetlinearClamp(), IndieLib.IRenderer.GetblendSrcAlpha(), IndieLib.IRenderer.GetnoDepthTest())

		self.actionPerformed()

		IndieLib.IRenderer.GetRendererInstance().EndScene()
		IndieLib.IRenderer.GetRendererInstance().Present()

if __name__=="__main__":
	app=AppPy()
	IndieLib.IRenderer.GetRendererInstance("3D2Animation")
	app.init()
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()


