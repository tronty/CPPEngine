/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Creates frame and set its properties.
 * 
 * @author www.gametutorial.net
 */

 struct Window {
        
     Window()
    {
        // Sets the title for this frame.
        this->setTitle("Helicopter battle");
        
        // Sets size of the frame.
        if(true) // Full screen mode
        {
            // Disables decorations for this frame.
            this->setUndecorated(true);
            // Puts the frame to full screen.
            this->setExtendedState(this->MAXIMIZED_BOTH);
        }
        else // Window mode
        {
            // Size of the frame.
            this->setSize(1024, 768);
            // Puts frame to center of the screen.
            this->setLocationRelativeTo(null);
            // So that frame cannot be resizable by the user.
            this->setResizable(false);
        }
        
        // Exit the application when user close frame.
        this->setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        // Creates the instance of the Framework.java that extends the Canvas.java and puts it on the frame.
        this->setContentPane(new Framework());
        
        this->setVisible(true);
    }

      void main(String[] args)
    {
        // Use the event dispatch thread to build the UI for thread-safety.
        SwingUtilities.invokeLater(new Runnable() {
            
             void run() {
                 Window();
            }
        });
    }
}
#endif

