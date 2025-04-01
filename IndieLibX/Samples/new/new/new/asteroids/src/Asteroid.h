#ifndef __Asteroid__
#define __Asteroid__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

	/**
	 * ...
	 * @author Chris Moeller
	 */
	 struct Asteroid  
	{
		
		  const std::vector<Directions> = {D3DXFROMWINE_PI / 4, 3 * D3DXFROMWINE_PI / 4, 5 * D3DXFROMWINE_PI / 4, 7 * D3DXFROMWINE_PI / 4};
		  const std::vector<int> Sizes= [40, 20, 15};
		  const std::vector<Speeds> = {1, 2, 4};
		  const std::vector<Types> = {0, 1};
		  const std::vector<Scores> = {20, 50, 100};
		
		  int type;
		  float direction;
		  D3DXFROMWINEVECTOR2 speed;
		  int size;
		
		  TextureID tex_asteroid1;
		  TextureID tex_asteroid2;
		
		  Asteroid(int x, int y, float size, float direction, int type)
		{
			//super(x, y, Sizes[size], Sizes[size], 0);
			this->size = size;
			this->type = type;
			this->direction = direction = Directions[direction];
			this->speed = D3DXFROMWINEVECTOR2( -Speeds[size] * cosf(direction), -Speeds[size] * sinf(direction));
			//trace("speed asteroid, x, y = "+(-Speeds[size] * cosf(direction))+", "+(-Speeds[size] * sinf(direction))+", direction = "+direction );
			
			image_sprite = Sprite();
			this->type = type;
			
			if (type == 0)
			{
                tex_asteroid1=IRenderer::GetRendererInstance()->addImageLibTexture("/new/asteroids/asteroid1.png");
			    TextureID temp_bitmap = tex_asteroid1;
				temp_bitmap.width = temp_bitmap.height = Sizes[size];
				image_sprite.addChild(temp_bitmap);
				
			}
			else if (type == 1)
			{
                tex_asteroid2=IRenderer::GetRendererInstance()->addImageLibTexture("/new/asteroids/asteroid2.png");
			    TextureID temp_bitmap2 = tex_asteroid2;
				temp_bitmap2.width = temp_bitmap2.height = Sizes[size];
				image_sprite.addChild(temp_bitmap2);			
			}
		}
		   void Render()
		{
			IRenderer::GetRendererInstance()->drawTexture(image_sprite, x, y);
			
			//Render();
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
				
			//Update();
		}
		
	};
#endif

