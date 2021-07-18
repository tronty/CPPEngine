#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		return

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_01_Installing")

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		#IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle()
		return 0

	def run(self):
		self.init(0,0)
		while True:
			if IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
				return
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

