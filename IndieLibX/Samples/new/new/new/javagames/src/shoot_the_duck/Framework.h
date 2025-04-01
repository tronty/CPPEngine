/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Framework that controls the game (Game.java) that created it, update it and draw it on the screen.
 * 
 * @author www.gametutorial.net
 */

 struct Framework extends Canvas {
    
    /**
     * Width of the frame.
     */
      int frameWidth;
    /**
     * Height of the frame.
     */
      int frameHeight;

    /**
     * Time of one second in nanoseconds.
     * 1 second = 1 000 000 000 nanoseconds
     */
      const long secInNanosec = 1000000000L;
    
    /**
     * Time of one millisecond in nanoseconds.
     * 1 millisecond = 1 000 000 nanoseconds
     */
      const long milisecInNanosec = 1000000L;
    
    /**
     * FPS - Frames per second
     * How many times per second the game should update?
     */
     const int GAME_FPS = 60;
    /**
     * Pause between updates. It is in nanoseconds.
     */
     const long GAME_UPDATE_PERIOD = secInNanosec / GAME_FPS;
    
    /**
     * Possible states of the game
     */
      enum GameState{STARTING, VISUALIZING, GAME_CONTENT_LOADING, MAIN_MENU, OPTIONS, PLAYING, GAMEOVER, DESTROYED}
    /**
     * Current state of the game
     */
      GameState gameState;
    
    /**
     * Elapsed game time in nanoseconds.
     */
     long gameTime;
    // It is used for calculating elapsed time.
     long lastTime;
    
    // The actual game
     Game game;
    
    
    /**
     * Image for menu.
     */
     TextureID shootTheDuck;    
    
    
     void Framework ()
    {
        super();
        
        gameState = GameState.VISUALIZING;
            
             void run(){
                GameLoop();
            }
        };
    }
    
    
   /**
     * Set variables and objects.
     * This method is intended to set the variables and objects for this struct, variables and objects for the actual game can be set in Game.java.
     */
     void Initialize()
    {

    }
    
    /**
     * Load files - images, sounds, ...
     * This method is intended to load files for this struct, files for the actual game can be loaded in Game.java.
     */
     void LoadContent()
    {
        shootTheDuck = IRenderer::GetRendererInstance()->addImageLibTexture("/shoot_the_duck/menu.jpg");
    }
    
    /**
     * In specific intervals of time (GAME_UPDATE_PERIOD) the game/logic is updated and then the game is drawn on the screen.
     */
     void GameLoop()
    {
        // This two variables are used in VISUALIZING state of the game. We used them to wait some time so that we get correct frame/window resolution.
        long visualizingTime = 0, lastVisualizingTime = System.nanoTime();
        
        // This variables are used for calculating the time that defines for how long we should put threat to sleep to meet the GAME_FPS.
        long beginTime, timeTaken, timeLeft;
        
        while(true)
        {
            beginTime = System.nanoTime();
            
            switch (gameState)
            {
                case PLAYING:
                    gameTime += System.nanoTime() - lastTime;
                    
                    game.UpdateGame(gameTime, mousePosition());
                    
                    lastTime = System.nanoTime();
                break;
                case GAMEOVER:
                    //...
                break;
                case MAIN_MENU:
                    //...
                break;
                case OPTIONS:
                    //...
                break;
                case GAME_CONTENT_LOADING:
                    //...
                break;
                case STARTING:
                    // Sets variables and objects.
                    Initialize();
                    // Load files - images, sounds, ...
                    LoadContent();

                    // When all things that are called above finished, we change game status to main menu.
                    gameState = GameState.MAIN_MENU;
                break;
                case VISUALIZING:
                    // On Ubuntu OS (when I tested on my old computer) this->getWidth() method doesn't return the correct value immediately (eg. for frame that should be 800px width, returns 0 than 790 and at last 798px). 
                    // So we wait one second for the window/frame to be set to its correct size. Just in case we
                    // also insert 'this->getWidth() > 1' condition in case when the window/frame size wasn't set in time,
                    // so that we although get approximately size.
                    if(this->getWidth() > 1 && visualizingTime > secInNanosec)
                    {
                        frameWidth = this->getWidth();
                        frameHeight = this->getHeight();

                        // When we get size of frame we change status.
                        gameState = GameState.STARTING;
                    }
                    else
                    {
                        visualizingTime += System.nanoTime() - lastVisualizingTime;
                        lastVisualizingTime = System.nanoTime();
                    }
                break;
            }
            
            // Repaint the screen.
            repaint();
            
            // Here we calculate the time that defines for how long we should put threat to sleep to meet the GAME_FPS.
            timeTaken = System.nanoTime() - beginTime;
            timeLeft = (GAME_UPDATE_PERIOD - timeTaken) / milisecInNanosec; // In milliseconds
            // If the time is less than 10 milliseconds, then we will put thread to sleep for 10 millisecond so that some other thread can do some work.
            if (timeLeft < 10) 
                timeLeft = 10; //set a minimum
            try {
                 //Provides the necessary delay and also yields control so that other thread can do work.
		 STX_Service::GetTimerInstance()->stx_Delay(timeLeft,0);
            } catch (InterruptedException ex) { }
        }
    }
    
    /**
     * Draw the game to the screen. It is called through repaint() method in GameLoop() method.
     */
    
     void Draw(Graphics2D g2d)
    {
        switch (gameState)
        {
            case PLAYING:
                game.Draw(g2d, mousePosition());
            break;
            case GAMEOVER:
                game.DrawGameOver(g2d, mousePosition());
            break;
            case MAIN_MENU:
                g2d.drawImage(shootTheDuckMenuImg, 0, 0, frameWidth, frameHeight, null);
                g2d.drawString("Use left mouse button to shot the duck.", frameWidth / 2 - 83, (int)(frameHeight * 0.65));   
                g2d.drawString("Click with left mouse button to start the game.", frameWidth / 2 - 100, (int)(frameHeight * 0.67));                
                g2d.drawString("Press ESC any time to exit the game.", frameWidth / 2 - 75, (int)(frameHeight * 0.70));
                g2d.setColor(Color.white);
                g2d.drawString("WWW.GAMETUTORIAL.NET", 7, frameHeight - 5);
            break;
            case OPTIONS:
                //...
            break;
            case GAME_CONTENT_LOADING:
                g2d.setColor(Color.white);
                g2d.drawString("GAME is LOADING", frameWidth / 2 - 50, frameHeight / 2);
            break;
        }
    }
    
    /**
     * Starts new game.
     */
     void newGame()
    {
        // We set gameTime to zero and lastTime to current time for later calculations.
        gameTime = 0;
        lastTime = System.nanoTime();
        
        game = Game();
    }
    
    /**
     *  Restart game - reset game time and call RestartGame() method of game object so that reset some variables.
     */
     void restartGame()
    {
        // We set gameTime to zero and lastTime to current time for later calculations.
        gameTime = 0;
        lastTime = System.nanoTime();
        
        game.RestartGame();
        
        // We change game status so that the game can start.
        gameState = GameState.PLAYING;
    }
    
    /**
     * Returns the position of the mouse pointer in game frame/window.
     * If mouse position is null than this method return 0,0 coordinate.
     * 
     * @return Point of mouse coordinates.
     */
     Point mousePosition()
    {
        try
        {
            Point mp = this->getMousePosition();
            
            if(mp != null)
                return this->getMousePosition();
            else
                return  Point(0, 0);
        }
        catch (Exception e)
        {
            return  Point(0, 0);
        }
    }
    
    /**
     * This method is called when keyboard key is released.
     * 
     * @param e KeyEvent
     */
    
     void keyReleasedFramework(KeyEvent e)
    {
        switch (gameState)
        {
            case GAMEOVER:
                if(e.getKeyCode() == KeyEvent.VK_ESCAPE)
                    System.exit(0);
                else if(e.getKeyCode() == KeyEvent.VK_SPACE || e.getKeyCode() == KeyEvent.VK_ENTER)
                    restartGame();
            break;
            case PLAYING:
            case MAIN_MENU:
                if(e.getKeyCode() == KeyEvent.VK_ESCAPE)
                    System.exit(0);
            break;
        }
    }
    
    /**
     * This method is called when mouse button is clicked.
     * 
     * @param e MouseEvent
     */
    
     void mouseClicked(MouseEvent e)
    {
        switch (gameState)
        {
            case MAIN_MENU:
                if(e.getButton() == MouseEvent.BUTTON1)
                    newGame();
            break;
        }
    }
}
#endif

