#ifndef __Game__
#define __Game__
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Actual game.
 * 
 * @author www.gametutorial.net
 */

 struct Game {
    
    /**
     * We use this to generate a random number.
     */
     Random random;
    
    /**
     * Font that we will use to write statistic to the screen.
     */
     Font font;
    
    /**
     * Array list of the ducks.
     */
     std::vector<Duck> ducks;
    
    /**
     * How many ducks leave the screen alive?
     */
     int runawayDucks;
    
   /**
     * How many ducks the player killed?
     */
     int killedDucks;
    
    /**
     * For each killed duck, the player gets points.
     */
     int score;
    
   /**
     * How many times a player is shot?
     */
     int shoots;
    
    /**
     * Last time of the shoot.
     */
     long lastTimeShoot;    
    /**
     * The time which must elapse between shots.
     */
     long timeBetweenShots;

    /**
     * Game background image.
     */
     TextureID backgroundImg;
    
    /**
     * Bottom grass.
     */
     TextureID grassImg;
    
    /**
     * Duck image.
     */
     TextureID duckImg;
    
    /**
     * Shotgun sight image.
     */
     TextureID sightImg;
    
    /**
     * Middle width of the sight image.
     */
     int sightImgMiddleWidth;
    /**
     * Middle height of the sight image.
     */
     int sightImgMiddleHeight;
    

     Game()
    {
        Framework.gameState = Framework.GameState.GAME_CONTENT_LOADING;
            
             void run(){
                // Sets variables and objects for the game.
                Initialize();
                // Load game files (images, sounds, ...)
                LoadContent();
                
                Framework.gameState = Framework.GameState.PLAYING;
            }
    }
    
    
   /**
     * Set variables and objects for the game.
     */
     void Initialize()
    {
        random = Random();
        
        ducks.clear();
        
        runawayDucks = 0;
        killedDucks = 0;
        score = 0;
        shoots = 0;
        
        lastTimeShoot = 0;
        timeBetweenShots = Framework.secInNanosec / 3;
    }
    
    /**
     * Load game files - images, sounds, ...
     */
     void LoadContent()
    {
            background =   IRenderer::GetRendererInstance()->addImageLibTexture("/shoot_the_duck/background.jpg");
            
            grass =   IRenderer::GetRendererInstance()->addImageLibTexture("/shoot_the_duck/grass.png");
            
            duck =   IRenderer::GetRendererInstance()->addImageLibTexture("/shoot_the_duck/duck.png");
            
            sight =   IRenderer::GetRendererInstance()->addImageLibTexture("/shoot_the_duck/sight.png");
            sightImgMiddleWidth = sightImg.getWidth() / 2;
            sightImgMiddleHeight = sightImg.getHeight() / 2;
    }
    
    
    /**
     * Restart game - reset some variables.
     */
     void RestartGame()
    {
        // Removes all of the ducks from this list.
        ducks.clear();
        
        // We set last duckt time to zero.
        Duck.lastDuckTime = 0;
        
        runawayDucks = 0;
        killedDucks = 0;
        score = 0;
        shoots = 0;
        
        lastTimeShoot = 0;
    }
    
    
    /**
     * Update game logic.
     * 
     * @param gameTime gameTime of the game.
     * @param mousePosition current mouse position.
     */
     void UpdateGame(long gameTime, Point mousePosition)
    {
        // Creates a new duck, if it's the time, and add it to the array list.
        if(System.nanoTime() - Duck.lastDuckTime >= Duck.timeBetweenDucks)
        {
            // Here we create new duck and add it to the array list.
            ducks.add(new Duck(Duck.duckLines[Duck.nextDuckLines][0] + random.nextInt(200), Duck.duckLines[Duck.nextDuckLines][1], Duck.duckLines[Duck.nextDuckLines][2], Duck.duckLines[Duck.nextDuckLines][3], duckImg));
            
            // Here we increase nextDuckLines so that next duck will be created in next line.
            Duck.nextDuckLines++;
            if(Duck.nextDuckLines >= Duck.duckLines.length)
                Duck.nextDuckLines = 0;
            
            Duck.lastDuckTime = System.nanoTime();
        }
        
        // Update all of the ducks.
        for(int i = 0; i < ducks.size(); i++)
        {
            // Move the duck.
            ducks.get(i).Update();
            
            // Checks if the duck leaves the screen and remove it if it does.
            if(ducks.get(i).x < 0 - duckImg.getWidth())
            {
                ducks.remove(i);
                runawayDucks++;
            }
        }
        
        // Does player shoots?
        if(Canvas.mouseButtonState(MouseEvent.BUTTON1))
        {
            // Checks if it can shoot again.
            if(System.nanoTime() - lastTimeShoot >= timeBetweenShots)
            {
                shoots++;
                
                // We go over all the ducks and we look if any of them was shoot.
                for(int i = 0; i < ducks.size(); i++)
                {
                    // We check, if the mouse was over ducks head or body, when player has shot.
                    if( Rectangle(ducks.get(i).x + 18, ducks.get(i).y     , 27, 30).contains(mousePosition) ||
                        Rectangle(ducks.get(i).x + 30, ducks.get(i).y + 30, 88, 25).contains(mousePosition))
                    {
                        killedDucks++;
                        score += ducks.get(i).score;
                        
                        // Remove the duck from the array list.
                        ducks.remove(i);
                        
                        // We found the duck that player shoot so we can leave the for loop.
                        break;
                    }
                }
                
                lastTimeShoot = System.nanoTime();
            }
        }
        
        // When 200 ducks runaway, the game ends.
        if(runawayDucks >= 200)
            Framework.gameState = Framework.GameState.GAMEOVER;
    }
    
    /**
     * Draw the game to the screen.
     * 
     * @param g2d Graphics2D
     * @param mousePosition current mouse position.
     */
     void Draw(Graphics2D g2d, Point mousePosition)
    {
        g2d.drawImage(backgroundImg, 0, 0, Framework.frameWidth, Framework.frameHeight, null);
        
        // Here we draw all the ducks.
        for(int i = 0; i < ducks.size(); i++)
        {
            ducks.get(i).Draw(g2d);
        }
        
        g2d.drawImage(grassImg, 0, Framework.frameHeight - grassImg.getHeight(), Framework.frameWidth, grassImg.getHeight(), null);
        
        g2d.drawImage(sightImg, mousePosition.x - sightImgMiddleWidth, mousePosition.y - sightImgMiddleHeight, null);
        
        g2d.setFont(font);
        g2d.setColor(Color.darkGray);
        
        g2d.drawString("RUNAWAY: " + runawayDucks, 10, 21);
        g2d.drawString("KILLS: " + killedDucks, 160, 21);
        g2d.drawString("SHOOTS: " + shoots, 299, 21);
        g2d.drawString("SCORE: " + score, 440, 21);
    }
    
    
    /**
     * Draw the game over screen.
     * 
     * @param g2d Graphics2D
     * @param mousePosition Current mouse position.
     */
     void DrawGameOver(Graphics2D g2d, Point mousePosition)
    {
        Draw(g2d, mousePosition);
        
        // The first text is used for shade.
        g2d.setColor(Color.black);
        g2d.drawString("Game Over", Framework.frameWidth / 2 - 39, (int)(Framework.frameHeight * 0.65) + 1);
        g2d.drawString("Press space or enter to restart.", Framework.frameWidth / 2 - 149, (int)(Framework.frameHeight * 0.70) + 1);
        g2d.setColor(Color.red);
        g2d.drawString("Game Over", Framework.frameWidth / 2 - 40, (int)(Framework.frameHeight * 0.65));
        g2d.drawString("Press space or enter to restart.", Framework.frameWidth / 2 - 150, (int)(Framework.frameHeight * 0.70));
    }
}
#endif

