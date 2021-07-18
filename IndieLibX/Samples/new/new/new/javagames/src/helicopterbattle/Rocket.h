#ifndef __Rocket__
#define __Rocket__
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Rocket.
 * Use: create object of this struct then initialize it with Initialize method.
 * 
 * @author www.gametutorial.net
 */

 struct Rocket {
    
    // Time that must pass before another rocket can be fired.
     const  long timeBetweenNewRockets = Framework.secInNanosec / 4;
      long timeOfLastCreatedRocket = 0;
    
    // Damage that is made to an enemy helicopter when it is hit with a rocket.
      int damagePower = 100;
    
    // Rocket position
     int xCoordinate;
     int yCoordinate;
    
    // Moving speed and also direction. Rocket goes always straight, so we move it only on x coordinate.
     double movingXspeed;
    
    // Life time of current piece of rocket smoke.
     long currentSmokeLifeTime;

    // Image of rocket. Image is loaded and set in Game struct in LoadContent() method.
      Image3 rocketImg;
    

    /**
     * Set variables and objects for this struct.
     */
     void Initialize(int xCoordinate, int yCoordinate)
    {
        this->xCoordinate = xCoordinate;
        this->yCoordinate = yCoordinate;
        
        this->movingXspeed = 23;
        
        this->currentSmokeLifeTime = Framework.secInNanosec / 2;
    }
    
    
    /**
     * Checks if the rocket is left the screen.
     * 
     * @return true if the rocket is left the screen, false otherwise.
     */
     bool isItLeftScreen()
    {
        if(xCoordinate > 0 && xCoordinate < Framework.frameWidth) //Rocket moves only on x coordinate so we don't need to check y coordinate.
            return false;
        else
            return true;
    }
    
    
    /**
     * Moves the rocket.
     */
     void Update()
    {
        xCoordinate += movingXspeed;
    }
    
    
    /**
     * Draws the rocket to the screen.
     * 
     * @param g2d Graphics2D
     */
     void Draw(Graphics2D g2d)
    {
        g2d.drawImage(rocketImg, xCoordinate, yCoordinate, null);
    }
}
#endif

