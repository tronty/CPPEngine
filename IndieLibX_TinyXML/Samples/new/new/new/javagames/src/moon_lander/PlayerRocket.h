/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * The space rocket with which player will have to land.
 * 
 * @author www.gametutorial.net
 */

 struct PlayerRocket {
    
    /**
     * We use this to generate a random number for starting x coordinate of the rocket.
     */
     Random random;
 
    /**
     * X coordinate of the rocket.
     */
     int x;
    /**
     * Y coordinate of the rocket.
     */
     int y;
    
    /**
     * Is rocket landed?
     */
     bool landed;
    
    /**
     * Has rocket crashed?
     */
     bool crashed;
        
    /**
     * Accelerating speed of the rocket.
     */
     int speedAccelerating;
    /**
     * Stopping/Falling speed of the rocket. Falling speed because, the gravity pulls the rocket down to the moon.
     */
     int speedStopping;
    
    /**
     * Maximum speed that rocket can have without having a crash when landing.
     */
     int topLandingSpeed;
    
    /**
     * How fast and to which direction rocket is moving on x coordinate?
     */
     int speedX;
    /**
     * How fast and to which direction rocket is moving on y coordinate?
     */
     int speedY;
            
    /**
     * Image of the rocket in air.
     */
     TextureID rocketImg;
    /**
     * Image of the rocket when landed.
     */
     TextureID rocketLandedImg;
    /**
     * Image of the rocket when crashed.
     */
     TextureID rocketCrashedImg;
    /**
     * Image of the rocket fire.
     */
     TextureID rocketFireImg;
    
    /**
     * Width of rocket.
     */
     int rocketImgWidth;
    /**
     * Height of rocket.
     */
     int rocketImgHeight;
    
    
     PlayerRocket()
    {
        Initialize();
        LoadContent();
        
        // Now that we have rocketImgWidth we set starting x coordinate.
        x = random.nextInt(Framework.frameWidth - rocketImgWidth);
    }
    
    
     void Initialize()
    {
        random = Random();
        
        ResetPlayer();
        
        speedAccelerating = 2;
        speedStopping = 1;
        
        topLandingSpeed = 5;
    }
    
     void LoadContent()
    {
            rocket =   IRenderer::GetRendererInstance()->addImageLibTexture("/moon_lander/rocket.png");
            rocketImgWidth = rocketImg.getWidth();
            rocketImgHeight = rocketImg.getHeight();
            
            rocketLanded =   IRenderer::GetRendererInstance()->addImageLibTexture("/moon_lander/rocket_landed.png");
            
            rocketCrashed =   IRenderer::GetRendererInstance()->addImageLibTexture("/moon_lander/rocket_crashed.png");
            
            rocketFire =   IRenderer::GetRendererInstance()->addImageLibTexture("/moon_lander/rocket_fire.png");
    }
    
    /**
     * Here we set up the rocket when we starting a new game.
     */
     void ResetPlayer()
    {
        landed = false;
        crashed = false;
        
        x = random.nextInt(Framework.frameWidth - rocketImgWidth);
        y = 10;
        
        speedX = 0;
        speedY = 0;
    }
    
    
    /**
     * Here we move the rocket.
     */
     void Update()
    {
        // Calculating speed for moving up or down.
        if(Canvas.keyboardKeyState(KeyEvent.VK_W))
            speedY -= speedAccelerating;
        else
            speedY += speedStopping;
        
        // Calculating speed for moving or stopping to the left.
        if(Canvas.keyboardKeyState(KeyEvent.VK_A))
            speedX -= speedAccelerating;
        else if(speedX < 0)
            speedX += speedStopping;
        
        // Calculating speed for moving or stopping to the right.
        if(Canvas.keyboardKeyState(KeyEvent.VK_D))
            speedX += speedAccelerating;
        else if(speedX > 0)
            speedX -= speedStopping;
        
        // Moves the rocket.
        x += speedX;
        y += speedY;
    }
    
     void Draw(Graphics2D g2d)
    {
        g2d.setColor(Color.white);
        g2d.drawString("Rocket coordinates: " + x + " : " + y, 5, 15);
        
        // If the rocket is landed.
        if(landed)
        {
            g2d.drawImage(rocketLandedImg, x, y, null);
        }
        // If the rocket is crashed.
        else if(crashed)
        {
            g2d.drawImage(rocketCrashedImg, x, y + rocketImgHeight - rocketCrashedImg.getHeight(), null);
        }
        // If the rocket is still in the space.
        else
        {
            // If player hold down a W key we draw rocket fire.
            if(Canvas.keyboardKeyState(KeyEvent.VK_W))
                g2d.drawImage(rocketFireImg, x + 12, y + 66, null);
            g2d.drawImage(rocketImg, x, y, null);
        }
    }
    
}
#endif

