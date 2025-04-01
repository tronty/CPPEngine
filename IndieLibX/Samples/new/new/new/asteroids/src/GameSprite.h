/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

	 struct GameSprite
	{
	   
		  float x;
		  float y;
		  int width;
		  int height;
		  float angle;
		  D3DXFROMWINEVECTOR2 rotate_offset;
		  bool visible;

		  Image3 image;
		  //Sprite image_sprite;
		
		  std collis_points::vector;

		  GameSprite(float x, float y, int width, int height, int angle=0)
		{
			this->x = x;
			this->y = y;
			this->width = width;
			this->height = height;
			this->angle = angle;
			visible = true;
			collis_points = std::vector(
			, D3DXFROMWINEVECTOR2(0, 0),
			, D3DXFROMWINEVECTOR2(width, 0),
			, D3DXFROMWINEVECTOR2(width, height),
			, D3DXFROMWINEVECTOR2(0, height)
			);
			rotate_offset = D3DXFROMWINEVECTOR2(0, 0);
		}
		 void Render()
		{            
		}
		 void Update()
		{
			if (angle == 0)
			{
				collis_points = [
					, D3DXFROMWINEVECTOR2(x, y),
					, D3DXFROMWINEVECTOR2(x+width, y),
					, D3DXFROMWINEVECTOR2(x+width, y+height),
					, D3DXFROMWINEVECTOR2(x, y+height)			
				];
			}

			/*
			if (angle != 0)
			{
				//have to find the 4 corners of the collision box, and we'll subtract the rotate point so that the points will rotate around that instead of the top left/ origin
				 std points::vector = [
					, D3DXFROMWINEVECTOR2(0-rotate_offset.x, 0-rotate_offset.y),
					, D3DXFROMWINEVECTOR2(width-rotate_offset.x, 0-rotate_offset.y),
					, D3DXFROMWINEVECTOR2(width-rotate_offset.x, height-rotate_offset.y),
					, D3DXFROMWINEVECTOR2(0-rotate_offset.x, height-rotate_offset.y)			
				];
				 std points_trans::vector = std::vector();
				//using the formula: x' = x*cos(angle)-y*sin(angle); y' = x*sin(angle)+y*cos(angle)  from: http://en.wikipedia.org/wiki/Rotation_(mathematics)
				for ( int i = 0; i < points.length;i++ )
				{
					 float x_trans = points[i].x * cosf(angle) - points[i].y * sinf(angle);
					 float y_trans = points[i].x * sinf(angle) + points[i].y * cosf(angle);
					//have to add back in the location of the ship, and add back in the rotational offset from having to take it away earlier
					points_trans.push_back( D3DXFROMWINEVECTOR2(x_trans+x+rotate_offset.x, y_trans+y+rotate_offset.y));
				}
				collis_points = points_trans;
			}
			else
			{
				collis_points = [
					, D3DXFROMWINEVECTOR2(x, y),
					, D3DXFROMWINEVECTOR2(x+width, y),
					, D3DXFROMWINEVECTOR2(x+width, y+height),
					, D3DXFROMWINEVECTOR2(x, y+height),					
				];
			}*/
		}     
		  CheckIfInNonRotatedRect(GameSprite obj2):bool
		{
			//we'll make sure that this object is a rentangle with no angle
			 bool intersecting = false;
			if (obj2.angle == 0)
			{
				//first we check the top left point
				if (x >= obj2.x && x <= obj2.x + obj2.width)
					if (y >= obj2.y && y <= obj2.y + obj2.height)
						return true;
						
				//now we'll check the top right point
				if (x+width >= obj2.x && x+width <= obj2.x + obj2.width)
					if (y >= obj2.y && y <= obj2.y + obj2.height)
						return true;
				
				//now we check the bottom right point
				if (x+width >= obj2.x && x+width <= obj2.x + obj2.width)
					if (y+height >= obj2.y && y+height <= obj2.y + obj2.height)
						return true;
						
				//And check the bottom left point
				if (x >= obj2.x && x <= obj2.x + obj2.width)
					if (y+height >= obj2.y && y+height <= obj2.y + obj2.height)
						return true;						
			}
			
			return intersecting;
		}
		/*  CheckRectIntersect(GameSprite obj2):bool
		{
			 bool answer = false;
			//http://stackoverflow.com/questions/115426/algorithm-to-detect-intersection-of-two-rectangles
			//we'll check if the object is intersecting the current object using the seperating axis theorem
			
			//first we need to get the vectors of the points making up the object- and performing rotations to find where the points are			
			//first step is we get all the edges by using the formula: edge = v(n) - v(n-1)
			 std edges1::vector = std::vector();
			for ( int i = 0; i < collis_points.length; i++)
			{
				if(i!=collis_points.length-1)
					edges1.push_back( D3DXFROMWINEVECTOR2(collis_points[i + 1].x - collis_points[i].x, collis_points[i + 1].y - collis_points[i].y));
				else
					edges1.push_back( D3DXFROMWINEVECTOR2(collis_points[0].x - collis_points[i].x, collis_points[0].y - collis_points[i].y));					
			}
			
			
			 std edges2::vector = std::vector();
			for ( int j = 0; j < obj2.collis_points.length; j++)
			{
				if(j!=obj2.collis_points.length-1)
					edges1.push_back( D3DXFROMWINEVECTOR2(obj2.collis_points[j + 1].x - obj2.collis_points[j].x, obj2.collis_points[j + 1].y - obj2.collis_points[j].y));
				else
					edges1.push_back( D3DXFROMWINEVECTOR2(obj2.collis_points[0].x - obj2.collis_points[j].x, obj2.collis_points[0].y - obj2.collis_points[j].y));					
			}
			
			//now we find the perpendicular of these edges using:
			//rotated.x = -unrotated.y; rotated.y =  unrotated.x
			 std rotated1::vector = std::vector();
			for ( int k; k < edges1.length; k++)
				rotated1.push( -edges1[k].y, edges1[k].x);
			
			 std rotated2::vector = std::vector();
			for ( int m; m < edges2.length; m++)
				rotated2.push( -edges2[m].y, edges2[m].x);
				
			//need to finish- now we need to do dot product to see what sign is on object	


			
			
			return answer;
		}*/
	};
#endif

