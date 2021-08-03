#ifndef __Duck__
#define __Duck__
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * The duck struct.
 * 
 * @author www.gametutorial.net
 */

 struct Duck {
    
    /**
     * How much time must pass in order to create a new duck?
     */
      long timeBetweenDucks = Framework.secInNanosec / 2;
    /**
     * Last time when the duck was created.
     */
      long lastDuckTime = 0;
    
    /**
     * Duck lines.
     * Where is starting location for the duck?
     * Speed of the duck?
     * How many points is a duck worth?
     */
      int[][] duckLines = {
                                       {Framework.frameWidth, (int)(Framework.frameHeight * 0.60), -2, 20},
                                       {Framework.frameWidth, (int)(Framework.frameHeight * 0.65), -3, 30},
                                       {Framework.frameWidth, (int)(Framework.frameHeight * 0.70), -4, 40},
                                       {Framework.frameWidth, (int)(Framework.frameHeight * 0.78), -5, 50}
                                      };
    /**
     * Indicate which is next duck line.
     */
      int nextDuckLines = 0;
    
    
    /**
     * X coordinate of the duck.
     */
     int x;
    /**
     * Y coordinate of the duck.
     */
     int y;
    
    /**
     * How fast the duck should move? And to which direction?
     */
     int speed;
    
    /**
     * How many points this duck is worth?
     */
     int score;
    
    /**
     * Duck image.
     */
     Image3 duckImg;
    
    
    /**
     * Creates new duck.
     * 
     * @param x Starting x coordinate.
     * @param y Starting y coordinate.
     * @param speed The speed of this duck.
     * @param score How many points this duck is worth?
     * @param duckImg Image of the duck.
     */
     Duck(int x, int y, int speed, int score, Image3 duckImg)
    {
        this->x = x;
        this->y = y;
        
        this->speed = speed;
        
        this->score = score;
        
        this->duckImg = duckImg;        
    }
    
    
    /**
     * Move the duck.
     */
     void Update()
    {
        x += speed;
    }
    
    /**
     * Draw the duck to the screen.
     * @param g2d Graphics2D
     */
     void Draw(Graphics2D g2d)
    {
        g2d.drawImage(duckImg, x, y, null);
    }
}
#endif

