#ifndef __Ship__
#define __Ship__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

	 struct Ship
	{
		  int rotate_amount;
		
		  D3DXFROMWINEVECTOR2 speed;
		  float speed_multi;
		  float friction;
		
		  float ship_death_time;

		  TextureID tex_ship;
		
		  Ship(int x, int y, int width, int height)
		{
			//super(x, y, width, height);
            speed_multi = .5;
		    friction = .95;
			image_sprite = Sprite();
			//image_sprite.graphics.lineStyle(2, 0xFFFFFF);
			rotate_amount = 15;
			speed =  , D3DXFROMWINEVECTOR2(0, 0);
			rotate_offset = D3DXFROMWINEVECTOR2(width / 2, height / 2);
			
            tex_ship=IRenderer::GetRendererInstance()->addImageLibTexture("/new/asteroids/ship.png");
			TextureID temp_bitmap = tex_ship;
			image_sprite.addChild(temp_bitmap);
			
			//create the ship drawing
			/*image_sprite.graphics.moveTo(0, height);
			image_sprite.graphics.lineTo(width / 2, 0);
			image_sprite.graphics.lineTo(width, height);
			
			//draw the line across
			image_sprite.graphics.moveTo((7*height/8 -height)/(-height/(width/2)), 7*height/8 );
			image_sprite.graphics.lineTo((7 * height / 8 -height) / (height / (width / 2)) + width, 7 * height / 8);
			*/
			ship_death_time = 0;
		}
		   void Render()
		{
			if (!visible)
				return;
				
			 D3DXFROMWINEMATRIX matrix = D3DXFROMWINEMATRIX();
			matrix.translate(-rotate_offset.x, -rotate_offset.y);
			matrix.rotate(angle);
			matrix.translate(rotate_offset.x, rotate_offset.y);
			matrix.translate(x, y);
			
			IRenderer::GetRendererInstance()->draw(image_sprite, matrix);
		}
		
		   void Update()
		{
			x += speed.x;
			y += speed.y;	
			speed.x *= friction;
			speed.y *= friction;
			 int angle_deg = angle * (180.0 / D3DXFROMWINE_PI);
			
			if (x + width <= 0)
				x = STX_Service::GetWindowInstance()->GetWidth - width;
			else if(x >= STX_Service::GetWindowInstance()->GetWidth)
				x = 0;
				
			if (y + height <= 0)
				y = STX_Service::GetWindowInstance()->GetHeight - height;
			else if(y >= STX_Service::GetWindowInstance()->GetHeight)
				y = 0;
				
			//now find the collision points
			 int x_top = 0 * cosf(angle) + rotate_offset.y * sinf(angle)+x+rotate_offset.x;
			 int y_top = 0 * sinf(angle) - rotate_offset.y * cosf(angle) + y + rotate_offset.y;
			
			 int x_b_left = (-rotate_offset.x) * cosf(angle) - (height-rotate_offset.y) * sinf(angle)+x+rotate_offset.x;
			 int y_b_left = (-rotate_offset.x) * sinf(angle) + (height-rotate_offset.y)  * cosf(angle) + y + rotate_offset.y;
			
			 int x_b_right = (rotate_offset.x) * cosf(angle) - (height-rotate_offset.y) * sinf(angle)+x+rotate_offset.x;
			 int y_b_right = (rotate_offset.x) * sinf(angle) + (height - rotate_offset.y) * cosf(angle) + y + rotate_offset.y;
			
			collis_points = [, D3DXFROMWINEVECTOR2(x_top, y_top), D3DXFROMWINEVECTOR2(x_b_left, y_b_left), D3DXFROMWINEVECTOR2(x_b_right, y_b_right)];
		}
		   bool CheckIfInNonRotatedRect(GameSprite obj2)
		{
			 bool intersecting = false;
			if (!visible)
				return false;
			
			
			if (obj2.angle == 0)
			{
				for ( int i = 0; i < collis_points.length; i++)
					if (collis_points[i].x >= obj2.x && collis_points[i].x <= obj2.x + obj2.width)
						if (collis_points[i].y >= obj2.y && collis_points[i].y <= obj2.y + obj2.height)
							return true;
			}
			return intersecting;
			
		}
		 void RotateLeft()
		{
			//first convert angle in rads
			 int angle_deg = roundf(angle * (180.0 / D3DXFROMWINE_PI));
			angle_deg -= rotate_amount;
			angle = angle_deg * (D3DXFROMWINE_PI / 180.0);
		}
		 void RotateRight()
		{
			 int angle_deg = angle * (180.0 / D3DXFROMWINE_PI);
			angle_deg += rotate_amount;
			angle = angle_deg * (D3DXFROMWINE_PI / 180.0);
		}
		 void Thrust(int dir=1)
		{			
			 int angle_deg = angle * (180.0 / D3DXFROMWINE_PI);
			if (dir == 1)
			{
				speed.x +=speed_multi * sinf(angle);
				speed.y -= speed_multi * cosf(angle);		
			}
			else
			{
				speed.x -=speed_multi * sinf(angle);
				speed.y += speed_multi * cosf(angle);		
			}
		}	
	};
#endif

