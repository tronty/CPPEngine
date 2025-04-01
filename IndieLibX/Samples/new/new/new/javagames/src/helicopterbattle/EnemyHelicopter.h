#ifndef __EnemyHelicopter__
#define __EnemyHelicopter__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Enemy helicopter.
 * 
 * @author www.gametutorial.net
 */

 struct EnemyHelicopter {
    
    // For creating new enemies.
      const long timeBetweenNewEnemiesInit = Framework.secInNanosec * 3;
      long timeBetweenNewEnemies = timeBetweenNewEnemiesInit;
      long timeOfLastCreatedEnemy = 0;
    
    // Health of the helicopter.
     int health;
    
    // Position of the helicopter on the screen.
     int xCoordinate;
     int yCoordinate;
    
    // Moving speed and direction.
      const double movingXspeedInit = -4;
      double movingXspeed = movingXspeedInit;
    
    // Images of enemy helicopter. Images are loaded and set in Game struct in LoadContent() method.
      Image3 helicopterBodyImg;
      Image3 helicopterFrontPropellerAnimImg;
      Image3 helicopterRearPropellerAnimImg;
    
    // Animation of the helicopter propeller.
     Animation helicopterFrontPropellerAnim;
     Animation helicopterRearPropellerAnim;
    // Offset for the propeler. We add offset to the position of the position of helicopter.
      int offsetXFrontPropeller = 4;
      int offsetYFrontPropeller = -7;
      int offsetXRearPropeller = 205;
      int offsetYRearPropeller = 6;


    /**
     * Initialize enemy helicopter.
     * 
     * @param xCoordinate Starting x coordinate of helicopter.
     * @param yCoordinate Starting y coordinate of helicopter.
     * @param helicopterBodyImg Image of helicopter body.
     * @param helicopterFrontPropellerAnimImg Image of front helicopter propeller.
     * @param helicopterRearPropellerAnimImg Image of rear helicopter propeller.
     */
     void Initialize(int xCoordinate, int yCoordinate)
    {
        health = 100;
        
        // Sets enemy position.
        this->xCoordinate = xCoordinate;
        this->yCoordinate = yCoordinate;
        
        // Initialize animation object.
        helicopterFrontPropellerAnim = Animation(helicopterFrontPropellerAnimImg, 158, 16, 3, 20, true, xCoordinate + offsetXFrontPropeller, yCoordinate + offsetYFrontPropeller, 0);
        helicopterRearPropellerAnim = Animation(helicopterRearPropellerAnimImg, 47, 47, 10, 10, true, xCoordinate + offsetXRearPropeller, yCoordinate + offsetYRearPropeller, 0);
       
        // Moving speed and direction of enemy.
        EnemyHelicopter.movingXspeed = -4;
    }
    
    /**
     * It sets speed and time between enemies to the initial properties.
     */
      void restartEnemy(){
        EnemyHelicopter.timeBetweenNewEnemies = timeBetweenNewEnemiesInit;
        EnemyHelicopter.timeOfLastCreatedEnemy = 0;
        EnemyHelicopter.movingXspeed = movingXspeedInit;
    }
    
    
    /**
     * It increase enemy speed and decrease time between new enemies.
     */
      void speedUp(){
        if(EnemyHelicopter.timeBetweenNewEnemies > Framework.secInNanosec)
            EnemyHelicopter.timeBetweenNewEnemies -= Framework.secInNanosec / 100;
        
        EnemyHelicopter.movingXspeed -= 0.25;
    }
    
    
    /**
     * Checks if the enemy is left the screen.
     * 
     * @return true if the enemy is left the screen, false otherwise.
     */
     bool isLeftScreen()
    {
        if(xCoordinate < 0 - helicopterBodyImg.getWidth()) // When the entire helicopter is out of the screen.
            return true;
        else
            return false;
    }
    
        
    /**
     * Updates position of helicopter, animations.
     */
     void Update()
    {
        // Move enemy on x coordinate.
        xCoordinate += movingXspeed;
        
        // Moves helicoper propeler animations with helicopter.
        helicopterFrontPropellerAnim.changeCoordinates(xCoordinate + offsetXFrontPropeller, yCoordinate + offsetYFrontPropeller);
        helicopterRearPropellerAnim.changeCoordinates(xCoordinate + offsetXRearPropeller, yCoordinate + offsetYRearPropeller);
    }
    
    
    /**
     * Draws helicopter to the screen.
     * 
     * @param g2d Graphics2D
     */
     void Draw(Graphics2D g2d)
    { 
        helicopterFrontPropellerAnim.Draw(g2d);
        g2d.drawImage(helicopterBodyImg, xCoordinate, yCoordinate, null);
        helicopterRearPropellerAnim.Draw(g2d);
    }
    
}
#endif

