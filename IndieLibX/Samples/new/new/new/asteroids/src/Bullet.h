#ifndef __Bullet__
#define __Bullet__
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

	/**
	 * ...
	 * @author Chris
	 */
	 struct Bullet  
	{
		  D3DXFROMWINEVECTOR2 speed;
		  float max_speed;
		  float life;
		  Bullet(int x, int y, float life, float angle = 0) 
		{
			//super(x, y, 2, 2, angle);
			max_speed = 10;
			image = Image3(width, height, false, 0xFFFFFF);
			this->speed = D3DXFROMWINEVECTOR2(max_speed*sinf(angle), -max_speed*cosf(angle));
			this->life = life;
			this->angle = angle;
		}
		   void Render()
		{
			IRenderer::GetRendererInstance()->copyPixels(image, Rectangle(0, 0, width, height), D3DXFROMWINEVECTOR2(x, y));
			Render();			
		}
		   void Update()
		{

			x += speed.x;
			y += speed.y;	
			
			if (x + width <= 0)
				x = STX_Service::GetWindowInstance()->GetWidth() - width;
			else if(x >= STX_Service::GetWindowInstance()->GetWidth())
				x = 0;
				
			if (y + height <= 0)
				y = STX_Service::GetWindowInstance()->GetHeight() - height;
			else if(y >= STX_Service::GetWindowInstance()->GetHeight())
				y = 0;
			
			Update();
			
		}
		
	};
#endif

