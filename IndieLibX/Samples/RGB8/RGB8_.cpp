/*
  Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

	/* because of malloc() etc. */
int ApplicationLogic() {
	/* First we initialize the library. */
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture");
	unsigned int w, h;
	/* Do not forget that... */
	/* We want all images to be loaded in a consistent manner */
	/* In the next section, we load one image */
	Image3 img;
	bool loaded=img.loadImageLibImage(stx_convertpath(
	"/test.bmp"
	//"/AdvAniCh12/Particle_People1.png"
	));
	if (loaded == false)
		return -1; /* error encountered during loading */
	unsigned char* data=img.getPixels();
	/* Let’s spy on it a little bit */
	w = img.getWidth () ;
	// getting image width
	h = img.getHeight();
	// and height
	LOG_PRINT("Our image resolution: %dx%d\n", w, h);
	/* how much memory will we need? */
	int memory_needed = w * h * 3 * sizeof(	unsigned char);
	/* We multiply by 3 here because we want 3 components per pixel */
	/* finally get the image data */
	/* We want to do something with the image, right? */
	int i;
	for(i = 0; i < memory_needed; i++)
	if(i % 31 == 0) /* vandalise the image */
	data[i] = i % 255;
	/* And maybe we want to save that all... */
	/* and dump them to the disc... */
	/* Finally, clean the mess! */
	img.saveImageLibImage("our_result.png");
	return 0;
}

