#ifndef __Game__
#define __Game__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
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
     * The space rocket with which player will have to land.
     */
     PlayerRocket playerRocket;
    
    /**
     * Landing area on which rocket will have to land.
     */
     LandingArea landingArea;
    
    /**
     * Game background image.
     */
     TextureID backgroundImg;
    
    /**
     * Red border of the frame. It is used when player crash the rocket.
     */
     TextureID redBorderImg;
    

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
        };
    }
    
    
   /**
     * Set variables and objects for the game.
     */
     void Initialize()
    {
        playerRocket = PlayerRocket();
        landingArea  = LandingArea();
    }
    
    /**
     * Load game files - images, sounds, ...
     */
     void LoadContent()
    {
            background =   IRenderer::GetRendererInstance()->addImageLibTexture("/moon_lander/background.jpg");
            
            redBorder =   IRenderer::GetRendererInstance()->addImageLibTexture("/moon_lander/red_border.png");
    }
    
    
    /**
     * Restart game - reset some variables.
     */
     void RestartGame()
    {
        playerRocket.ResetPlayer();
    }
    
    
    /**
     * Update game logic.
     * 
     * @param gameTime gameTime of the game.
     * @param mousePosition current mouse position.
     */
     void UpdateGame(long gameTime, Point mousePosition)
    {
        // Move the rocket
        playerRocket.Update();
        
        // Checks where the player rocket is. Is it still in the space or is it landed or crashed?
        // First we check bottom y coordinate of the rocket if is it near the landing area.
        if(playerRocket.y + playerRocket.rocketImgHeight - 10 > landingArea.y)
        {
            // Here we check if the rocket is over landing area.
            if((playerRocket.x > landingArea.x) && (playerRocket.x < landingArea.x + landingArea.landingAreaImgWidth - playerRocket.rocketImgWidth))
            {
                // Here we check if the rocket speed isn't too high.
                if(playerRocket.speedY <= playerRocket.topLandingSpeed)
                    playerRocket.landed = true;
                else
                    playerRocket.crashed = true;
            }
            else
                playerRocket.crashed = true;
                
            Framework.gameState = Framework.GameState.GAMEOVER;
        }
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
        
        landingArea.Draw(g2d);
        
        playerRocket.Draw(g2d);
    }
    
    
    /**
     * Draw the game over screen.
     * 
     * @param g2d Graphics2D
     * @param mousePosition Current mouse position.
     * @param gameTime Game time in nanoseconds.
     */
     void DrawGameOver(Graphics2D g2d, Point mousePosition, long gameTime)
    {
        Draw(g2d, mousePosition);
        
        g2d.drawString("Press space or enter to restart.", Framework.frameWidth / 2 - 100, Framework.frameHeight / 3 + 70);
        
        if(playerRocket.landed)
        {
            g2d.drawString("You have successfully landed!", Framework.frameWidth / 2 - 100, Framework.frameHeight / 3);
            g2d.drawString("You have landed in " + gameTime / Framework.secInNanosec + " seconds.", Framework.frameWidth / 2 - 100, Framework.frameHeight / 3 + 20);
        }
        else
        {
            g2d.setColor(Color.red);
            g2d.drawString("You have crashed the rocket!", Framework.frameWidth / 2 - 95, Framework.frameHeight / 3);
            g2d.drawImage(redBorderImg, 0, 0, Framework.frameWidth, Framework.frameHeight, null);
        }
    }
}
#endif

