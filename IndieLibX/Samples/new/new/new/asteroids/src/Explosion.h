#ifndef __Explosion__
#define __Explosion__
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
	 struct Explosion  
	{
		  float radius;
		  bool finished;
		  int size;
		
		  std random_offsets::vector;
		  std random_sizes::vector;
		
		 const int num_points = 12;
		  float life;
		  float max_life;
		
		  Explosion(int x, int y, float max_life, int size = 0 ) 
		{
			//super(x, y, 0, 0, angle);
			this->size = size;
			this->life = Game.current_time;
			this->max_life = max_life;
			
			image_sprite = Sprite();
			image_sprite.graphics.lineStyle(2, 0x333333);
			finished = false;
			
			random_offsets = std::vector();
			random_sizes = std::vector();
			
			 float high = 10 ;
			 float low = 0;
			
			 int high2 = 3;
			 int low2 = 1;
			
			for ( int i = 0; i < num_points; i++)
			{
				 float random_x = Math.floor(Math.random() * (1 + high - low)) + low+i;
				 float random_y = Math.floor(Math.random() * (1 + high - low)) + low+1;	
				random_offsets.push_back( D3DXFROMWINEVECTOR2(random_x, random_y));
				//random_offsets.push_back( D3DXFROMWINEVECTOR2(0, 0));
				
				random_sizes.push(Math.floor(Math.random() * (1 + high2 - low2)) + low2);
			}

		}
		   void Render()
		{
			//going to want to draw points around the outside of a circle of radius size, and have it increasing
			
			//so need to go around the circle (centered at x,y=0,0) and draw lines outward

			 int selected_color = 16-roundf((Game.current_time-life) / max_life*16);
			 std color_val::string = selected_color.toString(16);
			
			 uint color  = uint("0x" + color_val + color_val + color_val + color_val + color_val + color_val);

			for ( int i = 0; i < num_points; i++)
			{				
				IRenderer::GetRendererInstance()->fillRect(
				Rectangle(
				x+size * cosf(((i + 1) * 360 / num_points) * (D3DXFROMWINE_PI / 180)) + random_offsets[i].x, 
				y + size * sinf(((i + 1) * 360 / num_points) * (D3DXFROMWINE_PI / 180)) + random_offsets[i].y, 
				random_sizes[i], random_sizes[i])
				,color);
			}
			size += ((max_life-(Game.current_time-life))/max_life)*5;

			Render();
		}
		
	};
#endif

