#ifndef __Game__
#define __Game__
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "PlayerHelicopter.h"
#include "EnemyHelicopter.h"
#include "Bullet.h"

/**
 * Actual game.
 * 
 * @author www.gametutorial.net
 */

 struct Game {
    
    // Use this to generate a random number.
     Random random;
    
    // We will use this for seting mouse position.
     Robot robot;
    
    // Player - helicopter that is managed by player.
     PlayerHelicopter player;
    
    // Enemy helicopters.
     std::vector<EnemyHelicopter> enemyHelicopterList;
    
    // Explosions
     std::vector<Animation> explosionsList;
     TextureID explosionAnimImg;
    
    // List of all the machine gun bullets.
     std::vector<Bullet> bulletsList;
    
    // List of all the rockets.
     std::vector<Rocket> rocketsList;
    // List of all the rockets smoke.
     std::vector<RocketSmoke> rocketSmokeList;
    
    // Image for the sky color.
     TextureID skyColorImg;
    
    // Images for white spot on the sky.
     TextureID cloudLayer1Img;
     TextureID cloudLayer2Img;
    // Images for mountains and ground.
     TextureID mountainsImg;
     TextureID groundImg;
    
    // Objects of moving images.
     MovingBackground cloudLayer1Moving;
     MovingBackground cloudLayer2Moving;
     MovingBackground mountainsMoving;
     MovingBackground groundMoving;
    
    // Image of mouse cursor.
     TextureID mouseCursorImg;
    
    // Font that we will use to write statistic to the screen.
     Font font;
    
    // Statistics (destroyed enemies, run away enemies)
     int runAwayEnemies;
     int destroyedEnemies;
    

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
        
        try {
            robot = Robot();
        } catch (AWTException ex) {
            Logger.getLogger(Game.struct.getName()).log(Level.SEVERE, null, ex);
        }
        
        player = PlayerHelicopter(Framework.frameWidth / 4, Framework.frameHeight / 4);
        
        enemyHelicopterList.clear();
        
        explosionsList.clear();
        
        bulletsList.clear();
        
        rocketsList.clear();
        rocketSmokeList.clear();
        
        // Moving images.
        cloudLayer1Moving = MovingBackground();
        cloudLayer2Moving = MovingBackground();
        mountainsMoving = MovingBackground();
        groundMoving = MovingBackground();
        
        runAwayEnemies = 0;
        destroyedEnemies = 0;
    }
    
    /**
     * Load game files (images).
     */
     void LoadContent()
    {
            // Images of environment
            skyColor = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/sky_color.jpg");
            cloudLayer1 = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/cloud_layer_1.png");
            cloudLayer2 = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/cloud_layer_2.png");
            mountains = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/mountains.png");
            ground = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/ground.png");
            
            // Load images for enemy helicopter
            helicopterBody = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/2_helicopter_body.png");
            helicopterFrontPropellerAnim = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/2_front_propeller_anim.png");
            helicopterRearPropellerAnim = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/2_rear_propeller_anim.png");
            
            // Images of rocket and its smoke.
            rocket = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/rocket.png");
            rocketSmoke = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/rocket_smoke.png");
            
            // Imege of explosion animation.
            explosionAnim = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/explosion_anim.png");
            
            // Image of mouse cursor.
            mouseCursor = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/mouse_cursor.png");
            
            // Helicopter machine gun bullet.
            bullet = IRenderer::GetRendererInstance()->addImageLibTexture("/helicopterbattle/bullet.png");
        
        // Now that we have images we initialize moving images.
        cloudLayer1Moving.Initialize(cloudLayer1Img, -6, 0);
        cloudLayer2Moving.Initialize(cloudLayer2Img, -2, 0);
        mountainsMoving.Initialize(mountainsImg, -1, Framework.frameHeight - groundImg.getHeight() - mountainsImg.getHeight() + 40);
        groundMoving.Initialize(groundImg, -1.2, Framework.frameHeight - groundImg.getHeight());
    }
    
    
    /**
     * Restart game - reset some variables.
     */
     void RestartGame()
    {
        player.Reset(Framework.frameWidth / 4, Framework.frameHeight / 4);
        
        EnemyHelicopter.restartEnemy();
        
        Bullet.timeOfLastCreatedBullet = 0;
        Rocket.timeOfLastCreatedRocket = 0;
        
        // Empty all the lists.
        enemyHelicopterList.clear();
        bulletsList.clear();
        rocketsList.clear();
        rocketSmokeList.clear();
        explosionsList.clear();
        
        // Statistics
        runAwayEnemies = 0;
        destroyedEnemies = 0;
    }
    
    
    /**
     * Update game logic.
     * 
     * @param gameTime The elapsed game time in nanoseconds.
     * @param mousePosition current mouse position.
     */
     void UpdateGame(long gameTime, Point mousePosition)
    {
        /* Player */
        // When player is destroyed and all explosions are finished showing we change game status.
        if( !isPlayerAlive() && explosionsList.isEmpty() ){
            Framework.gameState = Framework.GameState.GAMEOVER;
            return; // If player is destroyed, we don't need to do thing below.
        }
        // When a player is out of rockets and machine gun bullets, and all lists 
        // of bullets, rockets and explosions are empyt(end showing) we finish the game.
        if(player.numberOfAmmo <= 0 && 
           player.numberOfRockets <= 0 && 
           bulletsList.isEmpty() && 
           rocketsList.isEmpty() && 
           explosionsList.isEmpty())
        {
            Framework.gameState = Framework.GameState.GAMEOVER;
            return;
        }
        // If player is alive we update him.
        if(isPlayerAlive()){
            isPlayerShooting(gameTime, mousePosition);
            didPlayerFiredRocket(gameTime);
            player.isMoving();
            player.Update();
        }
        
        /* Mouse */
        limitMousePosition(mousePosition);
        
        /* Bullets */
        updateBullets();
        
        /* Rockets */
        updateRockets(gameTime); // It also checks for collisions (if any of the rockets hit any of the enemy helicopter).
        updateRocketSmoke(gameTime);
        
        /* Enemies */
        createEnemyHelicopter(gameTime);
        updateEnemies();
        
        /* Explosions */
        updateExplosions();
    }
    
    /**
     * Draw the game to the screen.
     * 
     * @param g2d Graphics2D
     * @param mousePosition current mouse position.
     */
     void Draw(Graphics2D g2d, Point mousePosition, long gameTime)
    {
        // Image for background sky color.
        g2d.drawImage(skyColorImg, 0, 0, Framework.frameWidth, Framework.frameHeight, null);
        
        // Moving images.
        mountainsMoving.Draw(g2d);
        groundMoving.Draw(g2d);
        cloudLayer2Moving.Draw(g2d);
        
        if(isPlayerAlive())
            player.Draw(g2d);
        
        // Draws all the enemies.
        for(int i = 0; i < enemyHelicopterList.size(); i++)
        {
            enemyHelicopterList.get(i).Draw(g2d);
        }
        
        // Draws all the bullets. 
        for(int i = 0; i < bulletsList.size(); i++)
        {
            bulletsList.get(i).Draw(g2d);
        }
        
        // Draws all the rockets. 
        for(int i = 0; i < rocketsList.size(); i++)
        {
            rocketsList.get(i).Draw(g2d);
        }
        // Draws smoke of all the rockets.
        for(int i = 0; i < rocketSmokeList.size(); i++)
        {
            rocketSmokeList.get(i).Draw(g2d);
        }
        
        // Draw all explosions.
        for(int i = 0; i < explosionsList.size(); i++)
        {
            explosionsList.get(i).Draw(g2d);
        }
        
        // Draw statistics
        g2d.setFont(font);
        g2d.setColor(Color.darkGray);
        
        g2d.drawString(formatTime(gameTime), Framework.frameWidth/2 - 45, 21);
        g2d.drawString("DESTROYED: " + destroyedEnemies, 10, 21);
        g2d.drawString("RUNAWAY: "   + runAwayEnemies,   10, 41);
        g2d.drawString("ROCKETS: "   + player.numberOfRockets, 10, 81);
        g2d.drawString("AMMO: "      + player.numberOfAmmo, 10, 101);
        
        // Moving images. We draw this cloud in front of the helicopter.
        cloudLayer1Moving.Draw(g2d);
        
        // Mouse cursor
        if(isPlayerAlive())
            drawRotatedMouseCursor(g2d, mousePosition);
    }
    
    /**
     * Draws some game statistics when game is over.
     * 
     * @param g2d Graphics2D
     * @param gameTime Elapsed game time.
     */
     void DrawStatistic(Graphics2D g2d, long gameTime){
        g2d.drawString("Time: " + formatTime(gameTime),                Framework.frameWidth/2 - 50, Framework.frameHeight/3 + 80);
        g2d.drawString("Rockets left: "      + player.numberOfRockets, Framework.frameWidth/2 - 55, Framework.frameHeight/3 + 105);
        g2d.drawString("Ammo left: "         + player.numberOfAmmo,    Framework.frameWidth/2 - 55, Framework.frameHeight/3 + 125);
        g2d.drawString("Destroyed enemies: " + destroyedEnemies,       Framework.frameWidth/2 - 65, Framework.frameHeight/3 + 150);
        g2d.drawString("Runaway enemies: "   + runAwayEnemies,         Framework.frameWidth/2 - 65, Framework.frameHeight/3 + 170);
        g2d.setFont(font);
        g2d.drawString("Statistics: ",                                 Framework.frameWidth/2 - 75, Framework.frameHeight/3 + 60);
    }
    
    /**
     * Draws rotated mouse cursor.
     * It rotates the cursor image on the basis of the player helicopter machine gun.
     * 
     * @param g2d Graphics2D
     * @param mousePosition Position of the mouse.
     */
     void drawRotatedMouseCursor(Graphics2D g2d, Point mousePosition)
    {
        double RIGHT_ANGLE_RADIANS = Math.PI / 2;
        
        // Positon of the player helicopter machine gun.
        int pivotX = player.machineGunXcoordinate;
        int pivotY = player.machineGunYcoordinate;
        
        int a = pivotX - mousePosition.x;
        int b = pivotY - mousePosition.y;
        double ab = (double)a / (double)b;
        double alfaAngleRadians = Math.atan(ab);

        if(mousePosition.y < pivotY) // Above the helicopter.
            alfaAngleRadians = RIGHT_ANGLE_RADIANS - alfaAngleRadians - RIGHT_ANGLE_RADIANS*2;
        else if (mousePosition.y > pivotY) // Under the helicopter.
            alfaAngleRadians = RIGHT_ANGLE_RADIANS - alfaAngleRadians;
        else
            alfaAngleRadians = 0;

        AffineTransform origXform = g2d.getTransform();
        AffineTransform newXform = (AffineTransform)(origXform.clone());

        newXform.rotate(alfaAngleRadians, mousePosition.x, mousePosition.y);
        g2d.setTransform(newXform);
        
        g2d.drawImage(mouseCursorImg, mousePosition.x, mousePosition.y - mouseCursorImg.getHeight()/2, null); // We substract half of the cursor image so that will be drawn in center of the y mouse coordinate.
        
        g2d.setTransform(origXform);
    }
    
    /**
     * Format given time into 00:00 format.
     * 
     * @param time Time that is in nanoseconds.
     * @return Time in 00:00 format.
     */
      String formatTime(long time){
            // Given time in seconds.
            int sec = (int)(time / Framework.milisecInNanosec / 1000);

            // Given time in minutes and seconds.
            int min = sec / 60;
            sec = sec - (min * 60);

            String minString, secString;

            if(min <= 9)
                minString = "0" + Integer.toString(min);
            else
                minString = "" + Integer.toString(min);

            if(sec <= 9)
                secString = "0" + Integer.toString(sec);
            else
                secString = "" + Integer.toString(sec);

            return minString + ":" + secString;
    }
    
    
    
    
    
    /*
     * 
     * Methods for updating the game. 
     * 
     */
    
     
    /**
     * Check if player is alive. If not, set game over status.
     * 
     * @return True if player is alive, false otherwise.
     */
     bool isPlayerAlive()
    {
        if(player.health <= 0)
            return false;
        
        return true;
    }
    
    /**
     * Checks if the player is shooting with the machine gun and creates bullets if he shooting.
     * 
     * @param gameTime Game time.
     */
     void isPlayerShooting(long gameTime, Point mousePosition)
    {
        if(player.isShooting(gameTime))
        {
            Bullet.timeOfLastCreatedBullet = gameTime;
            player.numberOfAmmo--;
            
            Bullet b = Bullet(player.machineGunXcoordinate, player.machineGunYcoordinate, mousePosition);
            bulletsList.add(b);
        }
    }
    
    /**
     * Checks if the player is fired the rocket and creates it if he did.
     * It also checks if player can fire the rocket.
     * 
     * @param gameTime Game time.
     */
     void didPlayerFiredRocket(long gameTime)
    {
        if(player.isFiredRocket(gameTime))
        {
            Rocket.timeOfLastCreatedRocket = gameTime;
            player.numberOfRockets--;
            
            Rocket r = Rocket();
            r.Initialize(player.rocketHolderXcoordinate, player.rocketHolderYcoordinate);
            rocketsList.add(r);
        }
    }
    
    /**
     * Creates a new enemy if it's time.
     * 
     * @param gameTime Game time.
     */
     void createEnemyHelicopter(long gameTime)
    {
        if(gameTime - EnemyHelicopter.timeOfLastCreatedEnemy >= EnemyHelicopter.timeBetweenNewEnemies)
        {
            EnemyHelicopter eh = EnemyHelicopter();
            int xCoordinate = Framework.frameWidth;
            int yCoordinate = random.nextInt(Framework.frameHeight - EnemyHelicopter.helicopterBodyImg.getHeight());
            eh.Initialize(xCoordinate, yCoordinate);
            // Add created enemy to the list of enemies.
            enemyHelicopterList.add(eh);
            
            // Speed up enemy speed and aperence.
            EnemyHelicopter.speedUp();
            
            // Sets new time for last created enemy.
            EnemyHelicopter.timeOfLastCreatedEnemy = gameTime;
        }
    }
    
    /**
     * Updates all enemies.
     * Move the helicopter and checks if he left the screen.
     * Updates helicopter animations.
     * Checks if enemy was destroyed.
     * Checks if any enemy collision with player.
     */
     void updateEnemies()
    {
        for(int i = 0; i < enemyHelicopterList.size(); i++)
        {
            EnemyHelicopter eh = enemyHelicopterList.get(i);
            
            eh.Update();
            
            // Is chrashed with player?
            Rectangle playerRectangel = Rectangle(player.xCoordinate, player.yCoordinate, player.helicopterBodyImg.getWidth(), player.helicopterBodyImg.getHeight());
            Rectangle enemyRectangel = Rectangle(eh.xCoordinate, eh.yCoordinate, EnemyHelicopter.helicopterBodyImg.getWidth(), EnemyHelicopter.helicopterBodyImg.getHeight());
            if(playerRectangel.intersects(enemyRectangel)){
                player.health = 0;
                
                // Remove helicopter from the list.
                enemyHelicopterList.remove(i);
                
                // Add explosion of player helicopter.
                for(int exNum = 0; exNum < 3; exNum++){
                    Animation expAnim = Animation(explosionAnimImg, 134, 134, 12, 45, false, player.xCoordinate + exNum*60, player.yCoordinate - random.nextInt(100), exNum * 200 +random.nextInt(100));
                    explosionsList.add(expAnim);
                }
                // Add explosion of enemy helicopter.
                for(int exNum = 0; exNum < 3; exNum++){
                    Animation expAnim = Animation(explosionAnimImg, 134, 134, 12, 45, false, eh.xCoordinate + exNum*60, eh.yCoordinate - random.nextInt(100), exNum * 200 +random.nextInt(100));
                    explosionsList.add(expAnim);
                }
                
                // Because player crashed with enemy the game will be over so we don't need to check other enemies.
                break;
            }
            
            // Check health.
            if(eh.health <= 0){
                // Add explosion of helicopter.
                Animation expAnim = Animation(explosionAnimImg, 134, 134, 12, 45, false, eh.xCoordinate, eh.yCoordinate - explosionAnimImg.getHeight()/3, 0); // Substring 1/3 explosion image height (explosionAnimImg.getHeight()/3) so that explosion is drawn more at the center of the helicopter.
                explosionsList.add(expAnim);

                // Increase the destroyed enemies counter.
                destroyedEnemies++;
                
                // Remove helicopter from the list.
                enemyHelicopterList.remove(i);
                
                // Helicopter was destroyed so we can move to next helicopter.
                continue;
            }
            
            // If the current enemy is left the screen we remove him from the list and update the runAwayEnemies variable.
            if(eh.isLeftScreen())
            {
                enemyHelicopterList.remove(i);
                runAwayEnemies++;
            }
        }
    }
    
    /**
     * Update bullets. 
     * It moves bullets.
     * Checks if the bullet is left the screen.
     * Checks if any bullets is hit any enemy.
     */
     void updateBullets()
    {
        for(int i = 0; i < bulletsList.size(); i++)
        {
            Bullet bullet = bulletsList.get(i);
            
            // Move the bullet.
            bullet.Update();
            
            // Is left the screen?
            if(bullet.isItLeftScreen()){
                bulletsList.remove(i);
                // Bullet have left the screen so we removed it from the list and now we can continue to the next bullet.
                continue;
            }
            
            // Did hit any enemy?
            // Rectangle of the bullet image.
            Rectangle bulletRectangle = Rectangle((int)bullet.xCoordinate, (int)bullet.yCoordinate, Bullet.bulletImg.getWidth(), Bullet.bulletImg.getHeight());
            // Go trough all enemis.
            for(int j = 0; j < enemyHelicopterList.size(); j++)
            {
                EnemyHelicopter eh = enemyHelicopterList.get(j);

                // Current enemy rectangle.
                Rectangle enemyRectangel = Rectangle(eh.xCoordinate, eh.yCoordinate, EnemyHelicopter.helicopterBodyImg.getWidth(), EnemyHelicopter.helicopterBodyImg.getHeight());

                // Is current bullet over currnet enemy?
                if(bulletRectangle.intersects(enemyRectangel))
                {
                    // Bullet hit the enemy so we reduce his health.
                    eh.health -= Bullet.damagePower;
                    
                    // Bullet was also destroyed so we remove it.
                    bulletsList.remove(i);
                    
                    // That bullet hit enemy so we don't need to check other enemies.
                    break;
                }
            }
        }
    }

    /**
     * Update rockets. 
     * It moves rocket and add smoke behind it.
     * Checks if the rocket is left the screen.
     * Checks if any rocket is hit any enemy.
     * 
     * @param gameTime Game time.
     */
     void updateRockets(long gameTime)
    {
        for(int i = 0; i < rocketsList.size(); i++)
        {
            Rocket rocket = rocketsList.get(i);
            
            // Moves the rocket.
            rocket.Update();
            
            // Checks if it is left the screen.
            if(rocket.isItLeftScreen())
            {
                rocketsList.remove(i);
                // Rocket left the screen so we removed it from the list and now we can continue to the next rocket.
                continue;
            }
            
            // Creates a rocket smoke.
            RocketSmoke rs = RocketSmoke();
            int xCoordinate = rocket.xCoordinate - RocketSmoke.smokeImg.getWidth(); // Subtract the size of the rocket smoke image (rocketSmokeImg.getWidth()) so that smoke isn't drawn under/behind the image of rocket.
            int yCoordinte = rocket.yCoordinate - 5 + random.nextInt(6); // Subtract 5 so that smok will be at the middle of the rocket on y coordinate. We rendomly add a number between 0 and 6 so that the smoke line isn't straight line.
            rs.Initialize(xCoordinate, yCoordinte, gameTime, rocket.currentSmokeLifeTime);
            rocketSmokeList.add(rs);
            
            // Because the rocket is fast we get empty space between smokes so we need to add more smoke. 
            // The higher is the speed of rockets, the bigger are empty spaces.
            int smokePositionX = 5 + random.nextInt(8); // We will draw this smoke a little bit ahead of the one we draw before.
            rs = RocketSmoke();
            xCoordinate = rocket.xCoordinate - RocketSmoke.smokeImg.getWidth() + smokePositionX; // Here we need to add so that the smoke will not be on the same x coordinate as previous smoke. First we need to add 5 because we add random number from 0 to 8 and if the random number is 0 it would be on the same coordinate as smoke before.
            yCoordinte = rocket.yCoordinate - 5 + random.nextInt(6); // Subtract 5 so that smok will be at the middle of the rocket on y coordinate. We rendomly add a number between 0 and 6 so that the smoke line isn't straight line.
            rs.Initialize(xCoordinate, yCoordinte, gameTime, rocket.currentSmokeLifeTime);
            rocketSmokeList.add(rs);
            
            // Increase the life time for the next piece of rocket smoke.
            rocket.currentSmokeLifeTime *= 1.02;
            
            // Checks if current rocket hit any enemy.
            if( checkIfRocketHitEnemy(rocket) )
                // Rocket was also destroyed so we remove it.
                rocketsList.remove(i);
        }
    }
    
    /**
     * Checks if the given rocket is hit any of enemy helicopters.
     * 
     * @param rocket Rocket to check.
     * @return True if it hit any of enemy helicopters, false otherwise.
     */
     bool checkIfRocketHitEnemy(Rocket rocket)
    {
        bool didItHitEnemy = false;
        
        // Current rocket rectangle. // I inserted number 2 insted of rocketImg.getWidth() because I wanted that rocket 
        // is over helicopter when collision is detected, because actual image of helicopter isn't a rectangle shape. (We could calculate/make 3 areas where helicopter can be hit and checks these areas, but this is easier.)
        Rectangle rocketRectangle = Rectangle(rocket.xCoordinate, rocket.yCoordinate, 2, Rocket.rocketImg.getHeight());
        
        // Go trough all enemis.
        for(int j = 0; j < enemyHelicopterList.size(); j++)
        {
            EnemyHelicopter eh = enemyHelicopterList.get(j);

            // Current enemy rectangle.
            Rectangle enemyRectangel = Rectangle(eh.xCoordinate, eh.yCoordinate, EnemyHelicopter.helicopterBodyImg.getWidth(), EnemyHelicopter.helicopterBodyImg.getHeight());

            // Is current rocket over currnet enemy?
            if(rocketRectangle.intersects(enemyRectangel))
            {
                didItHitEnemy = true;
                
                // Rocket hit the enemy so we reduce his health.
                eh.health -= Rocket.damagePower;
                
                // Rocket hit enemy so we don't need to check other enemies.
                break;
            }
        }
        
        return didItHitEnemy;
    }
    
    /**
     * Updates smoke of all the rockets.
     * If the life time of the smoke is over then we delete it from list.
     * It also changes a transparency of a smoke image, so that smoke slowly disappear.
     * 
     * @param gameTime Game time.
     */
     void updateRocketSmoke(long gameTime)
    {
        for(int i = 0; i < rocketSmokeList.size(); i++)
        {
            RocketSmoke rs = rocketSmokeList.get(i);
            
            // Is it time to remove the smoke.
            if(rs.didSmokeDisapper(gameTime))
                rocketSmokeList.remove(i);
            
            // Set new transparency of rocket smoke image.
            rs.updateTransparency(gameTime);
        }
    }
    
    /**
     * Updates all the animations of an explosion and remove the animation when is over.
     */
     void updateExplosions()
    {
        for(int i = 0; i < explosionsList.size(); i++)
        {
            // If the animation is over we remove it from the list.
            if(!explosionsList.get(i).active)
                explosionsList.remove(i);
        }
    }
    
    /**
     * It limits the distance of the mouse from the player.
     * 
     * @param mousePosition Position of the mouse.
     */
     void limitMousePosition(Point mousePosition)
    {
        // Max distance from the player on y coordinate above player helicopter.
        int maxYcoordinateDistanceFromPlayer_top = 30;
        // Max distance from the player on y coordinate under player helicopter.
        int maxYcoordinateDistanceFromPlayer_bottom = 120;
        
        // Mouse cursor will always be the same distance from the player helicopter machine gun on the x coordinate.
        int mouseXcoordinate = player.machineGunXcoordinate + 250;
        
        // Here we will limit the distance of mouse cursor on the y coordinate.
        int mouseYcoordinate = mousePosition.y;
        if(mousePosition.y < player.machineGunYcoordinate){ // Above the helicopter machine gun.
            if(mousePosition.y < player.machineGunYcoordinate - maxYcoordinateDistanceFromPlayer_top)
                mouseYcoordinate = player.machineGunYcoordinate - maxYcoordinateDistanceFromPlayer_top;
        } else { // Under the helicopter.
            if(mousePosition.y > player.machineGunYcoordinate + maxYcoordinateDistanceFromPlayer_bottom)
                mouseYcoordinate = player.machineGunYcoordinate + maxYcoordinateDistanceFromPlayer_bottom;
        }
        
        // We move mouse on y coordinate with helicopter. That makes shooting easier.
        mouseYcoordinate += player.movingYspeed;
        
        // Move the mouse.
        robot.mouseMove(mouseXcoordinate, mouseYcoordinate);
    }
}
#endif

