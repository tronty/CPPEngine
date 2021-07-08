/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
using com.IndieLib.swig;

public class a_01_Installing {
	static void Main() {
		init(0,0);
		while(true) {
			if(STX_Service.GetInputInstance().IsKeyDown(STX_KEY_ESCAPE))
				return;
			render();
		}
	}

	static void init(int width, int height) {
		IndieLib.CIndieLib.Instance();
		IndieLib.CIndieLib.Instance().Init("a_01_Installing");
	}

	static int render() {
		IndieLib.CIndieLib.Instance().Input.Update ();
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60);
		IndieLib.CIndieLib.Instance().Render.BeginScene ();
		IndieLib.CIndieLib.Instance().Render.EndScene ();
		//IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle();
		return 0;
	}
}	

