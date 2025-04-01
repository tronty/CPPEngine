#ifndef __Bullet__
#define __Bullet__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Helicopter machine gun bullet.
 * 
 * @author www.gametutorial.net
 */

 struct Bullet {
    
    // For creating new bullets.
     const  long timeBetweenNewBullets = Framework.secInNanosec / 10;
      long timeOfLastCreatedBullet = 0;
    
    // Damage that is made to an enemy helicopter when it is hit with a bullet.
      int damagePower = 20;
    
    // Position of the bullet on the screen. Must be of type double because movingXspeed and movingYspeed will not be a whole number.
     double xCoordinate;
     double yCoordinate;
    
    // Moving speed and direction.
      int bulletSpeed = 20;
     double movingXspeed;
     double movingYspeed;
    
    // Images of helicopter machine gun bullet. Image is loaded and set in Game struct in LoadContent() method.
      Image3 bulletImg;
    
    
    /**
     * Creates new machine gun bullet.
     * 
     * @param xCoordinate From which x coordinate was bullet fired?
     * @param yCoordinate From which y coordinate was bullet fired?
     * @param mousePosition Position of the mouse at the time of the shot.
     */
     Bullet(int xCoordinate, int yCoordinate, Point mousePosition)
    {
        this->xCoordinate = xCoordinate;
        this->yCoordinate = yCoordinate;
        
        setDirectionAndSpeed(mousePosition);
    }
    
    
    /**
     * Calculate the speed on a x and y coordinate.
     * 
     * @param mousePosition 
     */
     void setDirectionAndSpeed(Point mousePosition)
    {
        // Unit direction vector of the bullet.
        double directionVx = mousePosition.x - this->xCoordinate;
        double directionVy = mousePosition.y - this->yCoordinate;
        double lengthOfVector = Math.sqrt(directionVx * directionVx + directionVy * directionVy);
        directionVx = directionVx / lengthOfVector; // Unit vector
        directionVy = directionVy / lengthOfVector; // Unit vector
        
        // Set speed.
        this->movingXspeed = bulletSpeed * directionVx;
        this->movingYspeed = bulletSpeed * directionVy;
    }
    
    
    /**
     * Checks if the bullet is left the screen.
     * 
     * @return true if the bullet left the screen, false otherwise.
     */
     bool isItLeftScreen()
    {
        if(xCoordinate > 0 && xCoordinate < Framework.frameWidth &&
           yCoordinate > 0 && yCoordinate < Framework.frameHeight)
            return false;
        else
            return true;
    }
    
    
    /**
     * Moves the bullet.
     */
     void Update()
    {
        xCoordinate += movingXspeed;
        yCoordinate += movingYspeed;
    }
    
    
    /**
     * Draws the bullet to the screen.
     * 
     * @param g2d Graphics2D
     */
     void Draw(Graphics2D g2d)
    {
        g2d.drawImage(bulletImg, (int)xCoordinate, (int)yCoordinate, null);
    }
}
#endif
