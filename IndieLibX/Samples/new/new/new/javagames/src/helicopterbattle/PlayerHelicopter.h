#ifndef __PlayerHelicopter__
#define __PlayerHelicopter__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "Animation.h"

/**
 * Helicopter which is managed by player.
 * 
 * @author www.gametutorial.net
 */

 struct PlayerHelicopter {
    
    // Health of the helicopter.
     const int healthInit = 100;
     int health;
    
    // Position of the helicopter on the screen.
     int xCoordinate;
     int yCoordinate;
    
    // Moving speed and also direction.
     double movingXspeed;
     double movingYspeed;
     double acceleratingXspeed;
     double acceleratingYspeed;
     double stoppingXspeed;
     double stoppingYspeed;
    
    // Helicopter rockets.
     const int numberOfRocketsInit = 80;
     int numberOfRockets;
    
    // Helicopter machinegun ammo.
     const int numberOfAmmoInit = 1400;
     int numberOfAmmo;
    
    // Images of helicopter and its propellers.
     TextureID helicopterBodyImg;
     TextureID helicopterFrontPropellerAnimImg;
     TextureID helicopterRearPropellerAnimImg;
    
    // Animation of the helicopter propeller.
     Animation helicopterFrontPropellerAnim;
     Animation helicopterRearPropellerAnim;
    // Offset for the propeler. We add offset to the position of the position of helicopter.
     int offsetXFrontPropeller;
     int offsetYFrontPropeller;
     int offsetXRearPropeller;
     int offsetYRearPropeller;
    
    // Offset of the helicopter rocket holder.
     int offsetXRocketHolder;
     int offsetYRocketHolder;
    // Position on the frame/window of the helicopter rocket holder.
     int rocketHolderXcoordinate;
     int rocketHolderYcoordinate;
    
    // Offset of the helicopter machine gun. We add offset to the position of the position of helicopter.
     int offsetXMachineGun;
     int offsetYMachineGun;
    // Position on the frame/window of the helicopter machine gun.
     int machineGunXcoordinate;
     int machineGunYcoordinate;
    
    
    /**
     * Creates object of player.
     * 
     * @param xCoordinate Starting x coordinate of helicopter.
     * @param yCoordinate Starting y coordinate of helicopter.
     */
     PlayerHelicopter(int xCoordinate, int yCoordinate)
    {
        this->xCoordinate = xCoordinate;
        this->yCoordinate = yCoordinate;
        
        LoadContent();
        Initialize();
    }
    
    
    /**
     * Set variables and objects for this struct.
     */
     void Initialize()
    {
        this->health = healthInit;
        
        this->numberOfRockets = numberOfRocketsInit;
        this->numberOfAmmo = numberOfAmmoInit;
        
        this->movingXspeed = 0;
        this->movingYspeed = 0;
        this->acceleratingXspeed = 0.2;
        this->acceleratingYspeed = 0.2;
        this->stoppingXspeed = 0.1;
        this->stoppingYspeed = 0.1;

        this->offsetXFrontPropeller = 70;
        this->offsetYFrontPropeller = -23;        
        this->offsetXRearPropeller = -6;
        this->offsetYRearPropeller = -21;
        
        this->offsetXRocketHolder = 138;
        this->offsetYRocketHolder = 40;
        this->rocketHolderXcoordinate = this->xCoordinate + this->offsetXRocketHolder;
        this->rocketHolderYcoordinate = this->yCoordinate + this->offsetYRocketHolder;
        
        this->offsetXMachineGun = helicopterBodyImg.getWidth() - 40;
        this->offsetYMachineGun = helicopterBodyImg.getHeight();
        this->machineGunXcoordinate = this->xCoordinate + this->offsetXMachineGun;
        this->machineGunYcoordinate = this->yCoordinate + this->offsetYMachineGun;
    }
    
    /**
     * Load files for this struct.
     */
     void LoadContent()
    {
        helicopterBody = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/1_helicopter_body.png");
            
        helicopterFrontPropellerAnim = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/1_front_propeller_anim.png");
            
        helicopterRearPropellerAnim = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/1_rear_propeller_anim_blur.png");
        
        // Now that we have images of propeller animation we initialize animation object.
        helicopterFrontPropellerAnim = Animation(helicopterFrontPropellerAnimImg, 204, 34, 3, 20, true, xCoordinate + offsetXFrontPropeller, yCoordinate + offsetYFrontPropeller, 0);
        helicopterRearPropellerAnim = Animation(helicopterRearPropellerAnimImg, 54, 54, 4, 20, true, xCoordinate + offsetXRearPropeller, yCoordinate + offsetYRearPropeller, 0);
    }
    
    
    /**
     * Resets the player.
     * 
     * @param xCoordinate Starting x coordinate of helicopter.
     * @param yCoordinate Starting y coordinate of helicopter.
     */
     void Reset(int xCoordinate, int yCoordinate)
    {
        this->health = healthInit;
        
        this->numberOfRockets = numberOfRocketsInit;
        this->numberOfAmmo = numberOfAmmoInit;
        
        this->xCoordinate = xCoordinate;
        this->yCoordinate = yCoordinate;
        
        this->machineGunXcoordinate = this->xCoordinate + this->offsetXMachineGun;
        this->machineGunYcoordinate = this->yCoordinate + this->offsetYMachineGun;
        
        this->movingXspeed = 0;
        this->movingYspeed = 0;
    }
    
    
    /**
     * Checks if player is shooting. It also checks if player can 
     * shoot (time between bullets, does a player have any bullet left).
     * 
     * @param gameTime The current elapsed game time in nanoseconds.
     * @return true if player is shooting.
     */
     bool isShooting(long gameTime)
    {
        // Checks if left mouse button is down && if it is the time for a new bullet.
        if( Canvas.mouseButtonState(MouseEvent.BUTTON1) && 
            ((gameTime - Bullet.timeOfLastCreatedBullet) >= Bullet.timeBetweenNewBullets) &&
            this->numberOfAmmo > 0) 
        {
            return true;
        } else
            return false;
    }
    
    
    /**
     * Checks if player is fired a rocket. It also checks if player can 
     * fire a rocket (time between rockets, does a player have any rocket left).
     * 
     * @param gameTime The current elapsed game time in nanoseconds.
     * @return true if player is fired a rocket.
     */
     bool isFiredRocket(long gameTime)
    {
        // Checks if right mouse button is down && if it is the time for new rocket && if he has any rocket left.
        if( Canvas.mouseButtonState(MouseEvent.BUTTON3) && 
            ((gameTime - Rocket.timeOfLastCreatedRocket) >= Rocket.timeBetweenNewRockets) && 
            this->numberOfRockets > 0 ) 
        {
            return true;
        } else
            return false;
    }
    
    
    /**
     * Checks if player moving helicopter and sets its moving speed if player is moving.
     */
     void isMoving()
    {
        // Moving on the x coordinate.
        if(Canvas.keyboardKeyState(KeyEvent.VK_D) || Canvas.keyboardKeyState(KeyEvent.VK_RIGHT))
            movingXspeed += acceleratingXspeed;
        else if(Canvas.keyboardKeyState(KeyEvent.VK_A) || Canvas.keyboardKeyState(KeyEvent.VK_LEFT))
            movingXspeed -= acceleratingXspeed;
        else    // Stoping
            if(movingXspeed < 0)
                movingXspeed += stoppingXspeed;
            else if(movingXspeed > 0)
                movingXspeed -= stoppingXspeed;
        
        // Moving on the y coordinate.
        if(Canvas.keyboardKeyState(KeyEvent.VK_W) || Canvas.keyboardKeyState(KeyEvent.VK_UP))
            movingYspeed -= acceleratingYspeed;
        else if(Canvas.keyboardKeyState(KeyEvent.VK_S) || Canvas.keyboardKeyState(KeyEvent.VK_DOWN))
            movingYspeed += acceleratingYspeed;
        else    // Stoping
            if(movingYspeed < 0)
                movingYspeed += stoppingYspeed;
            else if(movingYspeed > 0)
                movingYspeed -= stoppingYspeed;
    }
    
    
    /**
     * Updates position of helicopter, animations.
     */
     void Update()
    {
        // Move helicopter and its propellers.
        xCoordinate += movingXspeed;
        yCoordinate += movingYspeed;
        helicopterFrontPropellerAnim.changeCoordinates(xCoordinate + offsetXFrontPropeller, yCoordinate + offsetYFrontPropeller);
        helicopterRearPropellerAnim.changeCoordinates(xCoordinate + offsetXRearPropeller, yCoordinate + offsetYRearPropeller);
        
        // Change position of the rocket holder.
        this->rocketHolderXcoordinate = this->xCoordinate + this->offsetXRocketHolder;
        this->rocketHolderYcoordinate = this->yCoordinate + this->offsetYRocketHolder;
        
        // Move the machine gun with helicopter.
        this->machineGunXcoordinate = this->xCoordinate + this->offsetXMachineGun;
        this->machineGunYcoordinate = this->yCoordinate + this->offsetYMachineGun;
    }
    
    
    /**
     * Draws helicopter to the screen.
     * 
     * @param g2d Graphics2D
     */
     void Draw(Graphics2D g2d)
    {
        helicopterFrontPropellerAnim.Draw(g2d);
        helicopterRearPropellerAnim.Draw(g2d);
        g2d.drawImage(helicopterBodyImg, xCoordinate, yCoordinate, null);
    }
    
}
#endif

