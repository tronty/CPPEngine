/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

/**
 * Landing area where rocket will land.
 * 
 * @author www.gametutorial.net
 */

 struct LandingArea {
    
    /**
     * X coordinate of the landing area.
     */
     int x;
    /**
     * Y coordinate of the landing area.
     */
     int y;
    
    /**
     * Image of landing area.
     */
     TextureID landingAreaImg;
    
    /**
     * Width of landing area.
     */
     int landingAreaImgWidth;
    
    
     LandingArea()
    {
        Initialize();
        LoadContent();
    }
    
    
     void Initialize()
    {   
        // X coordinate of the landing area is at 46% frame width.
        x = (int)(Framework.frameWidth * 0.46);
        // Y coordinate of the landing area is at 86% frame height.
        y = (int)(Framework.frameHeight * 0.88);
    }
    
     void LoadContent()
    {
        landingArea =   IRenderer::GetRendererInstance()->addImageLibTexture("/moon_lander/landing_area.png");
        landingAreaImgWidth = landingAreaImg.getWidth();        
    }
    
    
     void Draw(Graphics2D g2d)
    {
        g2d.drawImage(landingAreaImg, x, y, null);
    }
    
}
#endif

