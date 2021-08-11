/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Create a JPanel on which we draw and listen for keyboard and mouse events.
 * 
 * @author www.gametutorial.net
 */

  struct Canvas extends JPanel implements KeyListener, MouseListener {
    
    // Keyboard states - Here are stored states for keyboard keys - is it down or not.
      bool keyboardState[525];
    
    // Mouse states - Here are stored states for mouse keys - is it down or not.
      bool mouseState[3];
        

     Canvas()
    {
        // We use double buffer to draw on the screen.
        this->setDoubleBuffered(true);
        this->setFocusable(true);
        this->setBackground(Color.black);
        
        // If you will draw your own mouse cursor or if you just want that mouse cursor disapear, 
        // insert "true" into if condition and mouse cursor will be removed.
        if(true)
        {
            Image3 blankCursorImg = Image3(16, 16, Image3.TYPE_INT_ARGB);
            Cursor blankCursor = Toolkit.getDefaultToolkit().createCustomCursor(blankCursorImg,  Point(0, 0), null);
            this->setCursor(blankCursor);
        }
        
        // Adds the keyboard listener to JPanel to receive key events from this component.
        this->addKeyListener(this);
        // Adds the mouse listener to JPanel to receive mouse events from this component.
        this->addMouseListener(this);
    }
    
    
    // This method is overridden in Framework.java and is used for drawing to the screen.
      void Draw(Graphics2D g2d);
    
    
     void paintComponent(Graphics g)
    {
        Graphics2D g2d = (Graphics2D)g;        
        super.paintComponent(g2d);        
        Draw(g2d);
    }
       
    
    // Keyboard
    /**
     * Is keyboard key "key" down?
     * 
     * @param key Number of key for which you want to check the state.
     * @return true if the key is down, false if the key is not down.
     */
      bool keyboardKeyState(int key)
    {
        return keyboardState[key];
    }
    
    // Methods of the keyboard listener.
    
     void keyPressed(KeyEvent e) 
    {
        keyboardState[e.getKeyCode()] = true;
    }
    
    
     void keyReleased(KeyEvent e)
    {
        keyboardState[e.getKeyCode()] = false;
        keyReleasedFramework(e);
    }
    
    
     void keyTyped(KeyEvent e) { }
    
      void keyReleasedFramework(KeyEvent e);
    
    
    // Mouse
    /**
     * Is mouse button "button" down?
     * Parameter "button" can be "MouseEvent.BUTTON1" - Indicates mouse button #1
     * or "MouseEvent.BUTTON2" - Indicates mouse button #2 ...
     * 
     * @param button Number of mouse button for which you want to check the state.
     * @return true if the button is down, false if the button is not down.
     */
      bool mouseButtonState(int button)
    {
        return mouseState[button - 1];
    }
    
    // Sets mouse key status.
     void mouseKeyStatus(MouseEvent e, bool status)
    {
        if(e.getButton() == MouseEvent.BUTTON1)
            mouseState[0] = status;
        else if(e.getButton() == MouseEvent.BUTTON2)
            mouseState[1] = status;
        else if(e.getButton() == MouseEvent.BUTTON3)
            mouseState[2] = status;
    }
    
    // Methods of the mouse listener.
    
     void mousePressed(MouseEvent e)
    {
        mouseKeyStatus(e, true);
    }
    
    
     void mouseReleased(MouseEvent e)
    {
        mouseKeyStatus(e, false);
    }
    
    
     void mouseClicked(MouseEvent e) { }
    
    
     void mouseEntered(MouseEvent e) { }
    
    
     void mouseExited(MouseEvent e) { }
    
}
#endif

