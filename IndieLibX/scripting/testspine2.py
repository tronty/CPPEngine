#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class TestSpine2:
	def __init__(self):
		
		self.name=["raptor", "hero", "powerup", "spineboy", "speedy", "alien", "goblins", "dragon", "spineboy-old"]
		self.state=["walk", "Walk", "???", "run", "run", "run", "walk", "flying", "walk"]
		self.y=[590, 460, 460, 460, 460, 460, 460, 400, 460]
		self.mouseLocked=False
    		self.guiFrame=0
    		self.userControls=0
		self.irender=3
		self.atlas=[]
		self.skeletonData=[]
		self.drawable=[]
	def init2(self, ai):
		atlasfile="/spine/%s/export/%s.atlas" % (self.name[ai], self.name[ai])
		jsonfile="/spine/%s/export/%s.json" % (self.name[ai], self.name[ai])
		self.atlas.append(IndieLib.spAtlas_createFromFile(atlasfile, 0))
		json = IndieLib.spSkeletonJson_create(self.atlas[len(self.atlas)-1])
		json.scale = 0.5
		self.skeletonData.append(IndieLib.spSkeletonJson_readSkeletonDataFile(json, jsonfile))
		if not self.skeletonData[len(self.skeletonData)-1]:
			cl="Error: %s\n" % json.error
			print cl
			exit(0)
	
		SkeletonJson_dispose(json)
		drawable.append(SkeletonDrawable(skeletonData[len(self.skeletonData)-1]))
		drawable[len(drawable)-1].timeScale = 1
		skeleton = drawable[len(drawable)-1].skeleton
		skeleton.x = 320
		skeleton.y = y[len(drawable)-1]
		Skeleton_updateWorldTransform(skeleton)
		AnimationState_setAnimationByName(drawable[len(drawable)-1].state, 0, state[len(drawable)-1], True)

	def init(self, width, height):
		self.init2(0)
		self.init2(1)
		self.init2(3)
		self.init2(4)
		self.init2(5)
		self.init2(6)
		self.init2(7)
		self.init2(8)		
  		IndieLib.InitNSGUISWIG("/spine/GUILayout2.xml")
		self.irender=3

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
					self.irender=0
					for i in range(0,len(self.name)):
						if i == 2:
							continue
      						if callbackString == self.name[i]:
							self.irender=i
							break

	def render(self):
		f=128.0
		IndieLib.IRenderer.GetRendererInstance().Clear(True, False, IndieLib.D3DXFROMWINEVECTOR4 (f/256.0, f/256.0, f/256.0, 1.0))
		IndieLib.IRenderer.GetRendererInstance().BeginScene()
		self.actionPerformed()

		delta = timeGetTime()/1000000.0

		drawable[self.irender].update(delta)			
		drawable[self.irender].draw()

		IndieLib.IRenderer.GetRendererInstance().EndScene()
		IndieLib.IRenderer.GetRendererInstance().Present( )

if __name__=="__main__":
	app=TestSpine2()
	app.init(640, 512)
	IndieLib.IRenderer.GetRendererInstance("testspine2")		
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()

