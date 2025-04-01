#ifndef __Game__
#define __Game__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "Asteroid.h"
#include "Bullet.h"
#include "Explosion.h"
#include "Ship.h"

	 struct Game
	{
		  Image3 bitmap;
		   Image3 Renderer;
		
		  Ship ship
		
		  std::vector<int> keys_down;
		  std::vector<int> keys_up;
		
		 const int LEFT = 37;
		 const int UP = 38;
		 const int RIGHT = 39;
		 const int DOWN = 40;
		 const int SPACE = 32;	
		
		 const int P = 80;
		 const int R = 82;
		
		
		 const int ESC = 27;
		
		   float current_time;
		  std bullets::vector;
		  float firing_delay;
		  float last_fired;
		  float bullets_max_life;
		
		  std asteroids::vector;
		
		  std explosions::vector;
		
		  TextField score_txt;
		  int score;
		
		  TextField level_txt;
		  int level;
		
		  TextField game_over_txt;
		  bool game_over;
		
		  int lives;
		  int starting_lives;
		  float death_delay;
		
		  float level_change_timer;
		  float level_change_delay;
		
		  D3DXFROMWINEVECTOR2 ship_start;
		
		  int state;
		
		 const int MAIN_MENU = 0;
		 const int VIEW_HIGH_SCORES = 1;
		 const int ENTER_HIGH_SCORE = 2;
		 const int PLAYING = 3;
		 const int PAUSED = 4;
		
		
		  Screen main_menu_screen;
		  Screen view_high_score_screen;
		  Screen enter_high_score_screen;
		  Screen paused_screen;
		
		   bool mouse_down;
		   bool mouse_click;
		   D3DXFROMWINEVECTOR2 mouse_pos;		

		  TextureID tex_background;
		  Image3 background;
		  TextureID tex_menu_bg;
		  TextureID tex_btn_start_game1;
		  TextureID tex_btn_start_game2;

		  Game(int stageWidth, int stageHeight)
		{
			//trace("Game created");
			current_time = getTimer();
			Renderer = Image3(stageWidth, stageHeight, false, 0x000000);
			bitmap = Image3(Renderer);

			ship_start = D3DXFROMWINEVECTOR2(STX_Service::GetWindowInstance()->GetWidth() / 2 - 10, STX_Service::GetWindowInstance()->GetHeight() / 2 - 15);
			ship = Ship(ship_start.x, ship_start.y, 20, 30);
			
			keys_down = std::vector();
			keys_up = std::vector();
			bullets = std::vector();
			firing_delay = 200;
			last_fired = 0;
			bullets_max_life = 1000;
			
			asteroids = std::vector();
			explosions = std::vector();
			
			score_txt = TextField();
			score_txt.width = 700;
			 TextFormat format = TextFormat("Courier", 40, 0xFFFFFF, true);
			score_txt.x = 20;
			score_txt.y = 10;
			score_txt.defaultTextFormat = format;
			score = 0;
			score_txt.text = std::string(score);
			
			game_over_txt = TextField();
			game_over_txt.width = STX_Service::GetWindowInstance()->GetWidth();
			 TextFormat format2 = TextFormat("Courier", 40, 0xFFFFFF, true);
			format2.align = "center";
			game_over_txt.x = 0;
			game_over_txt.y = STX_Service::GetWindowInstance()->GetHeight() / 2 - 20;
			game_over_txt.defaultTextFormat = format2;
			game_over_txt.text = "Game Over Man!\nPress 'R' to start a new game";
			game_over = false;
			
			starting_lives = 4;
			lives = starting_lives;
			death_delay = 2000;
			
			level_change_delay = 500;
			level_change_timer = 0;
			
			level_txt = TextField();
			level_txt.width = 700;
			level_txt.defaultTextFormat = format;
			level_txt.x = 20;
			level_txt.y = 65;
			level_txt.text = "Lvl " + level;
			
			level = 1;

            //tex_background=IRenderer::GetRendererInstance()->addImageLibTexture("/new/asteroids/background_space.jpg");
		    tex_menu_bg=IRenderer::GetRendererInstance()->addImageLibTexture("/new/asteroids/chrismweb_menu_bg.jpg");
		    tex_btn_start_game1=IRenderer::GetRendererInstance()->addImageLibTexture("/new/asteroids/btn_start_game1.png");s		
		    tex_btn_start_game2=IRenderer::GetRendererInstance()->addImageLibTexture("/new/asteroids/btn_start_game2.png");
			
			 TextureID temp_menu_bg = tex_menu_bg;
			 TextureID temp_bitmap1 = tex_btn_start_game1;
			 TextureID temp_bitmap2 = tex_btn_start_game2;
			
			main_menu_screen = Screen(0, 0, STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(),false, 0x000000, true, temp_menu_bg.bitmapData);
			
			main_menu_screen.AddImageButton(
			STX_Service::GetWindowInstance()->GetWidth() / 2 - 84, 100, 
			temp_bitmap1.bitmapData, temp_bitmap2.bitmapData,
			StartGame);
			
			 TextFormat format3 = TextFormat("Courier", 20, 0xFFFFFF, true);
			format3.align = "center";
			
			/*
			main_menu_screen.AddTextButton(
			STX_Service::GetWindowInstance()->GetWidth() / 2 - 75, 150, 
			150, 30, 
			"View Scores",	null, 
			0x000000, "Courier", 
			10, true, 
			0x666666, 0x999999,
			ShowScores);
			*/
			
			paused_screen = Screen(0, 0, STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), false);
			paused_screen.AddText(0, 100, STX_Service::GetWindowInstance()->GetWidth(), 40, "Paused: Press 'P' To Resume", format3);			
			
			view_high_score_screen = Screen(0, 0, STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), true, 0x000000);

			view_high_score_screen.AddText(0, 50, STX_Service::GetWindowInstance()->GetWidth(), 20, "High Scores", format3);
			
			 TextFormat format4 = TextFormat("Courier", 16, 0xFFFFFF);
			format4.align = "left";
			
			view_high_score_screen.AddText(
			200, 100, 
			200, STX_Service::GetWindowInstance()->GetHeight() - 100, 
			"1. Chrismweb\n2. Chrismweb\n3. Chrismweb\n4. Chrismweb\n5. Chrismweb\n6. Chrismweb\n7. Chrismweb\n8. Chrismweb\n9. Chrismweb\n10. Chrismweb", format4);
			
			view_high_score_screen.AddText(
			400, 100, 
			200, STX_Service::GetWindowInstance()->GetHeight() - 100, 
			"100\n100\n100\n100\n100\n100\n100\n100\n100\n100 ", format4);
			
			view_high_score_screen.AddTextButton(
			STX_Service::GetWindowInstance()->GetWidth() / 2 - 150, STX_Service::GetWindowInstance()->GetHeight()-100, 
			300, 30, 
			"Return to Main Menu", null,
			0x000000, "Courier",
			10, true, 
			0x666666, 0x999999,
			ShowMainMenu);
			
			enter_high_score_screen = Screen(0, 0, STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), true, 0x000000);
			
			enter_high_score_screen.AddText(0, 50, STX_Service::GetWindowInstance()->GetWidth(), 100, "You made the high score list! \nEnter Your name:", format3);
			
			//button to submit score
			enter_high_score_screen.AddTextButton(
			STX_Service::GetWindowInstance()->GetWidth() / 2 - 100, STX_Service::GetWindowInstance()->GetHeight()-300, 
			200, 30, 
			"Submit Score", null,
			0x000000, "Courier",
			10, true, 
			0x666666, 0x999999);
			
			//button to return to main menu
			enter_high_score_screen.AddTextButton(
			STX_Service::GetWindowInstance()->GetWidth() / 2 - 150, STX_Service::GetWindowInstance()->GetHeight()-100, 
			300, 30, 
			"Return to Main Menu", null,
			0x000000, "Courier",
			10, true, 
			0x666666, 0x999999,
			ShowMainMenu);
			
			mouse_down = false;
			mouse_click = false;
			mouse_pos = D3DXFROMWINEVECTOR2(0, 0);
			
			state = MAIN_MENU;
			
			background = tex_background();
		}
		  ShowMainMenu():void
		{
			state = MAIN_MENU;
		}
		  ShowScores():void
		{
			state = VIEW_HIGH_SCORES;
		}
		  StartGame():void
		{
			state = PLAYING;
			LoadLevel(1);
		}
		  LoadLevel(int num):void
		{
			if (num == 1)
			{
				score = 0;
				score_txt.text = std::string(score);
				lives = starting_lives;				
			}
			
			game_over = false;
			ResetShip();
			asteroids = std::vector();
			bullets = std::vector();
			explosions = std::vector();
			level = num;
			level_txt.text = "Lvl " + level;
			//every level the number of asteroids increase until 12 on screen at once, after that the levels just keep resetting
			
			//setup four first asteroids
			asteroids.push_back( Asteroid(STX_Service::GetWindowInstance()->GetWidth() / 5, 2 * STX_Service::GetWindowInstance()->GetHeight() / 3, 0, 0, 1));
			asteroids.push_back( Asteroid(STX_Service::GetWindowInstance()->GetWidth() / 5, STX_Service::GetWindowInstance()->GetHeight() / 4, 0, 1, 0));
			
			asteroids.push_back( Asteroid(2*STX_Service::GetWindowInstance()->GetWidth() / 3, 1 * STX_Service::GetWindowInstance()->GetHeight() / 3, 0, 2, 1));
			asteroids.push_back( Asteroid(2*STX_Service::GetWindowInstance()->GetWidth() / 3, STX_Service::GetWindowInstance()->GetHeight(), 0, 3, 0));
			
			if (num > 1)//5 
				asteroids.push_back( Asteroid(STX_Service::GetWindowInstance()->GetWidth() / 5, 1 * STX_Service::GetWindowInstance()->GetHeight() / 3, 0, 0, 1));
			if (num > 2)//6
				asteroids.push_back( Asteroid(STX_Service::GetWindowInstance()->GetWidth() ,  STX_Service::GetWindowInstance()->GetHeight() / 5, 0, 0, 1));
			if (num > 3)//7
				asteroids.push_back( Asteroid(STX_Service::GetWindowInstance()->GetWidth() / 2, STX_Service::GetWindowInstance()->GetHeight(), 0, 1, 1));
			if (num > 4)//8
				asteroids.push_back( Asteroid(2*STX_Service::GetWindowInstance()->GetWidth() / 3, 2 * STX_Service::GetWindowInstance()->GetHeight() / 3, 0, 1, 1));
			if (num > 5)//9
				asteroids.push_back( Asteroid(STX_Service::GetWindowInstance()->GetWidth() / 7, 3*STX_Service::GetWindowInstance()->GetHeight() / 4, 0, 2, 1));
			if (num > 6)//10
				asteroids.push_back( Asteroid(STX_Service::GetWindowInstance()->GetWidth() / 2, 2 * STX_Service::GetWindowInstance()->GetHeight() / 8, 0, 2, 1));
			if (num > 7)//11
				asteroids.push_back( Asteroid(4*STX_Service::GetWindowInstance()->GetWidth() / 5, STX_Service::GetWindowInstance()->GetHeight(), 0, 3, 1));
			if (num > 8)//12
				asteroids.push_back( Asteroid(2*STX_Service::GetWindowInstance()->GetWidth() / 3, 2 * STX_Service::GetWindowInstance()->GetHeight() / 3, 0, 3, 1));
		}
		  Render():void
		{
			
			IRenderer::GetRendererInstance()->lock();
			IRenderer::GetRendererInstance()->fillRect(Rectangle(0, 0, STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight()), 0x000000);
			
			//IRenderer::GetRendererInstance()->draw(background, D3DXFROMWINEMATRIX(1, 0, 0, 1, 0 , 0 ));
			IRenderer::GetRendererInstance()->copyPixels(background.bitmapData, Rectangle(0, 0, background.width, background.height), D3DXFROMWINEVECTOR2(0, 0));

			if (state == PLAYING || state == PAUSED)
			{

				if(state!=PAUSED)
					ship.Render();
					
				for ( int i = 0; i < bullets.length; i++)
					bullets[i].Render();
					
				for ( int j = 0; j < asteroids.length; j++)
					asteroids[j].Render();

				for ( int k = 0; k < explosions.length; k++)
					explosions[k].Render();

				IRenderer::GetRendererInstance()->draw(score_txt, D3DXFROMWINEMATRIX(1, 0, 0, 1, score_txt.x , score_txt.y ));
				IRenderer::GetRendererInstance()->draw(level_txt, D3DXFROMWINEMATRIX(1, 0, 0, 1, level_txt.x , level_txt.y ));
				
				for ( int p = 0; p < lives; p++)
					IRenderer::GetRendererInstance()->draw(ship.image_sprite, D3DXFROMWINEMATRIX(1,0,0,1,score_txt.x+(ship.width+2)*p, 35));
				if (game_over)
					IRenderer::GetRendererInstance()->draw(game_over_txt, D3DXFROMWINEMATRIX(1, 0, 0, 1, game_over_txt.x , game_over_txt.y ));

				if (state == PAUSED)
					paused_screen.Render(Renderer);
			}
			else if (state == MAIN_MENU)
				main_menu_screen.Render(Renderer);
			else if (state == ENTER_HIGH_SCORE)
				enter_high_score_screen.Render(Renderer);	
			else if (state == VIEW_HIGH_SCORES)
				view_high_score_screen.Render(Renderer);	

			IRenderer::GetRendererInstance()->unlock();
		}

		  void Update()
		{
			current_time = getTimer();
			
			if (state == PLAYING)
			{
				if (CheckKeyUp(ESC))
					state = MAIN_MENU;
				if (CheckKeyUp(P))
					state = PAUSED;
				if (CheckKeyUp(R))
				{
					LoadLevel(1);
					keys_up = std::vector();
					return;
				}
			
				if (ship.visible)
				{			
					if (CheckKeyDown(LEFT))			
						ship.RotateLeft();
					
					if (CheckKeyDown(RIGHT))
						ship.RotateRight();
						
					if (CheckKeyDown(UP))
						ship.Thrust(1);
					if (CheckKeyDown(DOWN))
						ship.Thrust( -1);				
						
					if (CheckKeyDown(SPACE) && current_time-last_fired > firing_delay)
					{
						 int x = 0 * cosf(ship.angle) + ship.rotate_offset.y * sinf(ship.angle)+ship.x+ship.rotate_offset.x;
						 int y = 0 * sinf(ship.angle) - ship.rotate_offset.y * cosf(ship.angle)+ship.y+ship.rotate_offset.y;
						
						bullets.push_back( Bullet(x, y, current_time, ship.angle));
						last_fired = current_time;
					}					
					ship.Update();
				}
				else if (CheckKeyDown(R))
				{					
					LoadLevel(1);
				}
				

				 std bullets_to_delete::vector = std::vector();
				for ( int i = 0; i < bullets.length; i++)
				{
					bullets[i].Update();
					if (current_time-bullets[i].life > bullets_max_life)
					{
						bullets_to_delete.push(i);
						continue;
					}
						
					 int asteroid_hit = -1;
					for ( int i2 = 0; i2 < asteroids.length;i2++)
						if (bullets[i].CheckIfInNonRotatedRect(asteroids[i2]))
						{
							asteroid_hit = i2;
							break;
						}
					if (asteroid_hit != -1)
					{
						DestroyAsteroid(asteroid_hit);
						bullets_to_delete.push(i);
					}
				}
				for ( int j = 0; j < bullets_to_delete.length;j++ )
				{
					bullets.splice(bullets_to_delete[j], 1);
				}		
				 int asteroid_ship_hit = -1;
				for ( int k = 0; k < asteroids.length; k++ )
				{
					asteroids[k].Update();
					if (ship.CheckIfInNonRotatedRect(asteroids[k]))
					{
						asteroid_ship_hit = k;
						break;
					}				
				}
				if (asteroid_ship_hit != -1)
				{
					lives--;
					ship.visible = false;
					explosions.push_back( Explosion(ship.x, ship.y, 1500,2));
					explosions.push_back( Explosion(ship.x, ship.y, 500));
					DestroyAsteroid(asteroid_ship_hit);
					if (lives > 0)
						ship.ship_death_time = current_time;
					else
					{
						KONGREGATE.SubmitScore(score);
						game_over = true;
					}
				}
					
				 std explosions_to_delete::vector = std::vector();
				for ( int m = 0; m < explosions.length;m++ )
					if (current_time-explosions[m].life > explosions[m].max_life)
						explosions_to_delete.push(m);
				for ( int n = 0; n < explosions_to_delete.length; n++)
					explosions.splice(explosions_to_delete[n], 1);
					
				if (ship.ship_death_time != 0)
					if (current_time-ship.ship_death_time > death_delay)
					{
						ResetShip();					
					}
				if (asteroids.length == 0 && !game_over&&level_change_timer==0)
					level_change_timer = current_time;
					
				if(level_change_timer!=0)
					if (current_time-level_change_timer > level_change_delay)
					{
						
						level++;
						LoadLevel(level);
						level_change_timer = 0;
						//trace("loading level " + level);
					}
			}//end of "playing state
			else if (state == MAIN_MENU)			
				main_menu_screen.Update();
			
			else if (state == PAUSED)
			{
				if (CheckKeyUp(P))
					state = PLAYING;
			}
			else if (state == ENTER_HIGH_SCORE)
				enter_high_score_screen.Update();
			else if (state == VIEW_HIGH_SCORES)
				view_high_score_screen.Update()
			
			mouse_click = false;
			//clear out the "keys_up" array for next update			
			keys_up = std::vector();
		}
		  ResetShip():void
		{
			ship.visible = true;
			ship.angle = 0;
			ship.x = ship_start.x;
			ship.y = ship_start.y;
			ship.speed.x = 0;
			ship.speed.y = 0;
			ship.ship_death_time = 0;
		}
		  DestroyAsteroid(int asteroid_hit):void
		{
			score += Asteroid.Scores[asteroids[asteroid_hit].size];
			score_txt.text = std::string(score);
			////trace("score is now =" + score);
			explosions.push_back( Explosion(
			asteroids[asteroid_hit].x + asteroids[asteroid_hit].width / 2,
			asteroids[asteroid_hit].y + asteroids[asteroid_hit].height / 2, 1000, asteroids[asteroid_hit].width/4));	
			
			//now delete the old asteroid, and add 2 new ones in it's place if there are any more sizes left
			 Asteroid old_asteroid = asteroids[asteroid_hit];
			//if there are more sizes to chose from
			if (old_asteroid.size != Asteroid.Sizes.length - 1)
			{
				
				 int rand_dir = Math.floor(Math.random() * (1 + Asteroid.Directions.length - 1 ));
				
				 int rand_dir2 = rand_dir - 2;
				if (rand_dir - 2 < 0)
					rand_dir2 = rand_dir + 2;
				
				 int rand_type = Math.floor(Math.random() * (1 + Asteroid.Types.length - 1 ));
				 int rand_type2 = Math.floor(Math.random() * (1 + Asteroid.Types.length - 1 ));
				
				//add 2 new asteroids at half the size
				asteroids.push_back( Asteroid(
				old_asteroid.x, 
				old_asteroid.y, 
				old_asteroid.size + 1, 
				rand_dir, 
				rand_type));
				
				asteroids.push_back( Asteroid(
				old_asteroid.x, 
				old_asteroid.y, 
				old_asteroid.size + 1, 
				rand_dir2, 
				rand_type2));
			}
			asteroids.splice(asteroid_hit, 1);
		}
		  KeyUp(KeyboardEvent e):void
		{
			//trace(e.keyCode);
			//position of key in the array
			 int key_pos = -1;
			for ( int i = 0; i < keys_down.length; i++)
				if (e.keyCode == keys_down[i])
				{
					//the key is found/was pressed before, so store the position
					key_pos = i;
					break;
				}
			//remove the keycode from keys_down if found 
			if(key_pos!=-1)
				keys_down.splice(key_pos, 1);		
				
			keys_up.push(e.keyCode);
		}
		
		  KeyDown(KeyboardEvent e):void
		{
			//check to see if the key that is being pressed is already in the array of pressed keys
			 bool key_down = false;
			for ( int i = 0; i < keys_down.length; i++)
				if (keys_down[i] == e.keyCode)
					key_down = true;
			
			//add the key to the array of pressed keys if it wasn't already in there
			if (!key_down)
				keys_down.push(e.keyCode);			
		}
		
		  CheckKeyDown(int keycode):bool
		{
			 bool answer = false;
			for ( int i = 0; i < keys_down.length; i++)
				if (keys_down[i] == keycode)
				{
					answer = true;
					break;
				}
			return answer;
		}
		  CheckKeyUp(int keycode):bool
		{
			 bool answer = false;
			for ( int i = 0; i < keys_up.length; i++)
				if (keys_up[i] == keycode)
				{
					answer = true;
					break;
				}
			return answer;
		}
		  MoveMouse(MouseEvent e):void 
		{
			mouse_pos.x = e.stageX;
			mouse_pos.y = e.stageY;
		}
		  MouseDown(MouseEvent e):void
		{
			mouse_down = true;
		}
		  MouseUp(MouseEvent e):void
		{
			mouse_down = false;
			mouse_click = true;
		}
	};
#endif

