#ifndef __MovingBackground__
#define __MovingBackground__
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Moving background image.
 * Move the image left or right in a loop.
 * 
 * @author www.gametutorial.net
 */

 struct MovingBackground {

    // Image of background
     Image3 image;

    // Speed of backgrount
     double speed;

    // Positions of background
     double xPositions[];
     int yPosition;


    /**
     * Initialize object for moving image.
     * 
     * @param image Image that will be moving.
     * @param speed How fast and into which direction should the image move? If number is negative, image will move into left. If you use decimal number, in some cases image can cover one over another at the end point, or could be spaces between images, so try another number.
     * @param yPosition y coordinate of the image.
     */
     void Initialize(Image3 image, double speed, int yPosition)
    {
        this->image = image;
        this->speed = speed;

        this->yPosition = yPosition;

        // We divide frame size with image size do that we get how many times we need to draw image to screen.
        int numberOfPositions = (Framework.frameWidth / this->image.getWidth()) + 2; // We need to add 2 so that we don't get blank spaces between images.
        xPositions = double[numberOfPositions];

        // Set x coordinate for each image that we need to draw.
        for (int i = 0; i < xPositions.length; i++)
        {
            xPositions[i] = i * image.getWidth();
        }
    }


    /**
     * Moves images.
     */
     void Update()
    {
        for (int i = 0; i < xPositions.length; i++)
        {
            // Move image
            xPositions[i] += speed;

            // If image moving left
            if (speed < 0)
            {
                // If image is out of the screen, it moves it to the end of line of images.
                if (xPositions[i] <= -image.getWidth())
                {
                    xPositions[i] = image.getWidth() * (xPositions.length - 1);
                }
            }
            // If image moving right
            else
            {
                // If image is out of the screen, it moves it to the end of line of images.
                if (xPositions[i] >= image.getWidth() * (xPositions.length - 1))
                {
                    xPositions[i] = -image.getWidth();
                }
            }
        }
    }

    /**
     * Draw images to the screen.
     * 
     * @param g2d Graphics2D
     */
     void Draw(Graphics2D g2d)
    {
        this->Update();
        
        for (int i = 0; i < xPositions.length; i++)
        {
            g2d.drawImage(image, (int)xPositions[i], yPosition, null);
        }
    }
}
#endif

